// Language: C++17
// File: aimbot.cpp
// Platform: Windows 11 / x64
// Target: FiveM Cheat Engine / LegitBot Module

#include "AimBot.hpp"
#include "../../Options.hpp"

namespace Cheat
{
    void AimBot::RunThread()
    {
        auto lastTime = std::chrono::steady_clock::now();

        while (!g_Options.General.ShutDown)
        {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<float> elapsed = now - lastTime;

            if (elapsed.count() < 0.01f + g_Options.General.ThreadDelay / 1000.0f)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            lastTime = now;

            if (!g_Options.LegitBot.AimBot.Enabled)
                continue;

            if (!g_Fivem.GetLocalPlayerInfo().Ped)
                continue;

            // Resetar alvo se nenhum válido
            AimbotTargetPed = 0;

            Entity ClosestEntity;

            if (g_Fivem.FindClosestEntity(
                g_Options.LegitBot.AimBot.FOV,
                g_Options.LegitBot.AimBot.MaxDistance,
                g_Options.LegitBot.AimBot.TargetNPC,
                g_Options.LegitBot.AimBot.ClosestFov,
                &ClosestEntity))
            {
                uint64_t pedAddress = (uint64_t)ClosestEntity.StaticInfo.Ped;

                // Validação de ponteiro inválido
                if (pedAddress >= 0xCCCCCCCCCCCCCC)
                    continue;

                // Checagem de amigo: Flag do FiveM ou Lista Customizada
                if (ClosestEntity.StaticInfo.bIsFriend ||
                    g_Options.LegitBot.AimBot.IsFriend(pedAddress))
                {
                    continue;
                }

                if (g_Options.LegitBot.AimBot.VisibleCheck && !ClosestEntity.Visible)
                    continue;

                if (!SafeCall(GetAsyncKeyState)(g_Options.LegitBot.AimBot.KeyBind))
                    continue;

                // Alvo validado: define como Ped na mira
                AimbotTargetPed = (DWORD_PTR)pedAddress;

                Vector3D BonePos;

                switch (g_Options.LegitBot.AimBot.HitBox)
                {
                case 0: // Head
                    BonePos = g_Fivem.GetBonePosVec3(ClosestEntity, SKEL_Head);
                    break;
                case 1: // Neck
                    BonePos = g_Fivem.GetBonePosVec3(ClosestEntity, SKEL_Neck_1);
                    break;
                case 2: // Chest
                    BonePos = g_Fivem.GetBonePosVec3(ClosestEntity, SKEL_Spine3);
                    break;
                default:
                    continue;
                }

                g_Fivem.ProcessCameraMovement(
                    BonePos,
                    g_Options.LegitBot.AimBot.SmoothHorizontal,
                    g_Options.LegitBot.AimBot.SmoothVertical);
            }
        }
    }
}