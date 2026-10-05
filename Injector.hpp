/*
 * Language:    C++17
 * File:        Injector.hpp
 * Environment: Windows 10/11
 * Target:      Auto-inject Dll1.dll into FiveM with Logging
 */

#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace Injector
{
    static constexpr const wchar_t* k_FiveMProcessNames[] = {
        L"FiveM_GTAProcess.exe",
        L"FiveM_b3258_GTAProcess.exe",
        L"FiveM_b3095_GTAProcess.exe",
        L"FiveM_b2372_GTAProcess.exe",
        L"FiveM_b2189_GTAProcess.exe",
        L"GTA5.exe",
        L"fivem.exe"
    };

    static constexpr const char* k_DllName = "Dll1.dll";

    inline void Log(const std::string& msg)
    {
        char exePath[MAX_PATH]{};
        GetModuleFileNameA(nullptr, exePath, MAX_PATH);
        std::string logPath = std::filesystem::path(exePath).parent_path().string() + "\\injector_log.txt";
        std::ofstream log(logPath, std::ios::app);
        if (log.is_open())
        {
            log << msg << "\n";
        }
    }

    inline DWORD FindTargetPID()
    {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE) return 0;

        PROCESSENTRY32W pe{ sizeof(pe) };
        DWORD pid = 0;

        if (Process32FirstW(snap, &pe))
            do {
                for (auto& name : k_FiveMProcessNames)
                {
                    if (_wcsicmp(pe.szExeFile, name) == 0)
                    {
                        pid = pe.th32ProcessID;
                        break;
                    }
                }
            } while (!pid && Process32NextW(snap, &pe));

        CloseHandle(snap);
        return pid;
    }

    inline bool IsAlreadyInjected(DWORD pid)
    {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snap == INVALID_HANDLE_VALUE) return false;

        MODULEENTRY32W me{ sizeof(me) };
        bool found = false;

        if (Module32FirstW(snap, &me))
            do {
                char n[MAX_PATH]{};
                WideCharToMultiByte(CP_ACP, 0, me.szModule, -1, n, sizeof(n), nullptr, nullptr);
                if (_stricmp(n, k_DllName) == 0) { found = true; break; }
            } while (Module32NextW(snap, &me));

        CloseHandle(snap);
        return found;
    }

    inline bool InjectDLL(DWORD pid, const std::string& dllPath)
    {
        HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!hProc)
        {
            Log("[ERRO] OpenProcess falhou. Error code: " + std::to_string(GetLastError()));
            return false;
        }

        LPVOID mem = VirtualAllocEx(hProc, nullptr, dllPath.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!mem)
        {
            Log("[ERRO] VirtualAllocEx falhou.");
            CloseHandle(hProc);
            return false;
        }

        WriteProcessMemory(hProc, mem, dllPath.c_str(), dllPath.size() + 1, nullptr);

        HANDLE hT = CreateRemoteThread(hProc, nullptr, 0,
            (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
            mem, 0, nullptr);

        if (hT)
        {
            WaitForSingleObject(hT, 5000);
            CloseHandle(hT);
            Log("[SUCESSO] DLL injetada com sucesso no PID " + std::to_string(pid));
        }
        else
        {
            Log("[ERRO] CreateRemoteThread falhou. Error code: " + std::to_string(GetLastError()));
        }

        VirtualFreeEx(hProc, mem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return hT != nullptr;
    }

    inline void AutoInjectAsync(int timeoutSeconds = 60)
    {
        std::thread([timeoutSeconds]()
            {
                char exePath[MAX_PATH]{};
                GetModuleFileNameA(nullptr, exePath, MAX_PATH);
                std::string dllFull = std::filesystem::path(exePath).parent_path().string() + "\\" + k_DllName;

                Log("=== Iniciando AutoInject ===");
                Log("Caminho da DLL: " + dllFull);

                if (!std::filesystem::exists(dllFull))
                {
                    Log("[ERRO] Dll1.dll nao foi encontrada na pasta!");
                    return;
                }

                auto start = std::chrono::steady_clock::now();
                while (std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - start).count() < timeoutSeconds)
                {
                    DWORD pid = FindTargetPID();
                    if (pid)
                    {
                        Log("Processo FiveM encontrado! PID: " + std::to_string(pid));
                        if (IsAlreadyInjected(pid))
                        {
                            Log("DLL ja esta injetada.");
                            return;
                        }

                        if (InjectDLL(pid, dllFull)) return;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
                Log("[TIMEOUT] Nao encontrou ou nao conseguiu injetar a DLL no tempo limite.");
            }).detach();
    }
}