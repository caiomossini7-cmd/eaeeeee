/*
 * Language:    C++17 / C++20
 * File:        Interface.cpp
 * Environment: Windows 10/11, DirectX 11, ImGui
 * Target:      Pecinha — Custom Menu UI
 */

#include "Interface.hpp"
#include "EspPreview.hpp"
#include <Cheat/Features/Misc/Exploits.hpp>
#include <cstdio>
#include <atomic>
#include <thread>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <Cheat/Options.hpp>
#include <Cheat/WebRemote.hpp>
#include <Cheat/Cheat.hpp>
#include <Cheat/ConfigSystem.hpp>
#include <game.hpp>
#include <Security/KeyAuth.hpp>
#include <FrameWork/Dependencies/ImGui/imgui_edited.hpp>
#include <FrameWork/Utilities/Notify.hpp>
#include "Logo.hpp"
#include <tchar.h>
#include <shellapi.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

inline Cheat::ConfigManager ConfigManager;

static std::atomic<bool> g_AuthCheckDone{ false };
static std::atomic<bool> g_AuthCheckResult{ false };

struct MarkerT
{
    Vector3D    Position;
    std::string Name;
};

ID3D11ShaderResourceView* Logo = nullptr;
static int                ActiveMenuTab = 3;
static char               SearchBuffer[128] = "";

std::vector<MarkerT> MarkerList;

// ─────────────────────────────────────────────────────────────────────────────
//  Pipe commands — devem ser iguais ao dllmain.cpp
// ─────────────────────────────────────────────────────────────────────────────
#define CMD_GIVE_ALL_WEAPONS  1
#define CMD_GIVE_WEAPON       2
#define CMD_REMOVE_WEAPONS    3

#define PIPE_NAME L"\\\\.\\pipe\\PecinhaPipe"

struct PipePacket
{
    int      Command;
    uint32_t WeaponHash;
    uint64_t PedAddress;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Anonymous helpers
// ─────────────────────────────────────────────────────────────────────────────
namespace
{
    // Envia um comando para a DLL via Named Pipe (passando o Ped local)
    bool SendPipeCommand(int cmd, uint32_t weaponHash = 0)
    {
        HANDLE hPipe = CreateFileW(
            PIPE_NAME,
            GENERIC_WRITE,
            0, nullptr,
            OPEN_EXISTING,
            0, nullptr);

        if (hPipe == INVALID_HANDLE_VALUE)
            return false;

        uint64_t pedAddr = 0;
        if (Cheat::g_Fivem.IsInitialized())
        {
            auto localPed = Cheat::g_Fivem.GetLocalPlayerInfo().Ped;
            if (localPed)
                pedAddr = (uint64_t)localPed;
        }

        PipePacket pkt{ cmd, weaponHash, pedAddr };
        DWORD written = 0;
        WriteFile(hPipe, &pkt, sizeof(pkt), &written, nullptr);
        CloseHandle(hPipe);
        return written == sizeof(pkt);
    }

    void ApplyOverlayWindowStyle(HWND window, LONG exStyle)
    {
        SetWindowLong(window, GWL_EXSTYLE, exStyle);
        SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    }

    
}

// ─────────────────────────────────────────────────────────────────────────────
//  FrameWork namespace
// ─────────────────────────────────────────────────────────────────────────────
namespace FrameWork
{
    void Interface::Initialize(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext)
    {
        hWindow = Window;
        hTargetWindow = TargetWindow;
        IDevice = Device;
        FrameWork::Overlay::g_pd3dDevice = Device;

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hWindow);
        ImGui_ImplDX11_Init(Device, DeviceContext);

        MarkerList.push_back({ Vector3D(212.6681f,   -813.7118f,  30.7386f), XorStr("Meeting Point") });
        MarkerList.push_back({ Vector3D(99.7721f,    -743.7130f,  45.7547f), XorStr("FIB-Tower") });
        MarkerList.push_back({ Vector3D(-1039.2391f, -2666.4702f, 13.8307f), XorStr("Airport") });
        MarkerList.push_back({ Vector3D(3627.5176f,   3754.3137f, 28.5157f), XorStr("Humanlabs") });
        MarkerList.push_back({ Vector3D(1404.8857f,   3162.1936f, 40.4341f), XorStr("Sandyshores Airfield") });

        static std::string cachedUserName = FrameWork::Misc::GetDiscordUsername();
        g_Options.General.UserName = cachedUserName;

        if (cachedUserName == XorStr(".llmig") || cachedUserName == XorStr("coto777"))
            g_Options.General.UserRole = XorStr("Developer");
        else
            g_Options.General.UserRole = XorStr("Client");

        Assets::Initialize(IDevice);
        Cheat::g_EspPreview.CreateTexture(IDevice);
    }

    // ── Sidebar helpers ──────────────────────────────────────────────────────
    static void RenderSidebarCategory(const char* label)
    {
        ImGui::Spacing();
        ImGui::PushFont(Assets::InterBold);
        ImGui::TextColored(ImVec4(0.85f, 0.18f, 0.18f, 1.0f), "%s", label);
        ImGui::PopFont();
        ImGui::Spacing();
    }

    static bool RenderSidebarItem(const char* label, int tabIndex, int& currentTab)
    {
        bool        selected = (currentTab == tabIndex);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2      p = ImGui::GetCursorScreenPos();
        float       w = ImGui::GetContentRegionAvail().x;
        const float h = 32.0f;

        bool clicked = ImGui::InvisibleButton(label, ImVec2(w, h));
        bool hovered = ImGui::IsItemHovered();

        ImU32 bg = selected ? ImColor(50, 10, 10, 240)
            : hovered ? ImColor(30, 10, 10, 180)
            : ImColor(0, 0, 0, 0);
        dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), bg, 5.0f);

        if (selected)
        {
            dl->AddRectFilled(ImVec2(p.x, p.y + 4.0f), ImVec2(p.x + 3.0f, p.y + h - 4.0f), ImColor(255, 35, 35, 255), 2.0f);
            dl->AddRectFilled(ImVec2(p.x + 3.0f, p.y + 6.0f), ImVec2(p.x + 5.0f, p.y + h - 6.0f), ImColor(180, 20, 20, 140), 2.0f);
        }
        else if (hovered)
        {
            dl->AddRectFilled(ImVec2(p.x, p.y + 8.0f), ImVec2(p.x + 2.0f, p.y + h - 8.0f), ImColor(180, 30, 30, 180), 1.0f);
        }

        ImU32 tc = selected ? ImColor(255, 255, 255, 255)
            : hovered ? ImColor(220, 220, 220, 255)
            : ImColor(145, 145, 150, 255);

        ImGui::PushFont(Assets::InterSemiBold);
        dl->AddText(ImVec2(p.x + 16.0f, p.y + 8.0f), tc, label);
        ImGui::PopFont();

        if (clicked) currentTab = tabIndex;
        return clicked;
    }

    // ── Active-features overlay ──────────────────────────────────────────────
    void Interface::RenderActiveFeaturesOverlay()
    {
        

        if (!g_Options.Misc.ShowActiveFeaturesOverlay) return;

        static std::vector<const char*> af;
        af.clear();

        if (g_Options.LegitBot.AimBot.Enabled)              af.push_back("Aimbot");
        if (g_Options.LegitBot.SilentAim.Enabled)           af.push_back("Silent Aim");
        if (g_Options.LegitBot.Trigger.Enabled)             af.push_back("Triggerbot");
        if (g_Options.Visuals.ESP.Players.Enabled)          af.push_back("Player ESP");
        if (g_Options.Visuals.ESP.Vehicles.Enabled)         af.push_back("Vehicle ESP");
        if (g_Options.Misc.Exploits.Vehicle.VehicleGodmode) af.push_back("Veh Godmode");
        
        

        if (af.empty()) return;

        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImVec2      screen = io.DisplaySize;

        const float pad = 10.0f, itemH = 18.0f, headerH = 25.0f;
        float w = 155.0f;

        ImGui::PushFont(Assets::InterSemiBold);
        for (const char* f : af)
        {
            float tw = ImGui::CalcTextSize(f).x + 30.0f;
            if (tw > w) w = tw;
        }
        ImGui::PopFont();

        float  h = headerH + af.size() * itemH + pad;
        ImVec2 pos = ImVec2(15.0f, screen.y * 0.5f - h * 0.5f);

        dl->AddRectFilled(pos, pos + ImVec2(w, h), ImColor(10, 8, 8, 245), 6.0f);
        dl->AddRect(pos, pos + ImVec2(w, h), ImColor(120, 20, 20, 255), 6.0f);

        ImGui::PushFont(Assets::InterBold);
        dl->AddText(pos + ImVec2(pad, 5.0f), ImColor(245, 30, 30, 255), "PECINHA");
        ImGui::PopFont();

        dl->AddRectFilled(pos + ImVec2(pad, headerH - 2.0f), pos + ImVec2(w - pad, headerH), ImColor(245, 30, 30, 255));

        ImGui::PushFont(Assets::InterRegular);
        float cy = pos.y + headerH + 5.0f;
        for (const char* f : af)
        {
            dl->AddCircleFilled(ImVec2(pos.x + 12.0f, cy + 9.0f), 2.2f, ImColor(245, 30, 30, 255));
            dl->AddText(ImVec2(pos.x + 22.0f, cy), ImColor(235, 235, 235, 255), f);
            cy += itemH;
        }
        ImGui::PopFont();
    }

    // ── Style setup ─────────────────────────────────────────────────────────
    void Interface::UpdateStyle()
    {
        ImGuiStyle& s = ImGui::GetStyle();

        s.WindowRounding = 8.0f;
        s.ChildRounding = 6.0f;
        s.FrameRounding = 4.0f;
        s.PopupRounding = 4.0f;
        s.ScrollbarRounding = 4.0f;
        s.GrabRounding = 3.0f;
        s.TabRounding = 4.0f;
        s.WindowPadding = ImVec2(8.0f, 8.0f);
        s.FramePadding = ImVec2(6.0f, 3.0f);
        s.ItemSpacing = ImVec2(6.0f, 4.0f);
        s.ScrollbarSize = 8.0f;

        ImVec4* c = s.Colors;
        c[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.040f, 0.040f, 0.98f);
        c[ImGuiCol_ChildBg] = ImVec4(0.065f, 0.048f, 0.048f, 0.90f);
        c[ImGuiCol_Border] = ImVec4(0.28f, 0.08f, 0.08f, 0.60f);
        c[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.06f, 0.06f, 1.00f);
        c[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.07f, 0.07f, 1.00f);
        c[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.08f, 0.08f, 1.00f);
        c[ImGuiCol_CheckMark] = ImVec4(0.96f, 0.13f, 0.13f, 1.00f);
        c[ImGuiCol_SliderGrab] = ImVec4(0.85f, 0.15f, 0.15f, 1.00f);
        c[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.20f, 0.20f, 1.00f);
        c[ImGuiCol_Button] = ImVec4(0.55f, 0.08f, 0.08f, 0.90f);
        c[ImGuiCol_ButtonHovered] = ImVec4(0.72f, 0.10f, 0.10f, 1.00f);
        c[ImGuiCol_ButtonActive] = ImVec4(0.90f, 0.14f, 0.14f, 1.00f);
        c[ImGuiCol_Header] = ImVec4(0.50f, 0.09f, 0.09f, 0.80f);
        c[ImGuiCol_HeaderHovered] = ImVec4(0.66f, 0.10f, 0.10f, 0.90f);
        c[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.12f, 0.12f, 1.00f);
        c[ImGuiCol_ScrollbarBg] = ImVec4(0.04f, 0.02f, 0.02f, 0.95f);
        c[ImGuiCol_ScrollbarGrab] = ImVec4(0.50f, 0.09f, 0.09f, 0.80f);
        c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.65f, 0.12f, 0.12f, 1.00f);
        c[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.14f, 0.14f, 1.00f);
        c[ImGuiCol_Separator] = ImVec4(0.30f, 0.08f, 0.08f, 0.70f);
        c[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
        c[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.42f, 0.42f, 1.00f);
        c[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.05f, 0.05f, 0.97f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Main render
    // ─────────────────────────────────────────────────────────────────────────
    void Interface::RenderGui()
    {
        RenderActiveFeaturesOverlay();
        if (!bIsMenuOpen) return;

        const ImVec2 WindowSize = ImVec2(850.0f, 540.0f);
        ImGui::SetNextWindowSize(WindowSize, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin(XorStr("##PecinhaMain"), nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::PopStyleVar(2);

        ImDrawList* DrawList = ImGui::GetWindowDrawList();
        ImVec2      WindowPos = ImGui::GetWindowPos();

        DrawList->AddRectFilled(WindowPos, WindowPos + WindowSize, ImColor(14, 10, 10, 252), 10.0f);
        DrawList->AddRect(WindowPos, WindowPos + WindowSize, ImColor(60, 14, 14, 255), 10.0f, 0, 1.5f);

        const float kSidebarW = 195.0f;
        const float kPanelH = WindowSize.y - 66.0f;

        // ── SIDEBAR ──────────────────────────────────────────────────────────
        ImGui::SetCursorPos(ImVec2(10.0f, 10.0f));
        ImGui::BeginChild(XorStr("##Sidebar"), ImVec2(kSidebarW, WindowSize.y - 20.0f), false,
            ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::SetCursorPos(ImVec2(8.0f, 6.0f));
            if (Logo)
            {
                ImGui::Image((void*)Logo, ImVec2(38.0f, 38.0f));
                ImGui::SameLine(50.0f);
            }
            ImGui::BeginGroup();
            {
                ImGui::PushFont(Assets::InterBold);
                ImGui::TextColored(ImVec4(245.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f), "PECINHA");
                ImGui::PopFont();
                ImGui::PushFont(Assets::InterRegular);
                ImGui::TextColored(ImVec4(0.55f, 0.55f, 0.58f, 1.0f), "v2.0 bypass");
                ImGui::PopFont();
            }
            ImGui::EndGroup();

            ImGui::SetCursorPosY(56.0f);

            RenderSidebarCategory("AIM");
            RenderSidebarItem("Aimbot", 0, ActiveMenuTab);
            RenderSidebarItem("Silent Aim", 1, ActiveMenuTab);
            RenderSidebarItem("Triggerbot", 2, ActiveMenuTab);

            RenderSidebarCategory("VISUAL");
            RenderSidebarItem("Players", 3, ActiveMenuTab);
            RenderSidebarItem("Veiculos", 4, ActiveMenuTab);
            RenderSidebarItem("Friends List", 5, ActiveMenuTab);

            RenderSidebarCategory("EXPLOITS");
            RenderSidebarItem("Local Player", 6, ActiveMenuTab);
            RenderSidebarItem("Teleporte", 7, ActiveMenuTab);
            RenderSidebarItem("Veh Exploits", 8, ActiveMenuTab);
            RenderSidebarItem("World Players", 9, ActiveMenuTab);

            RenderSidebarCategory("SISTEMA");
            RenderSidebarItem("Config", 10, ActiveMenuTab);
        }
        ImGui::EndChild();

        DrawList->AddLine(
            WindowPos + ImVec2(kSidebarW + 12.0f, 0.0f),
            WindowPos + ImVec2(kSidebarW + 12.0f, WindowSize.y),
            ImColor(38, 14, 14, 255));

        // ── CONTENT ──────────────────────────────────────────────────────────
        const float contentX = kSidebarW + 22.0f;
        const float contentW = WindowSize.x - contentX - 14.0f;

        ImGui::SetCursorPos(ImVec2(contentX, 10.0f));
        ImGui::BeginGroup();
        ImGui::PushItemWidth(contentW);
        ImGui::InputTextWithHint(XorStr("##Search"), XorStr("  Pesquisar..."), SearchBuffer, sizeof(SearchBuffer));
        ImGui::PopItemWidth();
        ImGui::EndGroup();

        ImGui::SetCursorPos(ImVec2(contentX, 46.0f));
        ImGui::BeginChild(XorStr("##Content"), ImVec2(contentW, kPanelH), false, ImGuiWindowFlags_NoScrollbar);
        {
            // ── TAB 0: AIMBOT ────────────────────────────────────────────────
            if (ActiveMenuTab == 0)
            {
                ImGui::Columns(2, nullptr, false);

                ImGui::CustomChild(XorStr("Geral"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::Checkbox(XorStr("Ativado"), &g_Options.LegitBot.AimBot.Enabled);
                    ImGui::KeyBind(XorStr("Tecla"), &g_Options.LegitBot.AimBot.KeyBind, &g_Options.LegitBot.AimBot.KeyBindState);
                    ImGui::Checkbox(XorStr("Target NPC"), &g_Options.LegitBot.AimBot.TargetNPC);
                    ImGui::Checkbox(XorStr("Visible Check"), &g_Options.LegitBot.AimBot.VisibleCheck);
                    ImGui::Checkbox(XorStr("Ignorar Amigos"), &g_Options.LegitBot.AimBot.IgnoreFriends);
                    
                    ImGui::Spacing();
                    ImGui::SliderInt(XorStr("Smooth X"), &g_Options.LegitBot.AimBot.SmoothHorizontal, 0, 100, "%d");
                    ImGui::SliderInt(XorStr("Smooth Y"), &g_Options.LegitBot.AimBot.SmoothVertical, 0, 100, "%d");
                    ImGui::SliderInt(XorStr("Dist Max"), &g_Options.LegitBot.AimBot.MaxDistance, 0, 600, "%dm");
                }
                ImGui::EndCustomChild();

                ImGui::NextColumn();

                ImGui::CustomChild(XorStr("Config"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::SliderInt(XorStr("FOV"), &g_Options.LegitBot.AimBot.FOV, 0, 800, "%dpx");
                    ImGui::Combo(XorStr("HitBox"), &g_Options.LegitBot.AimBot.HitBox, XorStr("Cabeca\0Pescoco\0Peito\0"));
                    ImGui::Checkbox(XorStr("Mostrar FOV"), &g_Options.Misc.Screen.ShowAimbotFov);
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
            // ── TAB 1: SILENT AIM ────────────────────────────────────────────
            else if (ActiveMenuTab == 1)
            {
                ImGui::Columns(2, nullptr, false);

                ImGui::CustomChild(XorStr("Silent Aim"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::Checkbox(XorStr("Ativado"), &g_Options.LegitBot.SilentAim.Enabled);
                    ImGui::KeyBind(XorStr("Tecla"), &g_Options.LegitBot.SilentAim.KeyBind, &g_Options.LegitBot.SilentAim.KeyBindState);
                    ImGui::Checkbox(XorStr("Atirar NPC"), &g_Options.LegitBot.SilentAim.ShotNPC);
                    ImGui::Checkbox(XorStr("Visible Check"), &g_Options.LegitBot.SilentAim.VisibleCheck);
                    ImGui::Checkbox(XorStr("Ignorar Amigos"), &g_Options.LegitBot.SilentAim.IgnoreFriends);
                    ImGui::Spacing();
                    ImGui::SliderInt(XorStr("FOV"), &g_Options.LegitBot.SilentAim.Fov, 0, 800, "%dpx");
                    ImGui::Checkbox(XorStr("Desenhar FOV (Silent)"), &g_Options.Misc.Screen.ShowSilentAimFov);
                    ImGui::SliderInt(XorStr("Dist Max"), &g_Options.LegitBot.SilentAim.MaxDistance, 0, 600, "%dm");
                    ImGui::SliderInt(XorStr("Miss %"), &g_Options.LegitBot.SilentAim.MissChance, 0, 100, "%d%%");
                    ImGui::Combo(XorStr("HitBox"), &g_Options.LegitBot.SilentAim.HitBox, XorStr("Cabeca\0Pescoco\0Peito\0"));
                }
                ImGui::EndCustomChild();

                ImGui::NextColumn();

                ImGui::CustomChild(XorStr("Magic Bullet"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::Checkbox(XorStr("Ativado"), &g_Options.LegitBot.MagicBullet.Enabled);
                    ImGui::KeyBind(XorStr("Tecla"), &g_Options.LegitBot.MagicBullet.KeyBind, &g_Options.LegitBot.MagicBullet.KeyBindState);
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
            // ── TAB 2: TRIGGERBOT ────────────────────────────────────────────
            else if (ActiveMenuTab == 2)
            {
                ImGui::Columns(2, nullptr, false);

                ImGui::CustomChild(XorStr("Triggerbot"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::Checkbox(XorStr("Ativado"), &g_Options.LegitBot.Trigger.Enabled);
                    ImGui::KeyBind(XorStr("Tecla"), &g_Options.LegitBot.Trigger.KeyBind, &g_Options.LegitBot.Trigger.KeyBindState);
                    ImGui::Checkbox(XorStr("Atirar NPC"), &g_Options.LegitBot.Trigger.ShotNPC);
                    ImGui::Checkbox(XorStr("Visible Check"), &g_Options.LegitBot.Trigger.VisibleCheck);
                    ImGui::Checkbox(XorStr("Ignorar Amigos"), &g_Options.LegitBot.Trigger.IgnoreFriends);
                    ImGui::Spacing();
                    ImGui::SliderInt(XorStr("Dist Max"), &g_Options.LegitBot.Trigger.MaxDistance, 0, 600, "%dm");
                    ImGui::SliderInt(XorStr("Reacao (ms)"), &g_Options.LegitBot.Trigger.ReactionTime, 0, 500, "%dms");
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
            // ── TAB 3: PLAYERS ESP ───────────────────────────────────────────
            else if (ActiveMenuTab == 3)
            {
                const float half = (contentW - 16.0f) * 0.5f;

                ImGui::Columns(2, nullptr, false);

                ImGui::BeginGroup();
                {
                    ImGui::CustomChild(XorStr("Geral"), ImVec2(half, 235));
                    {
                        ImGui::Checkbox(XorStr("ESP Players"), &g_Options.Visuals.ESP.Players.Enabled);
                        ImGui::Checkbox(XorStr("Mostrar NPC"), &g_Options.Visuals.ESP.Players.ShowNPCs);
                        ImGui::Checkbox(XorStr("Visible Only"), &g_Options.Visuals.ESP.Players.VisibleOnly);
                        ImGui::Checkbox(XorStr("Excluir Mortos"), &g_Options.Visuals.ESP.Players.ExcludeDeads);
                        ImGui::Spacing();
                        ImGui::SliderInt(XorStr("Distancia"), &g_Options.Visuals.ESP.Players.RenderDistance, 10, 1000, "%dm");
                        ImGui::SliderFloat(XorStr("Head Size"), &g_Options.General.FontHeight, 0.5f, 5.0f, "%.1f");
                    }
                    ImGui::EndCustomChild();

                    ImGui::Spacing();

                    ImGui::CustomChild(XorStr("Cores"), ImVec2(half, kPanelH - 255));
                    {
                        ImGui::ColorEdit4(XorStr("Box Visivel"), g_Options.Visuals.ESP.Players.BoxColor, ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4(XorStr("Box Invisivel"), g_Options.Visuals.ESP.Players.TextColor, ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4(XorStr("Skel Visivel"), g_Options.Visuals.ESP.Players.SkeletonColor, ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4(XorStr("Skel Invisivel"), g_Options.Visuals.ESP.Players.SnapLinesColor, ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4(XorStr("Head Visivel"), g_Options.Visuals.ESP.Players.FriendColor, ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4(XorStr("Head Invisivel"), g_Options.Visuals.ESP.Players.FriendsSkeletonColor, ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4(XorStr("Barra HP"), g_Options.Visuals.ESP.Players.HealthBarColor, ImGuiColorEditFlags_AlphaBar);
                    }
                    ImGui::EndCustomChild();
                }
                ImGui::EndGroup();

                ImGui::NextColumn();

                ImGui::CustomChild(XorStr("Opcoes"), ImVec2(half, kPanelH - 10));
                {
                    Cheat::g_EspPreview.DragDropHandler();
                    Cheat::g_EspPreview.Draw();

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::Checkbox(XorStr("Caixa"), &g_Options.Visuals.ESP.Players.Box);
                    ImGui::Checkbox(XorStr("Distancia"), &g_Options.Visuals.ESP.Players.Distance);
                    ImGui::Checkbox(XorStr("Esqueleto"), &g_Options.Visuals.ESP.Players.Skeleton);
                    ImGui::Checkbox(XorStr("Head Circle"), &g_Options.Visuals.ESP.Players.Minimap);
                    ImGui::Checkbox(XorStr("Barra HP"), &g_Options.Visuals.ESP.Players.HealthBar);
                    ImGui::Checkbox(XorStr("Barra Armor"), &g_Options.Visuals.ESP.Players.ArmorBar);
                    ImGui::Checkbox(XorStr("Nome"), &g_Options.Visuals.ESP.Players.Name);
                    ImGui::Checkbox(XorStr("Nome Arma"), &g_Options.Visuals.ESP.Players.WeaponName);
                    ImGui::Checkbox(XorStr("Bolhas"), &g_Options.Visuals.ESP.Players.SnapLines);
                    
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
            // ── TAB 4: VEHICLES ESP ──────────────────────────────────────────
            else if (ActiveMenuTab == 4)
            {
                ImGui::Columns(2, nullptr, false);

                ImGui::CustomChild(XorStr("Veiculos ESP"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::Checkbox(XorStr("Ativado"), &g_Options.Visuals.ESP.Vehicles.Enabled);
                    ImGui::Checkbox(XorStr("Modelo"), &g_Options.Visuals.ESP.Vehicles.Model);
                    ImGui::Checkbox(XorStr("Nome"), &g_Options.Visuals.ESP.Vehicles.Name);
                    ImGui::Checkbox(XorStr("Distancia"), &g_Options.Visuals.ESP.Vehicles.Distance);
                    ImGui::Checkbox(XorStr("Marcador"), &g_Options.Visuals.ESP.Vehicles.Marker);
                    ImGui::Checkbox(XorStr("Porta"), &g_Options.Visuals.ESP.Vehicles.Door);
                    ImGui::Checkbox(XorStr("Ignorar Ocupados"), &g_Options.Visuals.ESP.Vehicles.IgnoreOccupiedVehicles);
                    ImGui::Spacing();
                    ImGui::SliderInt(XorStr("Dist Max"), &g_Options.Visuals.ESP.Vehicles.RenderDistance, 10, 1000, "%dm");
                }
                ImGui::EndCustomChild();

                ImGui::NextColumn();

                ImGui::CustomChild(XorStr("Cores"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::ColorEdit4(XorStr("Cor Box"), g_Options.Visuals.ESP.Vehicles.Color, ImGuiColorEditFlags_AlphaBar);
                    ImGui::ColorEdit4(XorStr("Cor Texto"), g_Options.Visuals.ESP.Vehicles.TextColor, ImGuiColorEditFlags_AlphaBar);
                    ImGui::ColorEdit4(XorStr("Cor Marcador"), g_Options.Visuals.ESP.Vehicles.MarkerColor, ImGuiColorEditFlags_AlphaBar);
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
            // ── TAB 5: FRIENDS LIST ──────────────────────────────────────────
            else if (ActiveMenuTab == 5)
            {
                static char FriendInput[64] = "";
                static int  SelFriendIdx = -1;

                ImGui::Columns(2, nullptr, false);

                ImGui::CustomChild(XorStr("Adicionar Amigo"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::InputTextWithHint(XorStr("##FIn"), XorStr("Nome ou ID..."), FriendInput, sizeof(FriendInput));
                    ImGui::Spacing();

                    if (ImGui::Button(XorStr("Adicionar"), ImVec2(ImGui::GetColumnWidth() - 16, 24)))
                    {
                        if (FriendInput[0] != '\0')
                        {
                            std::string name(FriendInput);
                            auto& fl = g_Options.General.FriendsList;
                            if (std::find(fl.begin(), fl.end(), name) == fl.end())
                            {
                                fl.push_back(name);
                                FriendInput[0] = '\0';
                            }
                        }
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::Checkbox(XorStr("Ignorar em Aimbot"), &g_Options.LegitBot.AimBot.IgnoreFriends);
                    ImGui::Checkbox(XorStr("Ignorar em Silent"), &g_Options.LegitBot.SilentAim.IgnoreFriends);
                    ImGui::Checkbox(XorStr("Ignorar em Trigger"), &g_Options.LegitBot.Trigger.IgnoreFriends);
                    
                }
                ImGui::EndCustomChild();

                ImGui::NextColumn();

                ImGui::CustomChild(XorStr("Lista de Amigos"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    auto& fl = g_Options.General.FriendsList;
                    if (fl.empty())
                    {
                        ImGui::TextDisabled(XorStr("Nenhum amigo adicionado."));
                    }
                    else
                    {
                        ImGui::BeginChild(XorStr("##FrList"), ImVec2(0, kPanelH - 90), true);
                        for (int i = 0; i < (int)fl.size(); i++)
                        {
                            ImGui::PushID(i);
                            bool sel = (SelFriendIdx == i);
                            if (ImGui::Selectable(fl[i].c_str(), sel))
                                SelFriendIdx = i;
                            ImGui::PopID();
                        }
                        ImGui::EndChild();

                        ImGui::Spacing();
                        if (SelFriendIdx >= 0 && SelFriendIdx < (int)fl.size())
                        {
                            if (ImGui::Button(XorStr("Remover Selecionado"), ImVec2(ImGui::GetColumnWidth() - 16, 24)))
                            {
                                fl.erase(fl.begin() + SelFriendIdx);
                                SelFriendIdx = -1;
                            }
                        }
                    }
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
            // ── TAB 6: LOCAL PLAYER ──────────────────────────────────────────
            else if (ActiveMenuTab == 6)
            {
                ImGui::Columns(2, nullptr, false);

                ImGui::CustomChild(XorStr("Geral"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::Checkbox(XorStr("God Mode"), &g_Options.Misc.Exploits.LocalPlayer.GodMode);
                    if (g_Options.Misc.Exploits.LocalPlayer.GodMode)
                        ImGui::KeyBind(XorStr("Tecla God"), &g_Options.Misc.Exploits.LocalPlayer.GodModeKey, &g_Options.Misc.Exploits.LocalPlayer.GodModeKeyState);

                    ImGui::Checkbox(XorStr("Semi Godmode"), &g_Options.Misc.Exploits.LocalPlayer.SemiGod);

                    

                    

                    if (ImGui::Checkbox(XorStr("Noclip"), &g_Options.Misc.Exploits.LocalPlayer.Noclip))
                    {
                        auto ped = Cheat::g_Fivem.GetLocalPlayerInfo().Ped;
                        if (ped)
                        {
                            if (g_Options.Misc.Exploits.LocalPlayer.Noclip) ped->invisible_on(true);
                            else                                              ped->invisible_off(true);
                        }
                    }
                    if (g_Options.Misc.Exploits.LocalPlayer.Noclip)
                    {
                        ImGui::KeyBind(XorStr("Tecla Noclip"), &g_Options.Misc.Exploits.LocalPlayer.NoclipKey, &g_Options.Misc.Exploits.LocalPlayer.NoclipKeyState);
                        ImGui::SliderFloat(XorStr("Velocidade Noclip"), &g_Options.Misc.Exploits.LocalPlayer.NoClipSpeed, 1.0f, 100.0f, "%.1f m/s");
                    }

                    ImGui::Checkbox(XorStr("FreeCam"), &g_Options.Misc.Exploits.LocalPlayer.FreeCam);
                    if (g_Options.Misc.Exploits.LocalPlayer.FreeCam)
                    {
                        ImGui::KeyBind(XorStr("Tecla FreeCam"), &g_Options.Misc.Exploits.LocalPlayer.FreeCamKey, &g_Options.Misc.Exploits.LocalPlayer.FreeCamKeyState);
                        ImGui::SliderFloat(XorStr("Velocidade FreeCam"), &g_Options.Misc.Exploits.LocalPlayer.FreeCamSpeed, 1.0f, 100.0f, "%.1f m/s");
                    }

                    ImGui::Checkbox(XorStr("Sem Ragdoll"), &g_Options.Misc.Exploits.LocalPlayer.NoRagdoll);
                    ImGui::Checkbox(XorStr("Stamina Infinita"), &g_Options.Misc.Exploits.LocalPlayer.InfiniteStamina);
                    ImGui::Checkbox(XorStr("Combat Roll Infinito"), &g_Options.Misc.Exploits.LocalPlayer.InfiniteCombatRoll);
                }
                ImGui::EndCustomChild();

                ImGui::NextColumn();

                // ── Painel Extra & Armas ─────────────────────────────────────
                ImGui::CustomChild(XorStr("Extra & Armas"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::Checkbox(XorStr("Cinto de Seguranca"), &g_Options.Misc.Exploits.LocalPlayer.SeatBelt);
                    ImGui::Checkbox(XorStr("Municao Infinita"), &g_Options.Misc.Exploits.Weapon.InfiniteAmmoEnabled);
                    ImGui::Checkbox(XorStr("Sem Spread"), &g_Options.Misc.Exploits.Weapon.RemoveSpread);
                    ImGui::Checkbox(XorStr("Sem Recuo"), &g_Options.Misc.Exploits.Weapon.RemoveRecoil);
                    ImGui::SliderFloat(XorStr("Alcance Arma"), &g_Options.Misc.Exploits.Weapon.WeaponRange, 10.0f, 2000.0f, "%.0fm");
                    ImGui::Checkbox(XorStr("Sem Recarregar"), &g_Options.Misc.Exploits.Weapon.NoReload);
                    ImGui::Checkbox(XorStr("Tiro Duplo"), &g_Options.Misc.Exploits.Weapon.DoubleShot);
                    ImGui::Checkbox(XorStr("Rapid Fire"), &g_Options.Misc.Exploits.Weapon.RapidFire);
                    ImGui::Checkbox(XorStr("Ammo Explosiva"), &g_Options.Misc.Exploits.Weapon.ExplosiveAmmo);

                    ImGui::Spacing();
                    ImGui::Separator();
                    
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
            // ── TAB 7: TELEPORT ──────────────────────────────────────────────
            else if (ActiveMenuTab == 7)
            {
                ImGui::Columns(1, nullptr, false);

                ImGui::CustomChild(XorStr("Teleporte"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    if (ImGui::Button(XorStr("Teleportar ao Waypoint"), ImVec2(ImGui::GetColumnWidth() - 16, 24)))
                        Cheat::Exploits::TeleportToWaypoint();

                    ImGui::Spacing();
                    ImGui::TextDisabled(XorStr("Marcadores"));
                    ImGui::Separator();
                    ImGui::Spacing();

                    static char MarkerNameBuf[64] = "";
                    ImGui::InputTextWithHint(XorStr("##MkName"), XorStr("Nome do marcador..."), MarkerNameBuf, sizeof(MarkerNameBuf));
                    ImGui::Spacing();

                    if (ImGui::Button(XorStr("Salvar Posicao Atual"), ImVec2(ImGui::GetColumnWidth() - 16, 24)))
                    {
                        if (MarkerNameBuf[0] != '\0' && Cheat::g_Fivem.IsInitialized())
                        {
                            MarkerT mk;
                            mk.Position = Cheat::g_Fivem.GetLocalPlayerInfo().WorldPos;
                            mk.Name = MarkerNameBuf;
                            MarkerList.push_back(mk);
                            MarkerNameBuf[0] = '\0';
                        }
                    }

                    ImGui::Spacing();
                    ImGui::BeginChild(XorStr("##MkList"), ImVec2(0, kPanelH - 170), true);
                    for (int i = 0; i < (int)MarkerList.size(); i++)
                    {
                        ImGui::PushID(70000 + i);
                        if (ImGui::Selectable(MarkerList[i].Name.c_str(), false))
                            Cheat::Exploits::TeleportLocalToCoords(MarkerList[i].Position);
                        ImGui::PopID();
                    }
                    ImGui::EndChild();
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
            // ── TAB 8: VEH EXPLOITS ──────────────────────────────────────────
            else if (ActiveMenuTab == 8)
            {
                const float half = (contentW - 16.0f) * 0.5f;
                static int  WorldSelVehicle = -1;

                ImGui::Columns(2, nullptr, false);

                ImGui::BeginGroup();
                {
                    ImGui::CustomChild(XorStr("Veiculo"), ImVec2(half, 255));
                    {
                        if (ImGui::Checkbox(XorStr("Veiculo Godmode"), &g_Options.Misc.Exploits.Vehicle.VehicleGodmode))
                            g_Options.Misc.Exploits.Vehicle.GodMode = g_Options.Misc.Exploits.Vehicle.VehicleGodmode;

                        ImGui::Checkbox(XorStr("Rocket Boost (X)"), &g_Options.Misc.Exploits.Vehicle.RocketBoost);
                        ImGui::Checkbox(XorStr("Salto de Carro (E)"), &g_Options.Misc.Exploits.Vehicle.JumpingCar);
                        ImGui::Checkbox(XorStr("Boost + Salto"), &g_Options.Misc.Exploits.Vehicle.RocketBoostJumping);
                        
                        ImGui::Checkbox(XorStr("Desbloquear Carros"), &g_Options.Misc.Exploits.LocalPlayer.StealCarEnabled);
                        ImGui::Spacing();
                        if (ImGui::Button(XorStr("Reparar Veiculo"), ImVec2(half - 16, 24)))
                            Cheat::Exploits::RepairVehicle();
                        ImGui::KeyBind(XorStr("Tecla Repair"), &g_Options.Misc.Exploits.Vehicle.RepairKey, &g_Options.Misc.Exploits.Vehicle.RepairKeyState);
                    }
                    ImGui::EndCustomChild();

                    ImGui::Spacing();

                    ImGui::CustomChild(XorStr("Cores Customizadas"), ImVec2(half, kPanelH - 275));
                    {
                        ImGui::ColorEdit4(XorStr("Cor Primaria"), g_Options.Misc.Exploits.Vehicle.PrimaryColor, ImGuiColorEditFlags_NoAlpha);
                        ImGui::ColorEdit4(XorStr("Cor Secundaria"), g_Options.Misc.Exploits.Vehicle.SecondaryColor, ImGuiColorEditFlags_NoAlpha);
                        ImGui::ColorEdit4(XorStr("Cor Rodas"), g_Options.Misc.Exploits.Vehicle.WheelColor, ImGuiColorEditFlags_NoAlpha);
                        ImGui::Spacing();
                        if (ImGui::Button(XorStr("Aplicar Cores"), ImVec2(half - 16, 24)))
                            g_Options.Misc.Exploits.Vehicle.UpdateCarColor = true;
                    }
                    ImGui::EndCustomChild();
                }
                ImGui::EndGroup();

                ImGui::NextColumn();

                ImGui::BeginGroup();
                {
                    ImGui::CustomChild(XorStr("Lista de Veiculos"), ImVec2(half, 235));
                    {
                        if (!Cheat::g_Fivem.IsInitialized())
                        {
                            ImGui::TextDisabled(XorStr("Nao conectado..."));
                        }
                        else
                        {
                            auto vList = Cheat::g_Fivem.GetVehicleList();
                            auto localPos = Cheat::g_Fivem.GetLocalPlayerInfo().WorldPos;

                            if (WorldSelVehicle < 0 || WorldSelVehicle >= (int)vList.size())
                                WorldSelVehicle = -1;

                            ImGui::BeginChild(XorStr("##VehInner"), ImVec2(0, 0), true);
                            for (int i = 0; i < (int)vList.size(); i++)
                            {
                                auto& vi = vList[i];
                                if (!vi.Vehicle) continue;
                                float dist = vi.Vehicle->GetCoordinate().DistTo(localPos);
                                char  line[256];
                                std::snprintf(line, sizeof(line), "Veiculo %d | %.0fm", i, dist);

                                ImGui::PushID(93000 + i);
                                if (ImGui::Selectable(line, WorldSelVehicle == i))
                                    WorldSelVehicle = i;
                                ImGui::PopID();
                            }
                            ImGui::EndChild();
                        }
                    }
                    ImGui::EndCustomChild();

                    ImGui::Spacing();

                    ImGui::CustomChild(XorStr("Acoes"), ImVec2(half, kPanelH - 255));
                    {
                        if (Cheat::g_Fivem.IsInitialized())
                        {
                            auto vList = Cheat::g_Fivem.GetVehicleList();
                            if (WorldSelVehicle >= 0 && WorldSelVehicle < (int)vList.size() && vList[WorldSelVehicle].Vehicle)
                            {
                                auto& vi = vList[WorldSelVehicle];
                                if (ImGui::Button(XorStr("Teleportar p/ Veiculo"), ImVec2(half - 16, 24)))
                                    Cheat::Exploits::TeleportLocalToCoords(vi.Vehicle->GetCoordinate());
                                ImGui::Spacing();
                                if (ImGui::Button(XorStr("Destrancar"), ImVec2(half - 16, 24)))
                                    Cheat::Exploits::WorldUnlockVehicle(vi.Vehicle);
                                ImGui::Spacing();
                                if (ImGui::Button(XorStr("Puxar para Mim"), ImVec2(half - 16, 24)))
                                    Cheat::Exploits::WorldPullVehicle(vi.Vehicle);
                            }
                            else
                            {
                                ImGui::TextDisabled(XorStr("Selecione um veiculo acima"));
                            }
                        }
                    }
                    ImGui::EndCustomChild();
                }
                ImGui::EndGroup();

                ImGui::Columns(1);
            }
            // ── TAB 9: WORLD PLAYERS ─────────────────────────────────────────
            else if (ActiveMenuTab == 9)
            {
                static int  WorldSelPlayer = -1;
                const float listH = (kPanelH - 20.0f) * 0.55f;
                const float optH = (kPanelH - 20.0f) * 0.40f;

                ImGui::CustomChild(XorStr("Jogadores"), ImVec2(ImGui::GetColumnWidth() - 8, listH));
                {
                    if (!Cheat::g_Fivem.IsInitialized())
                    {
                        ImGui::TextDisabled(XorStr("Nao conectado..."));
                    }
                    else
                    {
                        auto entities = Cheat::g_Fivem.GetEntitiyList();
                        auto localPos = Cheat::g_Fivem.GetLocalPlayerInfo().WorldPos;

                        if (WorldSelPlayer < 0 || WorldSelPlayer >= (int)entities.size())
                            WorldSelPlayer = -1;

                        ImGui::BeginChild(XorStr("##WPList"), ImVec2(0, 0), true);
                        for (int i = 0; i < (int)entities.size(); i++)
                        {
                            const auto& e = entities[i];
                            float       dist = e.Cordinates.DistTo(localPos);
                            char        line[256];

                            if (e.StaticInfo.bIsLocalPlayer)
                                std::snprintf(line, sizeof(line), "%s | %.0fm | Voce", e.StaticInfo.Name.c_str(), dist);
                            else if (e.StaticInfo.bIsNPC)
                                std::snprintf(line, sizeof(line), "%s | %.0fm | NPC", e.StaticInfo.Name.c_str(), dist);
                            else
                                std::snprintf(line, sizeof(line), "%s | %.0fm | ID %d", e.StaticInfo.Name.c_str(), dist, e.StaticInfo.NetId);

                            ImGui::PushID(91000 + i);
                            if (ImGui::Selectable(line, WorldSelPlayer == i))
                                WorldSelPlayer = i;
                            ImGui::PopID();
                        }
                        ImGui::EndChild();
                    }
                }
                ImGui::EndCustomChild();

                ImGui::Spacing();

                ImGui::CustomChild(XorStr("Acoes"), ImVec2(ImGui::GetColumnWidth() - 8, optH));
                {
                    if (Cheat::g_Fivem.IsInitialized())
                    {
                        auto entities = Cheat::g_Fivem.GetEntitiyList();
                        if (WorldSelPlayer >= 0 && WorldSelPlayer < (int)entities.size())
                        {
                            const auto& target = entities[WorldSelPlayer];

                            if (ImGui::Button(XorStr("Teleportar para Jogador"), ImVec2(ImGui::GetColumnWidth() - 16, 24)))
                                Cheat::Exploits::TeleportLocalToCoords(target.Cordinates);

                            bool isSpec = Cheat::Exploits::WorldIsSpectatingPed(target.StaticInfo.Ped);
                            if (ImGui::Checkbox(XorStr("Espectar"), &isSpec))
                                Cheat::Exploits::WorldSetSpectateTarget(isSpec ? target.StaticInfo.Ped : nullptr);

                            auto& fl = g_Options.General.FriendsList;
                            auto  friendIt = std::find(fl.begin(), fl.end(), target.StaticInfo.Name);
                            bool  isFriend = (friendIt != fl.end());

                            if (isFriend)
                            {
                                if (ImGui::Button(XorStr("Remover da Lista de Amigos"), ImVec2(ImGui::GetColumnWidth() - 16, 22)))
                                    fl.erase(friendIt);
                            }
                            else
                            {
                                if (ImGui::Button(XorStr("Adicionar a Lista de Amigos"), ImVec2(ImGui::GetColumnWidth() - 16, 22)))
                                    fl.push_back(target.StaticInfo.Name);
                            }
                        }
                        else
                        {
                            ImGui::TextDisabled(XorStr("Selecione um jogador acima"));
                        }
                    }
                }
                ImGui::EndCustomChild();
            }
            // ── TAB 10: CONFIG ───────────────────────────────────────────────
            else if (ActiveMenuTab == 10)
            {
                ImGui::Columns(2, nullptr, false);

                ImGui::CustomChild(XorStr("Configuracoes"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::TextDisabled(XorStr("Conta"));
                    ImGui::Text("Usuario: %s", g_Options.General.UserName.c_str());
                    ImGui::Text("Cargo:   %s", g_Options.General.UserRole.c_str());
                    ImGui::Separator();
                    ImGui::Spacing();

                    if (ImGui::Button(XorStr("Exportar para Clipboard"), ImVec2(ImGui::GetColumnWidth() - 16, 24)))
                        ConfigManager.ExportToClipboard();
                    if (ImGui::Button(XorStr("Importar do Clipboard"), ImVec2(ImGui::GetColumnWidth() - 16, 24)))
                        ConfigManager.ImportFromClipboard();

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::Checkbox(XorStr("Safe Mode"), &g_Options.General.SafeMode);
                    ImGui::Checkbox(XorStr("Features Ativas"), &g_Options.Misc.ShowActiveFeaturesOverlay);
                }
                ImGui::EndCustomChild();

                ImGui::NextColumn();

                ImGui::CustomChild(XorStr("Sistema"), ImVec2(ImGui::GetColumnWidth() - 8, kPanelH - 10));
                {
                    ImGui::KeyBind(XorStr("Tecla Menu"), &g_Options.General.MenuKey, &g_Options.General.KeyBindState);
                    ImGui::SliderInt(XorStr("Delay Processador"), &g_Options.General.ThreadDelay, 1, 15, "%dms");
                    ImGui::Checkbox(XorStr("Stream Mode"), &g_Options.General.CaptureBypass);
                    ImGui::Checkbox(XorStr("Particulas"), &g_Options.General.Particles);
                    ImGui::Spacing();
                    if (ImGui::Button(XorStr("Descarregar Cheat"), ImVec2(ImGui::GetColumnWidth() - 16, 24)))
                        ExitProcess(0);
                }
                ImGui::EndCustomChild();

                ImGui::Columns(1);
            }
        }
        ImGui::EndChild();

        // ── PARTICLES ────────────────────────────────────────────────────────
        if (g_Options.General.Particles)
        {
            static float ptAnim = 0.0f;
            ptAnim += ImGui::GetIO().DeltaTime * 0.5f;

            for (int i = 0; i < 50; i++)
            {
                float x = fmodf((sinf(i * 0.9f + 1.2f) * 0.5f + 0.5f) * WindowSize.x + i * 15.0f, WindowSize.x);
                float y = fmodf(WindowSize.y - (ptAnim * 70.0f + i * 22.0f), WindowSize.y);
                if (y < 0.0f) y += WindowSize.y;
                DrawList->AddCircleFilled(WindowPos + ImVec2(x, y), 1.5f, ImColor(245, 30, 30, 140));
            }
        }

        ImGui::End();
    }

    // ── WndProc ──────────────────────────────────────────────────────────────
    void Interface::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED)
            {
                ResizeWidht = (UINT)LOWORD(lParam);
                ResizeHeight = (UINT)HIWORD(lParam);
            }
            break;
        }
        if (bIsMenuOpen)
            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    }

    // ── Menu key ─────────────────────────────────────────────────────────────
    void Interface::HandleMenuKey()
    {
        static bool KeyDown = false;

        if (GetAsyncKeyState(g_Options.General.MenuKey) & 0x8000)
        {
            if (!KeyDown)
            {
                KeyDown = true;
                HWND fg = GetForegroundWindow();
                if (fg == hTargetWindow || fg == hWindow)
                {
                    bIsMenuOpen = !bIsMenuOpen;

                    if (bIsMenuOpen)
                    {
                        ApplyOverlayWindowStyle(hWindow, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
                        SetForegroundWindow(hWindow);
                    }
                    else
                    {
                        ApplyOverlayWindowStyle(hWindow, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED);
                        SetForegroundWindow(hTargetWindow);
                    }
                }
            }
        }
        else
        {
            KeyDown = false;
        }
    }

    // ── Shutdown ─────────────────────────────────────────────────────────────
    void Interface::ShutDown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}


