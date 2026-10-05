#pragma once

#include <Cheat/Cheat.hpp>
#include <FrameWork/FrameWork.hpp>
#include <FrameWork/Render/Preview Images.hpp>
#include <d3d11.h>
#include <d3dx11.h>

#include <vector>

namespace Cheat
{
	class EspPreview {
	public:
		ID3D11ShaderResourceView* PreviewTexture = nullptr;

		void CreateTexture(ID3D11Device* Device)
		{
			if (PreviewTexture) return;
			D3DX11CreateShaderResourceViewFromMemory(Device, PreviewImages, sizeof(PreviewImages), NULL, NULL, &PreviewTexture, NULL);
		}

		void DragDropHandler()
		{
            Position Positions[] =
            {
                {ImVec2(iBox_X, iBox_Y - 10.0f)}, // Top - aumentei de -3.5f para -10.0f
                {ImVec2(iBox_X + iBox_Width + 3.5f, iBox_Y)}, // Right
                {ImVec2(iBox_X, iBox_Y + iBox_Height + 3.5f)}, // Bottom
                {ImVec2(iBox_X - 3.5f, iBox_Y)}, // Left
            };

			for (size_t i = 0; i < mWidgets.size(); i++)
			{
				auto& Widget = mWidgets.at(i);

				if (!Widget.Initialized)
					continue;

				if (Widget.AlphaAnimation == 0)
					continue;

				ImGui::PushID(i);
				{
					ImGui::SetCursorScreenPos(Widget.Position);
					ImGui::InvisibleButton((XorStr("Drag&Drop##") + Widget.Text).c_str(), Widget.Size + ImVec2(10, 10));
					bool Hovered = ImGui::IsItemHovered();
					int Pos = FindClosestPosition(ImGui::GetMousePos(), Positions, i); // Passei o �ndice do widget
					Widget.Helding = false;
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip))
					{
						ImGui::SetDragDropPayload(XorStr("Drag&Drop"), &i, sizeof(int), 0);
						Widget.Pos = 4;
						Widget.PossiblePos = Pos;
						Widget.Helding = true;
						ImGui::EndDragDropSource();
					}
					else if (Widget.Pos == 4)
					{
						Widget.Pos = Widget.PossiblePos;
						Widget.PossiblePos = -1;
					}

					Widget.HeldingAnimation = ImClamp(Widget.HeldingAnimation + (6 * ImGui::GetIO().DeltaTime * (Widget.Helding ? 1.f : -1.f)), 0.f, 1.f);
				}
				ImGui::PopID();
			}
		}

        void Draw()
        {
            iBox_Height = 220; // Aumentado de 140 para 220
            iBox_Width = iBox_Height / 2.f;

            iBox_X = ImGui::GetWindowPos().x + ImGui::GetWindowSize().x / 2 - iBox_Width / 2;
            iBox_Y = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y / 2 - iBox_Height / 2;

            Position Positions[] =
            {
                {ImVec2(iBox_X, iBox_Y - 3.5f)}, // Top
                {ImVec2(iBox_X + iBox_Width + 3.5f, iBox_Y)}, // Right
                {ImVec2(iBox_X, iBox_Y + iBox_Height + 3.5f)}, // Bottom
                {ImVec2(iBox_X - 3.5f, iBox_Y)}, // Left
            };

            static float BoxAnimation = 0.f;
            BoxAnimation = ImClamp(BoxAnimation + (6 * ImGui::GetIO().DeltaTime * (g_Options.Visuals.ESP.Players.Box ? 1.f : -1.f)), 0.f, 1.f);

            if (PreviewTexture)
            {
                ImGui::GetWindowDrawList()->AddImage(PreviewTexture, ImVec2(iBox_X, iBox_Y), ImVec2(iBox_X + iBox_Width, iBox_Y + iBox_Height), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, (int)(ImGui::GetStyle().Alpha * 255)));
            }

            if (BoxAnimation != 0)
            {
                ImGui::GetWindowDrawList()->AddRect(ImVec2(iBox_X, iBox_Y), ImVec2(iBox_X + iBox_Width, iBox_Y + iBox_Height), ImColor(0, 0, 0, (int)((int)(ImGui::GetStyle().Alpha * 255) * BoxAnimation)), 0, ImDrawFlags_None, 2);
                ImGui::GetWindowDrawList()->AddRect(ImVec2(iBox_X, iBox_Y), ImVec2(iBox_X + iBox_Width, iBox_Y + iBox_Height), ImColor((int)g_Options.Visuals.ESP.Players.BoxColor * 255, (int)g_Options.Visuals.ESP.Players.BoxColor * 255, (int)g_Options.Visuals.ESP.Players.BoxColor * 255, (int)((int)(ImGui::GetStyle().Alpha * 255) * BoxAnimation)), 0, ImDrawFlags_None);
            }

            // RESETAR OS PADDINGS ANTES DE PROCESSAR
            for (size_t i = 0; i < 4; i++)
            {
                PaddingY[i] = 0;
                PaddingX[i] = 0;
            }

            // PROCESSAR NA ORDEM CORRETA: Name primeiro, depois Weapon, depois Distance
            int processingOrder[] = { 2, 3, 4, 0, 1 }; // Name, Weapon, Distance, HealthBar, ArmorBar

            for (size_t j = 0; j < 5; j++)
            {
                int i = processingOrder[j];
                auto& Widget = mWidgets.at(i);

                if (Widget.IsIcon)
                    ImGui::PushFont(FrameWork::Assets::FontAwesomeSolid);
                else
                    ImGui::PushFont(Widget.SmallFont ? FrameWork::Assets::InterRegular : FrameWork::Assets::InterRegular);

                if (!Widget.Initialized)
                {
                    Widget.Position = Positions[Widget.InitialPos].Pos;
                    Widget.Pos = Widget.InitialPos;

                    if (Widget.Type == 0)
                    {
                        Widget.Size = ImGui::CalcTextSize(Widget.Text.c_str());
                    }
                    else if (Widget.Type == 1)
                    {
                        if (Widget.InitialPos == 0 || Widget.InitialPos == 2)
                        {
                            Widget.Size = ImVec2(iBox_Width, 2);
                        }
                        else
                        {
                            Widget.Size = ImVec2(2, iBox_Height);
                        }
                    }

                    Widget.Initialized = true;
                }

                switch (i)
                {
                case 0:
                    Widget.Enabled = g_Options.Visuals.ESP.Players.HealthBar;
                    g_Options.Visuals.ESP.Players.HealthBarRawPos[0] = Widget.RawPos.x;
                    g_Options.Visuals.ESP.Players.HealthBarRawPos[1] = Widget.RawPos.y;
                    break;
                case 1:
                    Widget.Enabled = g_Options.Visuals.ESP.Players.ArmorBar;
                    g_Options.Visuals.ESP.Players.ArmorBarRawPos[0] = Widget.RawPos.x;
                    g_Options.Visuals.ESP.Players.ArmorBarRawPos[1] = Widget.RawPos.y;
                    break;
                case 2:
                    Widget.Enabled = g_Options.Visuals.ESP.Players.Name;
                    g_Options.Visuals.ESP.Players.NameRawPos[0] = Widget.RawPos.x;
                    g_Options.Visuals.ESP.Players.NameRawPos[1] = Widget.RawPos.y;
                    break;
                case 3:
                    Widget.Enabled = g_Options.Visuals.ESP.Players.WeaponName;
                    g_Options.Visuals.ESP.Players.WeaponNameRawPos[0] = Widget.RawPos.x;
                    g_Options.Visuals.ESP.Players.WeaponNameRawPos[1] = Widget.RawPos.y;
                    break;
                case 4:
                    Widget.Enabled = g_Options.Visuals.ESP.Players.Distance;
                    g_Options.Visuals.ESP.Players.DistanceRawPos[0] = Widget.RawPos.x;
                    g_Options.Visuals.ESP.Players.DistanceRawPos[1] = Widget.RawPos.y;
                    break;
                }

                static int seloco = 0;

                switch (i)
                {
                case 0:
                    if (Widget.Pos != 4)
                    {
                        if (g_Options.Visuals.ESP.Players.UpdateESP)
                        {
                            Widget.Pos = g_Options.Visuals.ESP.Players.HealthBarState;
                            seloco++;
                        }
                        else
                            g_Options.Visuals.ESP.Players.HealthBarState = Widget.Pos;
                    }
                    break;
                case 1:
                    if (Widget.Pos != 4)
                    {
                        if (g_Options.Visuals.ESP.Players.UpdateESP)
                        {
                            Widget.Pos = g_Options.Visuals.ESP.Players.ArmorBarState;
                            seloco++;
                        }
                        else
                            g_Options.Visuals.ESP.Players.ArmorBarState = Widget.Pos;
                    }
                    break;
                case 2:
                    if (Widget.Pos != 4)
                    {
                        if (g_Options.Visuals.ESP.Players.UpdateESP)
                        {
                            Widget.Pos = g_Options.Visuals.ESP.Players.NameState;
                            seloco++;
                        }
                        else
                            g_Options.Visuals.ESP.Players.NameState = Widget.Pos;
                    }
                    break;
                case 3:
                    if (Widget.Pos != 4)
                    {
                        if (g_Options.Visuals.ESP.Players.UpdateESP)
                        {
                            Widget.Pos = g_Options.Visuals.ESP.Players.WeaponNameState;
                            seloco++;
                        }
                        else
                            g_Options.Visuals.ESP.Players.WeaponNameState = Widget.Pos;
                    }
                    break;
                case 4:
                    if (Widget.Pos != 4)
                    {
                        if (g_Options.Visuals.ESP.Players.UpdateESP)
                        {
                            Widget.Pos = g_Options.Visuals.ESP.Players.DistanceState;
                            seloco++;
                        }
                        else {
                            g_Options.Visuals.ESP.Players.DistanceState = Widget.Pos;
                        }
                    }
                    break;
                }

                Widget.AlphaAnimation = ImClamp(Widget.AlphaAnimation + (6 * ImGui::GetIO().DeltaTime * (Widget.Enabled ? 1.f : -1.f)), 0.f, 1.f);
                if (Widget.AlphaAnimation == 0)
                {
                    ImGui::PopFont();
                    continue;
                }

                if (Widget.Helding)
                {
                    switch (Widget.PossiblePos)
                    {
                    case 0:
                        PaddingY[0] += -Widget.Size.y - 2;
                        break;
                    case 1:
                        Widget.Type == 0 ? PaddingY[1] += Widget.Size.y + 2 : PaddingX[1] += Widget.Size.x + 2;
                        break;
                    case 2:
                        PaddingY[2] += Widget.Size.y + 2;
                        break;
                    case 3:
                        Widget.Type == 0 ? PaddingY[3] += Widget.Size.y + 2 : PaddingX[3] += -Widget.Size.x - 2;
                        break;
                    }
                }

                if (Widget.Type == 0) //Text
                {
                    if (Widget.Pos == 0) // Top
                    {
                        float extraOffset = 0.0f;

                        // Se for o Name (�ndice 2), subir um pouco mais
                        if (i == 2) { // Name widget
                            extraOffset = -8.0f; // Subir 5 pixels extra
                        }

                        Widget.Position = Positions[Widget.Pos].Pos + ImVec2(iBox_Width / 2 - Widget.Size.x / 2, -Widget.Size.y + PaddingY[0] - 15 + extraOffset);
                        Widget.RawPos = ImVec2(0, -Widget.Size.y + PaddingY[0] - 15 + extraOffset);
                        PaddingY[0] += -Widget.Size.y - 2;
                    }

                    else if (Widget.Pos == 1) // Right
                    {
                        Widget.Position = Positions[Widget.Pos].Pos + ImVec2(PaddingX[1], PaddingY[1]);
                        Widget.RawPos = ImVec2(PaddingX[1], PaddingY[1]);
                        PaddingY[1] += Widget.Size.y + 1;
                    }
                    else if (Widget.Pos == 2) // Bottom
                    {
                        Widget.Position = Positions[Widget.Pos].Pos + ImVec2(iBox_Width / 2 - Widget.Size.x / 2, PaddingY[2]);
                        Widget.RawPos = ImVec2(0, PaddingY[2]);
                        PaddingY[2] += Widget.Size.y + 1;
                    }
                    else if (Widget.Pos == 3) // Left
                    {
                        Widget.Position = Positions[Widget.Pos].Pos + ImVec2(-Widget.Size.x + PaddingX[3], PaddingY[3]);
                        Widget.RawPos = ImVec2(PaddingX[3], PaddingY[3]);
                        PaddingY[3] += Widget.Size.y + 1;
                    }
                    else if (Widget.Pos == 4) // Moving
                    {
                        Widget.Position = ImLerp(Widget.Position, ImGui::GetMousePos() + ImVec2(-Widget.Size.x / 2.f, 0), ImGui::GetIO().DeltaTime * 8);
                    }

                    ImGui::GetWindowDrawList()->AddText(Widget.Position, ImColor(0.f, 0.f, 0.f, ImGui::GetStyle().Alpha * Widget.AlphaAnimation), Widget.Text.c_str());
                    ImGui::GetWindowDrawList()->AddText(Widget.Position + ImVec2(1, 1), ImGui::GetColorU32(ImVec4(Widget.Color.Value.x, Widget.Color.Value.y, Widget.Color.Value.z, ImGui::GetStyle().Alpha * Widget.AlphaAnimation)), Widget.Text.c_str());
                }
                if (Widget.Type == 1) //Bar
                {
                    if (Widget.Pos == 0 || Widget.Pos == 2) // Top or Bottom
                        Widget.Size = ImVec2(iBox_Width, 3);
                    else if (Widget.Pos == 1 || Widget.Pos == 3) // Right or Left
                        Widget.Size = ImVec2(3, iBox_Height);

                    if (Widget.Pos == 0) // Top
                    {
                        Widget.Position = Positions[Widget.Pos].Pos + ImVec2(0, -Widget.Size.y + PaddingY[0]);
                        Widget.RawPos = ImVec2(0, -Widget.Size.y + PaddingY[0]);
                        PaddingY[0] += -Widget.Size.y - 4;
                    }
                    else if (Widget.Pos == 1) // Right
                    {
                        Widget.Position = Positions[Widget.Pos].Pos + ImVec2(PaddingX[1], 0);
                        Widget.RawPos = ImVec2(PaddingX[1], 0);
                        PaddingX[1] += Widget.Size.x + 4;
                    }
                    else if (Widget.Pos == 2) // Bottom
                    {
                        Widget.Position = Positions[Widget.Pos].Pos + ImVec2(0, PaddingY[2]);
                        Widget.RawPos = ImVec2(0, PaddingY[2]);
                        PaddingY[2] += Widget.Size.y + 4;
                    }
                    else if (Widget.Pos == 3) // Left
                    {
                        Widget.Position = Positions[Widget.Pos].Pos + ImVec2(-Widget.Size.x + PaddingX[3], 0);
                        Widget.RawPos = ImVec2(-Widget.Size.x + PaddingX[3], 0);
                        PaddingX[3] += -Widget.Size.x - 4;
                    }
                    else if (Widget.Pos == 4) // Moving
                    {
                        if (Widget.PossiblePos == 0 || Widget.PossiblePos == 2)
                            Widget.Size = ImVec2(iBox_Width, 3);
                        else if (Widget.PossiblePos == 1 || Widget.PossiblePos == 3)
                            Widget.Size = ImVec2(3, iBox_Height);

                        Widget.Position = ImLerp(Widget.Position, ImGui::GetMousePos() + ImVec2(0, 0), ImGui::GetIO().DeltaTime * 8);
                    }

                    ImGui::GetWindowDrawList()->AddRectFilled(Widget.Position - ImVec2(1, 1), Widget.Position + Widget.Size + ImVec2(1, 1), ImGui::GetColorU32(ImVec4(ImColor(0, 0, 0)), ImGui::GetStyle().Alpha * Widget.AlphaAnimation));
                    ImGui::GetWindowDrawList()->AddRectFilled(Widget.Position, Widget.Position + Widget.Size, ImGui::GetColorU32(ImVec4(ImColor(80, 80, 80)), ImGui::GetStyle().Alpha * (125.f / 255.f)));
                    ImGui::GetWindowDrawList()->AddRectFilled(Widget.Position, Widget.Position + Widget.Size, ImGui::GetColorU32(ImVec4(Widget.Color.Value.x, Widget.Color.Value.y, Widget.Color.Value.z, ImGui::GetStyle().Alpha * Widget.AlphaAnimation)));
                }

                ImGui::PopFont();

                if (seloco == 5)
                {
                    g_Options.Visuals.ESP.Players.UpdateESP = false;
                    seloco = 0;
                }
            }
        }

	private:
		class cDragWidget {
		public:
			int Type; // 0 = Text | 1 = Bar
			int InitialPos; // 0 = Top | 1 = Right | 2 = Bottom | 3 = Left
			std::string Text;
			ImColor Color;
			bool IsIcon;
			bool SmallFont = false;
			bool Initialized = false;
			bool Enabled;
			ImVec2 Position;
			int Pos;
			ImVec2 Size;
			int PossiblePos;
			bool Helding;
			float HeldingAnimation;
			float AlphaAnimation;
			ImVec2 RawPos;
		};

		float PaddingY[4] = { 0, 0, 0, 0 };
		float PaddingX[4] = { 0, 0, 0, 0 };

		struct Position {
			ImVec2 Pos;
		};

		std::vector<cDragWidget> mWidgets =
		{
			cDragWidget{ 1, 3, XorStr("HealthBar"), ImColor(0, 255, 12) },
			cDragWidget{ 1, 3, XorStr("ArmorBar"), ImColor(25, 120, 245) },
			cDragWidget{ 0, 2, XorStr("Username"), ImColor(255, 255, 255) },
			cDragWidget{ 0, 2, XorStr("Unarmed"), ImColor(255, 255, 255) }, // Weapon - s� Top/Bottom
			cDragWidget{ 0, 2, XorStr("0m"), ImColor(255, 255, 255) } // Distance - s� Top/Bottom
		};

	private:
        int FindClosestPosition(ImVec2 CurrentPos, Position Positions[], int WidgetIndex)
        {
            float Closest = FLT_MAX;
            int Best = -1;
            for (int i = 0; i < 4; i++)
            {
                if (WidgetIndex == 2 && (i == 1 || i == 3))
                    continue;


                if ((WidgetIndex == 3 || WidgetIndex == 4) && (i == 1 || i == 3))
                    continue;


                ImVec2 Pos = Positions[i].Pos;
                float dist = std::sqrt((CurrentPos.x - Pos.x) * (CurrentPos.x - Pos.x) + (CurrentPos.y - Pos.y) * (CurrentPos.y - Pos.y));

                if (Closest > dist)
                {
                    Closest = dist;
                    Best = i;
                }
            }

            return Best;
        }

	private:
		int iBox_X;
		int iBox_Y;
		int iBox_Width;
		int iBox_Height;

	};
	inline EspPreview g_EspPreview;

}