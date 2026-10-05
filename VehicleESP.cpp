// by HyperX
#pragma once
#include "VehicleESP.hpp"

#include "../../FivemSDK/Fivem.hpp"
#include "../../Options.hpp"

namespace Cheat
{
	void ESP::Vehicles()
	{
		if (!g_Fivem.GetLocalPlayerInfo().Ped)
			return;

		int processed = 0;
		const int maxVehicles = 100;
		for (VehicleInfo Current : g_Fivem.GetVehicleList())
		{
			if (processed++ > maxVehicles)
				break;
			if (g_Options.Visuals.ESP.Vehicles.IgnoreOccupiedVehicles && Current.Vehicle->GetDriver())
				continue;

			ImVec2 Position = g_Fivem.WorldToScreen(Current.Vehicle->GetCoordinate());
			if (!g_Fivem.IsOnScreen(Position))
				continue;

			float OffsetY = 0;
			float dist = Current.Vehicle->GetCoordinate().DistTo(g_Fivem.GetLocalPlayerInfo().WorldPos);
			bool isClose = dist < 80.0f;


			/*
			if (g_Options.Visuals.ESP.Vehicles.Marker)
			{
				// Dibujar las ruedas
				ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(Position.x - 6, Position.y + OffsetY + 5), 2, ImColor(0, 0, 0, 255)); // Rueda izquierda
				ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(Position.x + 6, Position.y + OffsetY + 5), 2, ImColor(0, 0, 0, 255)); // Rueda derecha

				// Dibujar el chasis del coche (un rect�ngulo)
				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(Position.x - 8, Position.y + OffsetY), ImVec2(Position.x + 8, Position.y + OffsetY + 6), ImColor(255, 255, 255, 255), 2.0f);

				// Dibujar el parabrisas
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Position.x - 5, Position.y + OffsetY), ImVec2(Position.x - 3, Position.y + OffsetY - 4), ImColor(255, 255, 255, 255), 2.0f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Position.x + 5, Position.y + OffsetY), ImVec2(Position.x + 3, Position.y + OffsetY - 4), ImColor(255, 255, 255, 255), 2.0f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Position.x - 3, Position.y + OffsetY - 4), ImVec2(Position.x + 3, Position.y + OffsetY - 4), ImColor(255, 255, 255, 255), 2.0f);

				OffsetY += 14;
			}*/


			if (g_Options.Visuals.ESP.Vehicles.Marker)
			{
				ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(Position.x, Position.y + OffsetY), isClose ? 4 : 2, ImColor(0.f, 0.f, 0.f, g_Options.Visuals.ESP.Vehicles.MarkerColor[3] * (isClose ? 1.0f : 0.5f)));
				ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(Position.x, Position.y + OffsetY), isClose ? 3 : 1.5f, FrameWork::Misc::Float4ToImColor(g_Options.Visuals.ESP.Vehicles.MarkerColor));
				OffsetY += isClose ? 12 : 6;
			}

			if (!isClose)
				continue;

			ImGui::PushFont(FrameWork::Assets::InterBold12);

			if (g_Options.Visuals.ESP.Vehicles.Model)
			{
				// Concatenar el texto "MODEL: " con el nombre del modelo
				std::string ModelText = " " + Current.Name;

				// Calcular el tama�o del texto resultante
				ImVec2 TextSize = ImGui::CalcTextSize(ModelText.c_str());

				// Dibujar el texto con sombra (negro)
				ImGui::GetBackgroundDrawList()->AddText(
					ImVec2(Position.x + 1 - TextSize.x / 2, Position.y + OffsetY + 1),
					ImColor(0.f, 0.f, 0.f, g_Options.Visuals.ESP.Vehicles.TextColor[3]),
					ModelText.c_str()
				);

				// Dibujar el texto principal con el color definido
				ImGui::GetBackgroundDrawList()->AddText(
					ImVec2(Position.x - TextSize.x / 2, Position.y + OffsetY),
					FrameWork::Misc::Float4ToImColor(g_Options.Visuals.ESP.Vehicles.TextColor),
					ModelText.c_str()
				);

				// Incrementar el desplazamiento vertical
				OffsetY += 12;
			}

			if (g_Options.Visuals.ESP.Vehicles.Door)
			{
				// Verificar el estado de la puerta (bloqueada o desbloqueada)
				bool isUnlocked = Current.Vehicle->GetLockState() == CARLOCK_UNLOCKED;
				bool isLocked = Current.Vehicle->GetLockState() == CARLOCK_LOCKED;
				bool isLocked1 = Current.Vehicle->GetLockState() == CARLOCK_LOCKOUT_PLAYER_ONLY;
				bool isLocked2 = Current.Vehicle->GetLockState() == CARLOCK_NONE;
				bool isLocked3 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_PLAYER_INSIDE;
				bool isLocked4 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_INITIALLY;
				bool isLocked5 = Current.Vehicle->GetLockState() == CARLOCK_FORCE_SHUT_DOORS;
				bool isDamaged = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_BUT_CAN_BE_DAMAGED;
				bool isLocked6 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_BUT_BOOT_UNLOCKED;
				bool isLocked7 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_NO_PASSENGERS;
				bool isLocked8 = Current.Vehicle->GetLockState() == CARLOCK_CANNOT_ENTER;
				bool isLocked9 = Current.Vehicle->GetLockState() == CARLOCK_PARTIALLY;
				bool isLocked10 = Current.Vehicle->GetLockState() == CARLOCK_NUM_STATES;
				bool isLocked11 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_EXCEPT_TEAM1;
				bool isLocked12 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_EXCEPT_TEAM2;
				bool isLocked13 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_EXCEPT_TEAM3;
				bool isLocked14 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_EXCEPT_TEAM4;
				bool isLocked15 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_EXCEPT_TEAM5;
				bool isLocked16 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_EXCEPT_TEAM6;
				bool isLocked17 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_EXCEPT_TEAM7;
				bool isLocked18 = Current.Vehicle->GetLockState() == CARLOCK_LOCKED_EXCEPT_TEAM8;

				std::string VehicleStatusText;

				if (isUnlocked)
					VehicleStatusText = "Unlocked";
				else if (isLocked)
					VehicleStatusText = "Locked";
				else if (isDamaged)
					VehicleStatusText = "Locked But Possibly Damaged";

				else if (isLocked1)
					VehicleStatusText = "Lockout Player Only";
				else if (isLocked2)
					VehicleStatusText = "Unlocked";
				else if (isLocked3)
					VehicleStatusText = "Player Inside";
				else if (isLocked4)
					VehicleStatusText = "Locked";
				else if (isLocked5)
					VehicleStatusText = "Locked";
				else if (isLocked6)
					VehicleStatusText = "Unknown";
				else if (isLocked7)
					VehicleStatusText = "Player Inside - No Passengers";
				else if (isLocked8)
					VehicleStatusText = "Locked";
				else if (isLocked9)
					VehicleStatusText = "Locked Partially";
				else if (isLocked10)
					VehicleStatusText = "Unknown";
				else if (isLocked11)
					VehicleStatusText = "Locked1";
				else if (isLocked12)
					VehicleStatusText = "Locked2";
				else if (isLocked13)
					VehicleStatusText = "Locked2";
				else if (isLocked14)
					VehicleStatusText = "Locked3";
				else if (isLocked15)
					VehicleStatusText = "Locked4";
				else if (isLocked16)
					VehicleStatusText = "Locked5";
				else if (isLocked17)
					VehicleStatusText = "Locked6";
				else if (isLocked18)
					VehicleStatusText = "Locked7";
				else {
					VehicleStatusText = "Unknown";
				}

				// Calcular el tama�o del texto
				ImVec2 TextSize = ImGui::CalcTextSize(VehicleStatusText.c_str());

				// Dibujar el texto con sombra (negro)
				ImGui::GetBackgroundDrawList()->AddText(
					ImVec2(Position.x + 1 - TextSize.x / 2, Position.y + OffsetY + 1),
					ImColor(0.f, 0.f, 0.f, g_Options.Visuals.ESP.Vehicles.TextColor[3]),
					VehicleStatusText.c_str()
				);

				// Dibujar el texto principal con el color definido
				ImGui::GetBackgroundDrawList()->AddText(
					ImVec2(Position.x - TextSize.x / 2, Position.y + OffsetY),
					FrameWork::Misc::Float4ToImColor(g_Options.Visuals.ESP.Vehicles.TextColor),
					VehicleStatusText.c_str()
				);

				// Incrementar el desplazamiento vertical
				OffsetY += 12;
			}

			if (g_Options.Visuals.ESP.Vehicles.Distance)
			{
				// Crear un buffer para almacenar "DISTANCE: [valor]"
				char bfr[48];
				sprintf(
					bfr,
					XorStr(" %dm"),
					(int)Current.Vehicle->GetCoordinate().DistTo(g_Fivem.GetLocalPlayerInfo().WorldPos)
				);

				// Calcular el tama�o del texto
				ImVec2 TextSize = ImGui::CalcTextSize(bfr);

				// Dibujar el texto con sombra (negro)
				ImGui::GetBackgroundDrawList()->AddText(
					ImVec2(Position.x + 1 - TextSize.x / 2, Position.y + OffsetY + 1),
					ImColor(0.f, 0.f, 0.f, g_Options.Visuals.ESP.Vehicles.TextColor[3]),
					bfr
				);

				// Dibujar el texto principal con el color definido
				ImGui::GetBackgroundDrawList()->AddText(
					ImVec2(Position.x - TextSize.x / 2, Position.y + OffsetY),
					FrameWork::Misc::Float4ToImColor(g_Options.Visuals.ESP.Vehicles.TextColor),
					bfr
				);

				// Incrementar el desplazamiento vertical
				OffsetY += 12;
			}

			ImGui::PopFont();
		}
	}
}