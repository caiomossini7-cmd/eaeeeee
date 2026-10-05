#include "ConfigSystem.hpp"
#include <FrameWork/Utilities/Base64.hpp>
#include "Options.hpp"

namespace Cheat
{

	void ConfigManager::AddItem(void* Pointer, const char* Name, const std::string& Type)
	{
		Items.push_back(new CConfigItem(std::string(Name), Pointer, Type));
	}

	void ConfigManager::SetupItem(int* Pointer, float Value, const std::string& Name)
	{
		AddItem(Pointer, Name.c_str(), XorStr("int"));
		*Pointer = Value;
	}

	void ConfigManager::SetupItem(float* Pointer, float Value, const std::string& Name)
	{
		AddItem(Pointer, Name.c_str(), XorStr("float"));
		*Pointer = Value;
	}

	void ConfigManager::SetupItem(bool* Pointer, float Value, const std::string& Name)
	{
		AddItem(Pointer, Name.c_str(), XorStr("bool"));
		*Pointer = Value;
	}

    void ConfigManager::Setup()
    {
        // LegitBot - AimBot
        SetupItem(&g_Options.LegitBot.AimBot.Enabled, false, "LegitBot.AimBot.Enabled");
        SetupItem(&g_Options.LegitBot.AimBot.LegitMode, false, "LegitBot.AimBot.LegitMode");
        SetupItem(&g_Options.LegitBot.AimBot.ClosestFov, false, "LegitBot.AimBot.ClosestFov");
        SetupItem(&g_Options.LegitBot.AimBot.KeyBind, 0, "LegitBot.AimBot.KeyBind");
        SetupItem(&g_Options.LegitBot.AimBot.KeyBindState, 0, "LegitBot.AimBot.KeyBindState");
        SetupItem(&g_Options.LegitBot.AimBot.TargetNPC, false, "LegitBot.AimBot.TargetNPC");
        SetupItem(&g_Options.LegitBot.AimBot.VisibleCheck, false, "LegitBot.AimBot.VisibleCheck");
        SetupItem(&g_Options.LegitBot.AimBot.HitBox, 0, "LegitBot.AimBot.HitBox");
        SetupItem(&g_Options.LegitBot.AimBot.MaxDistance, 250, "LegitBot.AimBot.MaxDistance");
        SetupItem(&g_Options.LegitBot.AimBot.FOV, 10, "LegitBot.AimBot.FOV");
        SetupItem(&g_Options.LegitBot.AimBot.SmoothHorizontal, 2, "LegitBot.AimBot.SmoothHorizontal");
        SetupItem(&g_Options.LegitBot.AimBot.SmoothVertical, 2, "LegitBot.AimBot.SmoothVertical");

        // LegitBot - MagicBullet
        SetupItem(&g_Options.LegitBot.MagicBullet.Enabled, false, "LegitBot.MagicBullet.Enabled");
        SetupItem(&g_Options.LegitBot.MagicBullet.KeyBind, VK_LBUTTON, "LegitBot.MagicBullet.KeyBind");
        SetupItem(&g_Options.LegitBot.MagicBullet.KeyBindState, 0, "LegitBot.MagicBullet.KeyBindState");

        // LegitBot - TriggerBot
        SetupItem(&g_Options.LegitBot.Trigger.Enabled, false, "LegitBot.Trigger.Enabled");
        SetupItem(&g_Options.LegitBot.Trigger.KeyBind, 0, "LegitBot.Trigger.KeyBind");
        SetupItem(&g_Options.LegitBot.Trigger.KeyBindState, 0, "LegitBot.Trigger.KeyBindState");
        SetupItem(&g_Options.LegitBot.Trigger.ShotNPC, false, "LegitBot.Trigger.ShotNPC");
        SetupItem(&g_Options.LegitBot.Trigger.VisibleCheck, false, "LegitBot.Trigger.VisibleCheck");
        SetupItem(&g_Options.LegitBot.Trigger.MaxDistance, 250, "LegitBot.Trigger.MaxDistance");
        SetupItem(&g_Options.LegitBot.Trigger.ReactionTime, 0, "LegitBot.Trigger.ReactionTime");

        // LegitBot - SilentAim
        SetupItem(&g_Options.LegitBot.SilentAim.LegitMode, false, "LegitBot.SilentAim.LegitMode");
        SetupItem(&g_Options.LegitBot.SilentAim.Enabled, false, "LegitBot.SilentAim.Enabled");
        SetupItem(&g_Options.LegitBot.SilentAim.ClosestFov, false, "LegitBot.SilentAim.ClosestFov");
        SetupItem(&g_Options.LegitBot.SilentAim.Fov, 10, "LegitBot.SilentAim.Fov");
        SetupItem(&g_Options.LegitBot.SilentAim.KeyBind, 0, "LegitBot.SilentAim.KeyBind");
        SetupItem(&g_Options.LegitBot.SilentAim.KeyBindState, 0, "LegitBot.SilentAim.KeyBindState");
        SetupItem(&g_Options.LegitBot.SilentAim.MissChance, 1, "LegitBot.SilentAim.MissChance");
        SetupItem(&g_Options.LegitBot.SilentAim.ShotNPC, false, "LegitBot.SilentAim.ShotNPC");
        SetupItem(&g_Options.LegitBot.SilentAim.VisibleCheck, false, "LegitBot.SilentAim.VisibleCheck");
        SetupItem(&g_Options.LegitBot.SilentAim.MaxDistance, 250, "LegitBot.SilentAim.MaxDistance");
        SetupItem(&g_Options.LegitBot.SilentAim.HitBox, 0, "LegitBot.SilentAim.HitBox");

        // Visuals - ESP - Players
        SetupItem(&g_Options.Visuals.ESP.Players.Enabled, true, "Visuals.ESP.Players.Enabled");
        SetupItem(&g_Options.Visuals.ESP.Players.ShowLocalPlayer, false, "Visuals.ESP.Players.ShowLocalPlayer");
        SetupItem(&g_Options.Visuals.ESP.Players.ShowNPCs, false, "Visuals.ESP.Players.ShowNPCs");
        SetupItem(&g_Options.Visuals.ESP.Players.UpdateESP, false, "Visuals.ESP.Players.UpdateESP");
        SetupItem(&g_Options.Visuals.ESP.Players.VisibleOnly, false, "Visuals.ESP.Players.VisibleOnly");
        SetupItem(&g_Options.Visuals.ESP.Players.ExcludeDeads, true, "Visuals.ESP.Players.ExcludeDeads");
        SetupItem(&g_Options.Visuals.ESP.Players.RenderDistance, 200, "Visuals.ESP.Players.RenderDistance");
        SetupItem(&g_Options.Visuals.ESP.Players.Box, false, "Visuals.ESP.Players.Box");
        SetupItem(&g_Options.Visuals.ESP.Players.Minimap, false, "Visuals.ESP.Players.Minimap");
        SetupItem(&g_Options.Visuals.ESP.Players.MinimapRange, 65.0f, "Visuals.ESP.Players.MinimapRange");
        SetupItem(&g_Options.Visuals.ESP.Players.Skeleton, true, "Visuals.ESP.Players.Skeleton");
        SetupItem(&g_Options.Visuals.ESP.Players.Name, true, "Visuals.ESP.Players.Name");
        SetupItem(&g_Options.Visuals.ESP.Players.HealthBar, false, "Visuals.ESP.Players.HealthBar");
        SetupItem(&g_Options.Visuals.ESP.Players.ArmorBar, false, "Visuals.ESP.Players.ArmorBar");
        SetupItem(&g_Options.Visuals.ESP.Players.WeaponName, false, "Visuals.ESP.Players.WeaponName");
        SetupItem(&g_Options.Visuals.ESP.Players.Distance, false, "Visuals.ESP.Players.Distance");
        SetupItem(&g_Options.Visuals.ESP.Players.SnapLines, false, "Visuals.ESP.Players.SnapLines");

        // Visuals - ESP - Vehicles
        SetupItem(&g_Options.Visuals.ESP.Vehicles.Enabled, false, "Visuals.ESP.Vehicles.Enabled");
        SetupItem(&g_Options.Visuals.ESP.Vehicles.Marker, false, "Visuals.ESP.Vehicles.Marker");
        SetupItem(&g_Options.Visuals.ESP.Vehicles.Door, false, "Visuals.ESP.Vehicles.Door");
        SetupItem(&g_Options.Visuals.ESP.Vehicles.Name, false, "Visuals.ESP.Vehicles.Name");
        SetupItem(&g_Options.Visuals.ESP.Vehicles.RenderDistance, 250, "Visuals.ESP.Vehicles.RenderDistance");
        SetupItem(&g_Options.Visuals.ESP.Vehicles.Distance, false, "Visuals.ESP.Vehicles.Distance");
        SetupItem(&g_Options.Visuals.ESP.Vehicles.IgnoreOccupiedVehicles, false, "Visuals.ESP.Vehicles.IgnoreOccupiedVehicles");
        SetupItem(&g_Options.Visuals.ESP.Vehicles.Model, false, "Visuals.ESP.Vehicles.Model");

        // Misc - Screen
        SetupItem(&g_Options.Misc.Screen.ShowAimbotFov, false, "Misc.Screen.ShowAimbotFov");
        SetupItem(&g_Options.Misc.Screen.ShowAimbotRGB, true, "Misc.Screen.ShowAimbotRGB");
        SetupItem(&g_Options.Misc.Screen.ShowSilentAimFov, false, "Misc.Screen.ShowSilentAimFov");

        // Misc - Exploits - LocalPlayer
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.v_Traction, 5.0f, "Misc.Exploits.LocalPlayer.v_Traction");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.boostvehicle, 0, "Misc.Exploits.LocalPlayer.boostvehicle");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.v_Boost, 1.2f, "Misc.Exploits.LocalPlayer.v_Boost");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.vehSpeed, 0.0f, "Misc.Exploits.LocalPlayer.vehSpeed");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.Shrink, false, "Misc.Exploits.LocalPlayer.Shrink");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.Invencible, false, "Misc.Exploits.LocalPlayer.Invencible");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.Invencibletest, false, "Misc.Exploits.LocalPlayer.Invencibletest");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.GodmodeBind, 0, "Misc.Exploits.LocalPlayer.GodmodeBind");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.StealCarEnabled, false, "Misc.Exploits.LocalPlayer.StealCarEnabled");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.Unlock, false, "Misc.Exploits.LocalPlayer.Unlock");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.GodmodeBindState, 0, "Misc.Exploits.LocalPlayer.GodmodeBindState");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.ModifySpeed, 1.0f, "Misc.Exploits.LocalPlayer.ModifySpeed");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.InfiniteCombatRoll, false, "Misc.Exploits.LocalPlayer.InfiniteCombatRoll");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.InfiniteStamina, false, "Misc.Exploits.LocalPlayer.InfiniteStamina");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.RemoveCollisions, false, "Misc.Exploits.LocalPlayer.RemoveCollisions");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.nospread, false, "Misc.Exploits.LocalPlayer.nospread");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.norecoil, false, "Misc.Exploits.LocalPlayer.norecoil");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.Invisible, false, "Misc.Exploits.LocalPlayer.Invisible");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.noreload, false, "Misc.Exploits.LocalPlayer.noreload");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.SeatBelt, false, "Misc.Exploits.LocalPlayer.SeatBelt");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.NoRagdoll, false, "Misc.Exploits.LocalPlayer.NoRagdoll");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.Godmodesafe, false, "Misc.Exploits.LocalPlayer.Godmodesafe");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.Godmodesafe2, false, "Misc.Exploits.LocalPlayer.Godmodesafe2");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.customspreadvalue, 0.0f, "Misc.Exploits.LocalPlayer.customspreadvalue");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.customrecoilvalue, 0.0f, "Misc.Exploits.LocalPlayer.customrecoilvalue");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.StealthMode, false, "Misc.Exploits.LocalPlayer.StealthMode");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.NoFallFromVehicle, false, "Misc.Exploits.LocalPlayer.NoFallFromVehicle");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.revive, false, "Misc.Exploits.LocalPlayer.revive");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.GodMode, false, "Misc.Exploits.LocalPlayer.GodMode");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.GodModeKey, 0, "Misc.Exploits.LocalPlayer.GodModeKey");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.GodModeKeyState, 0, "Misc.Exploits.LocalPlayer.GodModeKeyState");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.BoomFist, false, "Misc.Exploits.LocalPlayer.BoomFist");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.speed, false, "Misc.Exploits.LocalPlayer.speed");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.Noclip, false, "Misc.Exploits.LocalPlayer.Noclip");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.FreeCam, false, "Misc.Exploits.LocalPlayer.FreeCam");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.FreeCamKey, 0, "Misc.Exploits.LocalPlayer.FreeCamKey");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.FreeCamKeyState, 0, "Misc.Exploits.LocalPlayer.FreeCamKeyState");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.FreeCamSpeed, 5.0f, "Misc.Exploits.LocalPlayer.FreeCamSpeed");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.speed_value, 1.0f, "Misc.Exploits.LocalPlayer.speed_value");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.antihs, false, "Misc.Exploits.LocalPlayer.antihs");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.LockAllCars, false, "Misc.Exploits.LocalPlayer.LockAllCars");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.UnLockAllCars, false, "Misc.Exploits.LocalPlayer.UnLockAllCars");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.NoclipKey, 0, "Misc.Exploits.LocalPlayer.NoclipKey");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.NoclipKeyState, 0, "Misc.Exploits.LocalPlayer.NoclipKeyState");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.NoClipSpeed, 10.0f, "Misc.Exploits.LocalPlayer.NoClipSpeed");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.TpWayKey, 0, "Misc.Exploits.LocalPlayer.TpWayKey");
        SetupItem(&g_Options.Misc.Exploits.LocalPlayer.TpWayKeyState, 0, "Misc.Exploits.LocalPlayer.TpWayKeyState");

        // Misc - Exploits - Vehicle
        SetupItem(&g_Options.Misc.Exploits.Vehicle.GodMode, false, "Misc.Exploits.Vehicle.GodMode");
        SetupItem(&g_Options.Misc.Exploits.Vehicle.JumpingCar, false, "Misc.Exploits.Vehicle.JumpingCar");
        SetupItem(&g_Options.Misc.Exploits.Vehicle.ExplodeCar, false, "Misc.Exploits.Vehicle.ExplodeCar");
        SetupItem(&g_Options.Misc.Exploits.Vehicle.RocketBoost, false, "Misc.Exploits.Vehicle.RocketBoost");
        SetupItem(&g_Options.Misc.Exploits.Vehicle.RocketBoostJumping, false, "Misc.Exploits.Vehicle.RocketBoostJumping");
        SetupItem(&g_Options.Misc.Exploits.Vehicle.Ramp, false, "Misc.Exploits.Vehicle.Ramp");
        SetupItem(&g_Options.Misc.Exploits.Vehicle.Parachute, false, "Misc.Exploits.Vehicle.Parachute");
        SetupItem(&g_Options.Misc.Exploits.Vehicle.ExplodeOnImpact, false, "Misc.Exploits.Vehicle.ExplodeOnImpact");
        SetupItem(&g_Options.Misc.Exploits.Vehicle.Tank, false, "Misc.Exploits.Vehicle.Tank");

        // Misc - Exploits - Weapon
        SetupItem(&g_Options.Misc.Exploits.Weapon.ExplosiveAmmo, false, "Misc.Exploits.Weapon.ExplosiveAmmo");
        SetupItem(&g_Options.Misc.Exploits.Weapon.InfiniteAmmoEnabled, false, "Misc.Exploits.Weapon.InfiniteAmmoEnabled");
        SetupItem(&g_Options.Misc.Exploits.Weapon.RemoveSpread, false, "Misc.Exploits.Weapon.RemoveSpread");
        SetupItem(&g_Options.Misc.Exploits.Weapon.RemoveRecoil, false, "Misc.Exploits.Weapon.RemoveRecoil");
        SetupItem(&g_Options.Misc.Exploits.Weapon.NoReload, false, "Misc.Exploits.Weapon.NoReload");
        SetupItem(&g_Options.Misc.Exploits.Weapon.DoubleShot, false, "Misc.Exploits.Weapon.DoubleShot");

        // Misc
        SetupItem(&g_Options.Misc.ShowActiveFeaturesOverlay, false, "Misc.ShowActiveFeaturesOverlay");

        // General
        SetupItem(&g_Options.General.LogoScale, 1.0f, "General.LogoScale");
        SetupItem(&g_Options.General.ShutDown, false, "General.ShutDown");
        SetupItem(&g_Options.General.SafeMode, false, "General.SafeMode");
        SetupItem(&g_Options.General.MenuKey, VK_INSERT, "General.MenuKey");
        SetupItem(&g_Options.General.KeyBind, 0, "General.KeyBind");
        SetupItem(&g_Options.General.KeyBindState, 0, "General.KeyBindState");
        SetupItem(&g_Options.General.CaptureBypass, true, "General.CaptureBypass");
        SetupItem(&g_Options.General.WaterMark, false, "General.WaterMark");
        SetupItem(&g_Options.General.Particles, true, "General.Particles");
        SetupItem(&g_Options.General.ThreadDelay, 1, "General.ThreadDelay");
        SetupItem(&g_Options.General.FontEspStyle, 0, "General.FontEspStyle");
        SetupItem(&g_Options.General.FontLetterCase, 0, "General.FontLetterCase");
        SetupItem(&g_Options.General.FontHighlight, 2, "General.FontHighlight");
        SetupItem(&g_Options.General.FontHeight, 0.0f, "General.FontHeight");
	}

	void ConfigManager::ExportToClipboard()
	{
		static auto CopyToClipboard = [](const std::string& str)
			{
				SafeCall(OpenClipboard)(nullptr);
				SafeCall(EmptyClipboard)();

				void* hg = SafeCall(GlobalAlloc)(GMEM_MOVEABLE, str.size() + 1);

				if (!hg) {
					SafeCall(CloseClipboard)();
					return;
				}

				memcpy(SafeCall(GlobalLock)(hg), str.c_str(), str.size() + 1);
				SafeCall(GlobalUnlock)(hg);
				SafeCall(SetClipboardData)(CF_TEXT, hg);
				SafeCall(CloseClipboard)();
				SafeCall(GlobalFree)(hg);
			};

		nlohmann::json allJson;
		std::set<std::string> seenItems;

		for (auto it : Items)
		{
			if (seenItems.count(it->Name) > 0) {
				continue;
			}

			nlohmann::json j;

			j[XorStr("name")] = it->Name;
			j[XorStr("type")] = it->Type;

			if (!it->Type.compare(XorStr("int")))
				j[XorStr("value")] = (int)*(int*)it->Pointer;
			else if (!it->Type.compare(XorStr("float")))
				j[XorStr("value")] = (float)*(float*)it->Pointer;
			else if (!it->Type.compare(XorStr("bool")))
				j[XorStr("value")] = (bool)*(bool*)it->Pointer;

			allJson.push_back(j);
			seenItems.insert(it->Name);
		}

		auto str = base64::Encode((std::string(XorStr("stps5m- ")).append(allJson.dump(-1, '~'/*, true*/))).c_str());
		CopyToClipboard(str);
	}

	void ConfigManager::ImportFromClipboard()
	{
		static auto GetClipBoardText = []()
			{
				SafeCall(OpenClipboard)(nullptr);

				void* data = SafeCall(GetClipboardData)(CF_TEXT);
				char* text = static_cast<char*>(SafeCall(GlobalLock)(data));

				std::string str_text(text);

				SafeCall(GlobalUnlock)(data);
				SafeCall(CloseClipboard)();

				return str_text;
			};

		static auto find_item = [](std::vector< CConfigItem* > items, std::string name) -> CConfigItem*
			{
				for (int i = 0; i < (int)items.size(); i++)
					if (!items[i]->Name.compare(name))
						return items[i];

				return nullptr;
			};

		if (GetClipBoardText().empty()) {
			return;
		}

		auto decoded_string = base64::Decode(GetClipBoardText());
		// this a cutiehook config?
		if (decoded_string[0] != 's' ||
			decoded_string[1] != 't' ||
			decoded_string[2] != 'p' ||
			decoded_string[3] != 's' ||
			decoded_string[4] != '5' ||
			decoded_string[5] != 'm' ||
			decoded_string[6] != '-' ||
			decoded_string[7] != ' ')
			return;

		auto parsed_config = nlohmann::json::parse(decoded_string.erase(0, 8));

		nlohmann::json allJson = parsed_config;

		for (auto it = allJson.begin(); it != allJson.end(); ++it)
		{
			nlohmann::json j = *it;

			std::string name = j[XorStr("name")];
			std::string type = j[XorStr("type")];

			auto item = find_item(Items, name);

			if (item)
			{
				if (!type.compare(XorStr("int")))
					*(int*)item->Pointer = j[XorStr("value")].get<int>();
				else if (!type.compare(XorStr("float")))
					*(float*)item->Pointer = j[XorStr("value")].get<float>();
				else if (!type.compare(XorStr("bool")))
					*(bool*)item->Pointer = j[XorStr("value")].get<bool>();
			}
		}
	}
}
