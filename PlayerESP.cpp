//Dev Ghostdobypass
#include "PlayerESP.hpp"

#include <cmath>
#include <unordered_map>
#include <string>
#include <vector>

#include "../../FivemSDK/Fivem.hpp"
#include "../../Options.hpp"
#include <Cheat/Features/LegitBot/AimBot.hpp>

struct Position
{
	ImVec2 Pos;
};

namespace Cheat
{
	static std::string GetPedNameFormatted(const Entity& Current)
	{
		if (Current.StaticInfo.bIsNPC)
			return XorStr("NPC");

		std::string name = Current.StaticInfo.Name;

		// Filtra nomes inválidos, vazios ou corrompidos ("Invalid", "*", etc.)
		if (name.empty() || name == XorStr("Unknown") || name.find(XorStr("Invalid")) != std::string::npos || name.find(XorStr("*")) != std::string::npos)
		{
			if (Current.StaticInfo.NetId > 0)
			{
				return XorStr("Player [") + std::to_string(Current.StaticInfo.NetId) + XorStr("]");
			}
			return XorStr("Player");
		}

		return name;
	}

	static void DrawHealthBarV(ImDrawList* DrawList, ImVec2 pos, ImVec2 dim, ImColor col, int background)
	{
		if (background == 1) {
			DrawList->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + dim.x, pos.y - (dim.y + 1)), col);
		}
		else {
			DrawList->AddRectFilled(ImVec2(pos.x - 1, pos.y + 1), ImVec2(pos.x + dim.x + 1, pos.y - (dim.y + 2)), ImColor(0, 0, 0, 255));
			DrawList->AddRectFilled(ImVec2(pos.x, pos.y - 1), ImVec2(pos.x + dim.x, pos.y - (dim.y + 2)), ImColor(80, 80, 80, 125));
		}
	}

	static void DrawHealthBarH(ImDrawList* DrawList, ImVec2 pos, ImVec2 dim, ImColor col, int background)
	{
		if (background == 1) {
			DrawList->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + dim.y, pos.y + dim.x), col);
		}
		else {
			DrawList->AddRectFilled(ImVec2(pos.x - 1, pos.y - 1), ImVec2(pos.x + dim.y + 1, pos.y + dim.x + 1), ImColor(0, 0, 0, 255));
			DrawList->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + dim.y, pos.y + dim.x), ImColor(80, 80, 80, 125));
		}
	}

	void ESP::Players()
	{
		if (!g_Fivem.GetLocalPlayerInfo().Ped)
			return;

		static std::unordered_map<CPed*, EspAnim> EspAnimations;

		auto& opts = g_Options.Visuals.ESP.Players;
		auto& miscOpts = g_Options.Misc.Screen;
		auto DrawList = ImGui::GetBackgroundDrawList();

		for (Entity Current : g_Fivem.GetEntitiyList())
		{
			if (Current.StaticInfo.bIsLocalPlayer && !opts.ShowLocalPlayer)
				continue;

			if (Current.StaticInfo.bIsNPC && !opts.ShowNPCs)
				continue;

			if (opts.VisibleOnly && !Current.Visible)
				continue;

			Vector3D PedCoordinates = Current.Cordinates;

			auto& CurrentESPAnim = EspAnimations[Current.StaticInfo.Ped];
			CurrentESPAnim.CanFadeOut = false;

			float Distance = PedCoordinates.DistTo(g_Fivem.GetLocalPlayerInfo().WorldPos);
			if (Distance > opts.RenderDistance)
				continue;

			ImVec2 PedLocation = g_Fivem.WorldToScreen(PedCoordinates);

			ImVec2 Head = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_Head));
			if (!g_Fivem.IsOnScreen(Head))
				continue;

			ImVec2 LeftFoot = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_L_Foot));
			if (!g_Fivem.IsOnScreen(LeftFoot))
				continue;

			ImVec2 RightFoot = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_R_Foot));
			if (!g_Fivem.IsOnScreen(RightFoot))
				continue;

			float maxFootY = LeftFoot.y > RightFoot.y ? LeftFoot.y : RightFoot.y;
			float Height = maxFootY - Head.y;

			float Width = Height / 1.8f;
			Height *= 1.37f;

			float BoxLeft = Head.x - (Width / 2.f);
			float BoxRight = Head.x + (Width / 2.f);
			float BoxTop = Head.y - Height * 0.11f;
			float BoxBottom = BoxTop + Height;

			ImVec2 BoxMin(BoxLeft, BoxTop);
			ImVec2 BoxMax(BoxRight, BoxBottom);

			ImVec2 BoxCenter = ImVec2((BoxMin.x + BoxMax.x) * 0.5f, (BoxMin.y + BoxMax.y) * 0.5f);

			float TextTopY = BoxTop - 20.0f;
			float TextBottomY = BoxBottom + 5.0f;

			if (opts.Box)
			{
				DrawList->AddRect({ BoxLeft - 1, BoxTop - 1 }, { BoxRight + 1, BoxBottom + 1 }, ImColor(0, 0, 0, 200), 0, 0, 1.5f);
				DrawList->AddRect({ BoxLeft + 1, BoxTop + 1 }, { BoxRight - 1, BoxBottom - 1 }, ImColor(0, 0, 0, 200), 0, 0, 1.5f);
				DrawList->AddRect({ BoxLeft, BoxTop }, { BoxRight, BoxBottom }, FrameWork::Misc::Float4ToImColor(opts.BoxColor), 0, 0, 1.5f);
			}

			if (opts.Skeleton)
			{
				do
				{
					ImVec2 Pelvis = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_Pelvis));
					if (!g_Fivem.IsOnScreen(Pelvis)) break;

					ImVec2 Neck = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_Neck_1));
					if (!g_Fivem.IsOnScreen(Neck)) break;

					ImVec2 LeftClavicle = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_L_Clavicle));
					if (!g_Fivem.IsOnScreen(LeftClavicle)) break;

					ImVec2 RightClavicle = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_R_Clavicle));
					if (!g_Fivem.IsOnScreen(RightClavicle)) break;

					ImVec2 LeftUperarm = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_L_UpperArm));
					if (!g_Fivem.IsOnScreen(LeftUperarm)) break;

					ImVec2 RightUperarm = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_R_UpperArm));
					if (!g_Fivem.IsOnScreen(RightUperarm)) break;

					ImVec2 RightFormArm = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_R_Forearm));
					if (!g_Fivem.IsOnScreen(RightFormArm)) break;

					ImVec2 LeftFormArm = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_L_Forearm));
					if (!g_Fivem.IsOnScreen(LeftFormArm)) break;

					ImVec2 RightHand = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_R_Hand));
					if (!g_Fivem.IsOnScreen(RightHand)) break;

					ImVec2 LeftHand = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_L_Hand));
					if (!g_Fivem.IsOnScreen(LeftHand)) break;

					ImVec2 LeftThigh = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_L_Thigh));
					if (!g_Fivem.IsOnScreen(LeftThigh)) break;

					ImVec2 LeftCalf = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_L_Calf));
					if (!g_Fivem.IsOnScreen(LeftCalf)) break;

					ImVec2 RightThigh = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_R_Thigh));
					if (!g_Fivem.IsOnScreen(RightThigh)) break;

					ImVec2 RightCalf = g_Fivem.WorldToScreen(g_Fivem.GetBonePosVec3(Current, SKEL_R_Calf));
					if (!g_Fivem.IsOnScreen(RightCalf)) break;

					ImColor Color = (uintptr_t)Current.StaticInfo.Ped == AimbotTargetPed && miscOpts.ShowAimbotRGB ?
						ImColor(
							(sinf(ImGui::GetTime() * 66.0f + 0.0f) * 0.5f + 0.5f),
							(sinf(ImGui::GetTime() * 66.0f + 2.0f) * 0.5f + 0.5f),
							(sinf(ImGui::GetTime() * 66.0f + 4.0f) * 0.5f + 0.5f)
						) :
						(Current.StaticInfo.bIsFriend ? FrameWork::Misc::Float4ToImColor(opts.FriendSkeletonColor)
							: FrameWork::Misc::Float4ToImColor(opts.SkeletonColor));

					DrawList->AddLine(Neck, RightClavicle, Color, 1.f);
					DrawList->AddLine(Neck, LeftClavicle, Color, 1.f);
					DrawList->AddLine(RightClavicle, RightUperarm, Color, 1.f);
					DrawList->AddLine(LeftClavicle, LeftUperarm, Color, 1.f);
					DrawList->AddLine(RightUperarm, RightFormArm, Color, 1.f);
					DrawList->AddLine(LeftUperarm, LeftFormArm, Color, 1.f);
					DrawList->AddLine(RightFormArm, RightHand, Color, 1.f);
					DrawList->AddLine(LeftFormArm, LeftHand, Color, 1.f);
					DrawList->AddLine(Neck, Pelvis, Color, 1.f);
					DrawList->AddLine(Pelvis, LeftThigh, Color, 1.f);
					DrawList->AddLine(Pelvis, RightThigh, Color, 1.f);
					DrawList->AddLine(LeftThigh, LeftCalf, Color, 1.f);
					DrawList->AddLine(RightThigh, RightCalf, Color, 1.f);
					DrawList->AddLine(LeftCalf, LeftFoot, Color, 1.f);
					DrawList->AddLine(RightCalf, RightFoot, Color, 1.f);
				} while (false);
			}

			if (opts.HealthBar)
			{
				float Health = Current.StaticInfo.Ped->GetHealth();
				float MaxHealth = Current.StaticInfo.Ped->GetMaxHealth();

				CurrentESPAnim.Health = ImLerp(CurrentESPAnim.Health, Health, ImGui::GetIO().DeltaTime * 4.0f);
				float AnimHealth = CurrentESPAnim.Health;

				float FullHealthBar = Height;
				float DecreaseHealthBar = FullHealthBar * (AnimHealth / MaxHealth);
				float FullHealthBarH = Width;
				float DecreaseHealthBarH = FullHealthBarH * (AnimHealth / MaxHealth);

				if (DecreaseHealthBarH > FullHealthBarH) DecreaseHealthBarH = FullHealthBarH;
				if (DecreaseHealthBar > FullHealthBar) DecreaseHealthBar = FullHealthBar;

				ImColor BarColor;
				ImColor FullHealth = ImColor(80, 80, 80, 200);

				if (Health > (MaxHealth / 2.0f))
				{
					BarColor = ImColor(66, 245, 132, 255);
				}
				else if (Health <= (MaxHealth / 2.0f) && (MaxHealth == 200 ? (Health > 50.0f) : (Health > 150.0f)))
				{
					BarColor = ImColor(245, 135, 66, 255);
				}
				else
				{
					BarColor = ImColor(245, 66, 66, 255);
				}

				switch (opts.HealthBarState)
				{
				case 0:
					DrawHealthBarH(DrawList, ImVec2(BoxMin.x, BoxMin.y - 6), ImVec2(3, FullHealthBarH), FullHealth, 0);
					DrawHealthBarH(DrawList, ImVec2(BoxMin.x, BoxMin.y - 6), ImVec2(3, DecreaseHealthBarH), BarColor, 1);
					break;
				case 1:
					DrawHealthBarV(DrawList, ImVec2(BoxMax.x + 6, BoxMax.y), ImVec2(3, FullHealthBar), FullHealth, 0);
					DrawHealthBarV(DrawList, ImVec2(BoxMax.x + 6, BoxMax.y), ImVec2(3, DecreaseHealthBar), BarColor, 1);
					break;
				case 2:
					DrawHealthBarH(DrawList, ImVec2(BoxMin.x, BoxMax.y + 6), ImVec2(3, FullHealthBarH), FullHealth, 0);
					DrawHealthBarH(DrawList, ImVec2(BoxMin.x, BoxMax.y + 6), ImVec2(3, DecreaseHealthBarH), BarColor, 1);
					break;
				case 3:
				default:
					DrawHealthBarV(DrawList, ImVec2(BoxMin.x - 6, BoxMax.y), ImVec2(3, FullHealthBar), FullHealth, 0);
					DrawHealthBarV(DrawList, ImVec2(BoxMin.x - 6, BoxMax.y), ImVec2(3, DecreaseHealthBar), BarColor, 1);
					break;
				}
			}

			struct TextInfo {
				std::string text;
				bool enabled;
				int positionState;
			};

			std::vector<TextInfo> textsToDraw;

			if (opts.Name)
			{
				std::string Name = GetPedNameFormatted(Current);
				textsToDraw.push_back({ Name, true, opts.NameState });
			}

			if (opts.Distance && !Current.StaticInfo.bIsLocalPlayer)
			{
				std::string PlayerDistance = std::to_string((int)Distance) + XorStr("m");
				textsToDraw.push_back({ PlayerDistance, true, opts.DistanceState });
			}

			if (opts.WeaponName)
			{
				CWeaponManager* WeaponManager = Current.StaticInfo.Ped->GetWeaponManager();
				if (WeaponManager)
				{
					CWeaponInfo* WeaponInfo = WeaponManager->GetWeaponInfo();
					if (WeaponInfo)
					{
						std::string WeaponName = WeaponInfo->GetWeaponName();
						if (!WeaponName.empty())
						{
							textsToDraw.push_back({ WeaponName, true, opts.WeaponNameState });
						}
					}
				}
			}

			float currentTextYTop = TextTopY;
			for (const auto& textInfo : textsToDraw)
			{
				if (textInfo.positionState == 0)
				{
					ImVec2 TextSize = ImGui::CalcTextSize(textInfo.text.c_str());
					ImVec2 TextPos = ImVec2(BoxCenter.x - (TextSize.x / 2), currentTextYTop);

					ImGui::PushFont(FrameWork::Assets::InterBold12);
					DrawList->AddText(ImVec2(TextPos.x + 1, TextPos.y + 1), ImColor(0.f, 0.f, 0.f, 1.f), textInfo.text.c_str());
					DrawList->AddText(TextPos, FrameWork::Misc::Float4ToImColor(opts.TextColor), textInfo.text.c_str());
					ImGui::PopFont();

					currentTextYTop -= 12.0f;
				}
			}

			float currentTextYBottom = TextBottomY;
			for (const auto& textInfo : textsToDraw)
			{
				if (textInfo.positionState == 2)
				{
					ImVec2 TextSize = ImGui::CalcTextSize(textInfo.text.c_str());
					ImVec2 TextPos = ImVec2(BoxCenter.x - (TextSize.x / 2), currentTextYBottom);

					ImGui::PushFont(FrameWork::Assets::InterBold12);
					DrawList->AddText(ImVec2(TextPos.x + 1, TextPos.y + 1), ImColor(0.f, 0.f, 0.f, 1.f), textInfo.text.c_str());
					DrawList->AddText(TextPos, FrameWork::Misc::Float4ToImColor(opts.TextColor), textInfo.text.c_str());
					ImGui::PopFont();

					currentTextYBottom += 12.0f;
				}
			}

			if (opts.SnapLines)
			{
				if (!Current.StaticInfo.bIsLocalPlayer)
				{
					DrawList->AddLine(g_Fivem.GetLocalPlayerInfo().ScreenPos, PedLocation, FrameWork::Misc::Float4ToImColor(opts.SnapLinesColor));
				}
			}
		}
	}
}