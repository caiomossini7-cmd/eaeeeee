#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>

namespace Cheat
{
    struct Options
    {
        struct LegitBot_t
        {
            struct AimBot_t
            {
                bool Enabled = false;
                bool LegitMode = false;
                bool ClosestFov = false;
                int KeyBind = 0;
                int KeyBindState = 0;
                bool TargetNPC = false;
                bool VisibleCheck = false;
                bool IgnoreFriends = false;
                int SmoothHorizontal = 10;
                int SmoothVertical = 10;
                int MaxDistance = 300;
                int FOV = 100;
                int HitBox = 0;

                bool IsFriend(const std::string& name) const;
                bool IsFriend(const char* name) const;
                template<typename T>
                bool IsFriend(const T&) const { return false; }
            } AimBot;

            struct SilentAim_t
            {
                bool Enabled = false;
                bool LegitMode = false;
                bool ClosestFov = false;
                int KeyBind = 0;
                int KeyBindState = 0;
                bool ShotNPC = false;
                bool VisibleCheck = false;
                bool IgnoreFriends = false;
                int MaxDistance = 300;
                int Fov = 100;
                int HitBox = 0;
                int MissChance = 0;
            } SilentAim;

            struct MagicBullet_t
            {
                bool Enabled = false;
                int KeyBind = 0;
                int KeyBindState = 0;
            } MagicBullet;

            struct TriggerBot_t
            {
                bool Enabled = false;
                int KeyBind = 0;
                int KeyBindState = 0;
                bool ShotNPC = false;
                bool VisibleCheck = false;
                bool IgnoreFriends = false;
                int MaxDistance = 300;
                int ReactionTime = 0;
            } Trigger;
        } LegitBot;

        struct Visuals_t
        {
            struct ESP_t
            {
                struct Players_t
                {
                    bool Enabled = false;
                    bool ShowNPCs = false;
                    bool ShowLocalPlayer = false;
                    bool UpdateESP = false;
                    bool VisibleOnly = false;
                    bool ExcludeDeads = false;
                    bool Minimap = false;
                    float MinimapRange = 100.0f;
                    int RenderDistance = 300;
                    bool Box = false;
                    bool Distance = false;
                    int DistanceState = 0;
                    float DistanceRawPos[2] = { 0.0f, 0.0f };
                    bool Skeleton = false;
                    bool HealthBar = false;
                    int HealthBarState = 0;
                    float HealthBarRawPos[2] = { 0.0f, 0.0f };
                    bool ArmorBar = false;
                    int ArmorBarState = 0;
                    float ArmorBarRawPos[2] = { 0.0f, 0.0f };
                    bool Name = false;
                    int NameState = 0;
                    float NameRawPos[2] = { 0.0f, 0.0f };
                    bool WeaponName = false;
                    int WeaponNameState = 0;
                    float WeaponNameRawPos[2] = { 0.0f, 0.0f };
                    bool SnapLines = false;
                    bool HighlightFriends = false;
                    float BoxColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    float SkeletonColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    float TextColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    float SnapLinesColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    float HealthBarColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                    float ArmorColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
                    float FriendColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                    float FriendSkeletonColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                    float FriendsSkeletonColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
                } Players;

                struct Vehicles_t
                {
                    bool Enabled = false;
                    bool Model = false;
                    bool Name = false;
                    bool Distance = false;
                    bool Marker = false;
                    float MarkerColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    bool Door = false;
                    bool IgnoreOccupiedVehicles = false;
                    int RenderDistance = 300;
                    float Color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    float TextColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                } Vehicles;
            } ESP;
        } Visuals;

        struct Misc_t
        {
            bool ShowActiveFeaturesOverlay = false;

            struct Screen_t
            {
                bool ShowAimbotFov = false;
                bool ShowSilentAimFov = false;
                bool ShowAimbotRGB = false;
                float AimbotFovColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                float SilentFovColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
            } Screen;

            struct Exploits_t
            {
                struct LocalPlayer_t
                {
                    bool GodMode = false;
                    int GodModeKey = 0;
                    int GodModeKeyState = 0;
                    int GodmodeBind = 0;
                    int GodmodeBindState = 0;
                    bool Godmodesafe = false;
                    bool Godmodesafe2 = false;
                    bool Invincible = false;
                    bool InvincibleTest = false;
                    bool Invencible = false;
                    bool Invencibletest = false;
                    bool SemiGod = false;
                    bool Noclip = false;
                    bool FreeCam = false;
                    int FreeCamKey = 0;
                    int FreeCamKeyState = 0;
                    float FreeCamSpeed = 5.0f;
                    int NoclipKey = 0;
                    int NoclipKeyState = 0;
                    float NoClipSpeed = 5.0f;
                    bool Invisible = false;
                    bool Shrink = false;
                    bool NoRagdoll = false;
                    bool InfiniteStamina = false;
                    bool InfiniteCombatRoll = false;
                    bool SeatBelt = false;
                    int TpWayKey = 0;
                    int TpWayKeyState = 0;
                    bool antihs = false;
                    bool LockAllCars = false;
                    bool UnLockAllCars = false;
                    bool ExplosiveMelee = false;
                    float RunSpeed = 1.0f;

                    // Vehicle & Speed modifier fields
                    float v_Traction = 0.0f;
                    bool boostvehicle = false;
                    float v_Boost = 0.0f;
                    float vehSpeed = 0.0f;
                    bool StealCarEnabled = false;
                    bool Unlock = false;
                    bool ModifySpeed = false;
                    bool RemoveCollisions = false;
                    bool NoFallFromVehicle = false;
                    bool speed = false;
                    float speed_value = 1.0f;

                    // Weapon & Combat exploit fields
                    bool nospread = false;
                    bool norecoil = false;
                    bool noreload = false;
                    float customspreadvalue = 0.0f;
                    float customrecoilvalue = 0.0f;
                    bool StealthMode = false;
                    bool revive = false;
                    bool BoomFist = false;
                } LocalPlayer;

                struct Weapon_t
                {
                    bool RemoveSpread = false;
                    bool RemoveRecoil = false;
                    float WeaponRange = 250.0f;
                    bool InfiniteAmmoEnabled = false;
                    bool ExplosiveAmmo = false;
                    bool NoReload = false;
                    bool DoubleShot = false;
                    bool RapidFire = false;
                } Weapon;

                struct Vehicle_t
                {
                    bool VehicleGodmode = false;
                    bool GodMode = false;
                    bool RocketBoost = false;
                    bool JumpingCar = false;
                    bool RocketBoostJumping = false;
                    bool ExplodeCar = false;
                    bool Ramp = false;
                    bool Parachute = false;
                    bool ExplodeOnImpact = false;
                    bool Tank = false;
                    bool StickyTires = false;
                    int RepairKey = 0;
                    int RepairKeyState = 0;
                    float PrimaryColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
                    float SecondaryColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                    float WheelColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    bool UpdateCarColor = false;
                } Vehicle;
            } Exploits;
        } Misc;

        struct General_t
        {
            std::string UserName = "";
            std::string UserRole = "";
            float PrimaryColor[4] = { 0.9f, 0.17f, 0.17f, 1.0f };
            bool SafeMode = false;
            int MenuKey = VK_INSERT;
            int KeyBind = 0;
            int KeyBindState = 0;
            int ThreadDelay = 1;
            bool CaptureBypass = false;
            bool Particles = false;
            bool EspOnSecondaryMonitor = false;
            bool ShutDown = false;
            bool WebRemoteEnabled = false;
            int WebRemotePort = 8080;
            float LogoScale = 1.0f;
            bool WaterMark = false;
            int FontEspStyle = 0;
            int FontLetterCase = 0;
            int FontHighlight = 0;
            float FontHeight = 12.0f;

            std::vector<std::string> FriendsList;
        } General;

        bool IsFriend(const std::string& name) const
        {
            return std::find(General.FriendsList.begin(), General.FriendsList.end(), name) != General.FriendsList.end();
        }
    };
}

inline Cheat::Options g_Options;

namespace Cheat
{
    using ::g_Options;

    inline bool Options::LegitBot_t::AimBot_t::IsFriend(const std::string& name) const
    {
        return std::find(::g_Options.General.FriendsList.begin(), ::g_Options.General.FriendsList.end(), name) != ::g_Options.General.FriendsList.end();
    }

    inline bool Options::LegitBot_t::AimBot_t::IsFriend(const char* name) const
    {
        if (!name) return false;
        return IsFriend(std::string(name));
    }
}
