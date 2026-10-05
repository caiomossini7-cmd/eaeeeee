#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

#include "WebRemote.hpp"
#include <Cheat/Options.hpp>
#include <FrameWork/Dependencies/NlohmannJson.hpp>
#include <sstream>
#include <vector>
#include <iostream>

using json = nlohmann::json;

namespace Cheat
{
    std::atomic<bool> WebRemote::m_Running = false;
    std::thread WebRemote::m_Thread;

    // Basic HTML content for the remote control - Using escaped strings to avoid raw string literal issues
    const char* HTML_CONTENT = 
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>Relikia Remote Control</title>\n"
        "    <style>\n"
        "        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f0f0f; color: #e0e0e0; margin: 0; padding: 20px; }\n"
        "        .container { max-width: 600px; margin: auto; background: #1a1a1a; padding: 20px; border-radius: 12px; border: 1px solid #333; box-shadow: 0 4px 15px rgba(0,0,0,0.5); }\n"
        "        h1 { text-align: center; color: #80bfff; font-size: 24px; margin-bottom: 20px; text-transform: uppercase; letter-spacing: 2px; }\n"
        "        .section { margin-bottom: 25px; padding-bottom: 15px; border-bottom: 1px solid #333; }\n"
        "        .section-title { font-weight: bold; color: #80bfff; margin-bottom: 15px; font-size: 18px; }\n"
        "        .control-group { display: flex; align-items: center; justify-content: space-between; margin-bottom: 12px; }\n"
        "        .label { font-size: 14px; }\n"
        "        .switch { position: relative; display: inline-block; width: 44px; height: 22px; }\n"
        "        .switch input { opacity: 0; width: 0; height: 0; }\n"
        "        .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #333; transition: .3s; border-radius: 22px; }\n"
        "        .slider:before { position: absolute; content: \"\"; height: 16px; width: 16px; left: 3px; bottom: 3px; background-color: white; transition: .3s; border-radius: 50%; }\n"
        "        input:checked + .slider { background-color: #80bfff; }\n"
        "        input:checked + .slider:before { transform: translateX(22px); }\n"
        "        input[type=range] { -webkit-appearance: none; width: 120px; background: transparent; }\n"
        "        input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; height: 16px; width: 16px; border-radius: 50%; background: #80bfff; cursor: pointer; margin-top: -6px; }\n"
        "        input[type=range]::-webkit-slider-runnable-track { width: 100%; height: 4px; cursor: pointer; background: #333; border-radius: 2px; }\n"
        "        .status-bar { text-align: center; font-size: 12px; color: #666; margin-top: 20px; }\n"
        "        .refresh-btn { display: block; width: 100%; padding: 10px; background: #333; border: none; color: white; border-radius: 6px; cursor: pointer; margin-top: 10px; transition: .2s; }\n"
        "        .refresh-btn:hover { background: #444; }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <div class=\"container\">\n"
        "        <h1>Relikia Remote</h1>\n"
        "        <div class=\"section\">\n"
        "            <div class=\"section-title\">LegitBot</div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">Aimbot Enabled</span>\n"
        "                <label class=\"switch\"><input type=\"checkbox\" id=\"aimbot_enabled\" onchange=\"update('aimbot_enabled')\"><span class=\"slider\"></span></label>\n"
        "            </div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">Silent Aim Enabled</span>\n"
        "                <label class=\"switch\"><input type=\"checkbox\" id=\"silent_enabled\" onchange=\"update('silent_enabled')\"><span class=\"slider\"></span></label>\n"
        "            </div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">Aimbot FOV</span>\n"
        "                <input type=\"range\" id=\"aimbot_fov\" min=\"0\" max=\"800\" onchange=\"update('aimbot_fov')\">\n"
        "            </div>\n"
        "        </div>\n"
        "        <div class=\"section\">\n"
        "            <div class=\"section-title\">Visuals</div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">Player ESP</span>\n"
        "                <label class=\"switch\"><input type=\"checkbox\" id=\"esp_players\" onchange=\"update('esp_players')\"><span class=\"slider\"></span></label>\n"
        "            </div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">Vehicle ESP</span>\n"
        "                <label class=\"switch\"><input type=\"checkbox\" id=\"esp_vehicles\" onchange=\"update('esp_vehicles')\"><span class=\"slider\"></span></label>\n"
        "            </div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">ESP Max Distance</span>\n"
        "                <input type=\"range\" id=\"esp_distance\" min=\"0\" max=\"1000\" onchange=\"update('esp_distance')\">\n"
        "            </div>\n"
        "        </div>\n"
        "        <div class=\"section\">\n"
        "            <div class=\"section-title\">Misc</div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">God Mode</span>\n"
        "                <label class=\"switch\"><input type=\"checkbox\" id=\"godmode\" onchange=\"update('godmode')\"><span class=\"slider\"></span></label>\n"
        "            </div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">Noclip</span>\n"
        "                <label class=\"switch\"><input type=\"checkbox\" id=\"noclip\" onchange=\"update('noclip')\"><span class=\"slider\"></span></label>\n"
        "            </div>\n"
        "            <div class=\"control-group\">\n"
        "                <span class=\"label\">Infinite Ammo</span>\n"
        "                <label class=\"switch\"><input type=\"checkbox\" id=\"inf_ammo\" onchange=\"update('inf_ammo')\"><span class=\"slider\"></span></label>\n"
        "            </div>\n"
        "        </div>\n"
        "        <button class=\"refresh-btn\" onclick=\"refresh()\">Refresh State</button>\n"
        "        <div class=\"status-bar\" id=\"status\">Connected to Localhost</div>\n"
        "    </div>\n"
        "    <script>\n"
        "        function refresh() {\n"
        "            fetch('/api/options')\n"
        "                .then(r => r.json())\n"
        "                .then(data => {\n"
        "                    document.getElementById('aimbot_enabled').checked = data.legitbot_aimbot_enabled;\n"
        "                    document.getElementById('silent_enabled').checked = data.legitbot_silent_enabled;\n"
        "                    document.getElementById('aimbot_fov').value = data.legitbot_aimbot_fov;\n"
        "                    document.getElementById('esp_players').checked = data.visuals_esp_players_enabled;\n"
        "                    document.getElementById('esp_vehicles').checked = data.visuals_esp_vehicles_enabled;\n"
        "                    document.getElementById('esp_distance').value = data.visuals_esp_players_distance;\n"
        "                    document.getElementById('godmode').checked = data.misc_exploits_godmode;\n"
        "                    document.getElementById('noclip').checked = data.misc_exploits_noclip;\n"
        "                    document.getElementById('inf_ammo').checked = data.misc_exploits_infammo;\n"
        "                    document.getElementById('status').innerText = 'Last updated: ' + new Date().toLocaleTimeString();\n"
        "                })\n"
        "                .catch(e => {\n"
        "                    document.getElementById('status').innerText = 'Connection Error!';\n"
        "                    document.getElementById('status').style.color = '#ff6b6b';\n"
        "                });\n"
        "        }\n"
        "        function update(key) {\n"
        "            let data = {};\n"
        "            const el = document.getElementById(key);\n"
        "            data[key] = el.type === 'checkbox' ? el.checked : parseInt(el.value);\n"
        "            fetch('/api/options', {\n"
        "                method: 'POST',\n"
        "                headers: { 'Content-Type': 'application/json' },\n"
        "                body: JSON.stringify(data)\n"
        "            }).then(() => {\n"
        "                document.getElementById('status').innerText = 'Setting updated!';\n"
        "                document.getElementById('status').style.color = '#80bfff';\n"
        "                setTimeout(() => { document.getElementById('status').style.color = '#666'; }, 2000);\n"
        "            });\n"
        "        }\n"
        "        refresh();\n"
        "        setInterval(refresh, 2000);\n"
        "    </script>\n"
        "</body>\n"
        "</html>";

    void WebRemote::Start()
    {
        if (m_Running) return;
        m_Running = true;
        m_Thread = std::thread(ServerThread);
    }

    void WebRemote::Stop()
    {
        m_Running = false;
        if (m_Thread.joinable())
            m_Thread.detach(); // We don't want to block main thread for socket timeouts
    }

    bool WebRemote::IsRunning()
    {
        return m_Running;
    }

    std::string WebRemote::GetURL()
    {
        if (!m_Running) return "";

        char name[256];
        if (gethostname(name, sizeof(name)) == SOCKET_ERROR)
            return "http://127.0.0.1:" + std::to_string(g_Options.General.WebRemotePort);

        struct hostent* host = gethostbyname(name);
        if (!host)
            return "http://127.0.0.1:" + std::to_string(g_Options.General.WebRemotePort);

        struct in_addr addr;
        memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));
        
        return "http://" + std::string(inet_ntoa(addr)) + ":" + std::to_string(g_Options.General.WebRemotePort);
    }

    void WebRemote::ServerThread()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return;

        SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET) { WSACleanup(); return; }

        // Set socket as non-blocking so we can check m_Running
        u_long mode = 1;
        ioctlsocket(listenSocket, FIONBIO, &mode);

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons((u_short)g_Options.General.WebRemotePort);

        if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        while (m_Running)
        {
            SOCKET clientSocket = accept(listenSocket, NULL, NULL);
            if (clientSocket == INVALID_SOCKET)
            {
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }
                break;
            }

            // Set a timeout for recv to avoid hanging the thread
            DWORD timeout = 2000; // 2 seconds
            setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

            char buffer[4096] = { 0 };
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived > 0)
            {
                std::string request(buffer, bytesReceived);
                std::string response = HandleRequest(request);
                send(clientSocket, response.c_str(), (int)response.length(), 0);
            }
            closesocket(clientSocket);
        }

        closesocket(listenSocket);
        WSACleanup();
    }

    std::string WebRemote::HandleRequest(const std::string& request)
    {
        std::string response;
        if (request.find("GET /api/options") != std::string::npos)
        {
            std::string jsonStr = GetOptionsJson();
            response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(jsonStr.length()) + "\r\nAccess-Control-Allow-Origin: *\r\n\r\n" + jsonStr;
        }
        else if (request.find("POST /api/options") != std::string::npos)
        {
            size_t bodyPos = request.find("\r\n\r\n");
            if (bodyPos != std::string::npos)
            {
                std::string body = request.substr(bodyPos + 4);
                UpdateOptionsFromJson(body);
            }
            response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
        }
        else if (request.find("GET /") != std::string::npos)
        {
            std::string html = HTML_CONTENT;
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(html.length()) + "\r\n\r\n" + html;
        }
        else
        {
            response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        }
        return response;
    }

    std::string WebRemote::GetOptionsJson()
    {
        json j;
        j["legitbot_aimbot_enabled"] = g_Options.LegitBot.AimBot.Enabled;
        j["legitbot_silent_enabled"] = g_Options.LegitBot.SilentAim.Enabled;
        j["legitbot_aimbot_fov"] = g_Options.LegitBot.AimBot.FOV;
        j["visuals_esp_players_enabled"] = g_Options.Visuals.ESP.Players.Enabled;
        j["visuals_esp_vehicles_enabled"] = g_Options.Visuals.ESP.Vehicles.Enabled;
        j["visuals_esp_players_distance"] = g_Options.Visuals.ESP.Players.RenderDistance;
        j["misc_exploits_godmode"] = g_Options.Misc.Exploits.LocalPlayer.GodMode;
        j["misc_exploits_noclip"] = g_Options.Misc.Exploits.LocalPlayer.Noclip;
        j["misc_exploits_infammo"] = g_Options.Misc.Exploits.Weapon.InfiniteAmmoEnabled;
        return j.dump();
    }

    void WebRemote::UpdateOptionsFromJson(const std::string& json_str)
    {
        try
        {
            json j = json::parse(json_str);
            if (j.contains("aimbot_enabled")) g_Options.LegitBot.AimBot.Enabled = j["aimbot_enabled"];
            if (j.contains("silent_enabled")) g_Options.LegitBot.SilentAim.Enabled = j["silent_enabled"];
            if (j.contains("aimbot_fov")) g_Options.LegitBot.AimBot.FOV = j["aimbot_fov"];
            if (j.contains("esp_players")) g_Options.Visuals.ESP.Players.Enabled = j["esp_players"];
            if (j.contains("esp_vehicles")) g_Options.Visuals.ESP.Vehicles.Enabled = j["esp_vehicles"];
            if (j.contains("esp_distance")) g_Options.Visuals.ESP.Players.RenderDistance = j["esp_distance"];
            if (j.contains("godmode")) g_Options.Misc.Exploits.LocalPlayer.GodMode = j["godmode"];
            if (j.contains("noclip")) g_Options.Misc.Exploits.LocalPlayer.Noclip = j["noclip"];
            if (j.contains("inf_ammo")) g_Options.Misc.Exploits.Weapon.InfiniteAmmoEnabled = j["inf_ammo"];
        }
        catch (...) {}
    }
}
