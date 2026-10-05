#pragma once
#include <cstdint>
#include <mutex>

#include "Classes.hpp"

#include <FrameWork/FrameWork.hpp>

namespace Cheat
{
	enum GAME_VERSION
	{
		GAME_VERSION_GAME_b2372,
		GAME_VERSION_GTA_b2372,
		GAME_VERSION_GAME_b2612,
		GAME_VERSION_GTA_b2612,
		GAME_VERSION_GAME_b2699,
		GAME_VERSION_GTA_b2699,
		GAME_VERSION_GAME_b2189,
		GAME_VERSION_GTA_b2189,
		GAME_VERSION_GAME_b2802,
		GAME_VERSION_GTA_b2802,
		GAME_VERSION_GAME_b2060,
		GAME_VERSION_GTA_b2060,
		GAME_VERSION_GAME_b2545,
		GAME_VERSION_GAME_b3407,
		GAME_VERSION_GTA_b3407,
		GAME_VERSION_GTA_b2545,
		GAME_VERSION_GAME_b2944,
		GAME_VERSION_GAME_b3258,
		GAME_VERSION_GTA_b2944,
		GAME_VERSION_GTA_b3258,
		GAME_VERSION_GAME_b3095,
		GAME_VERSION_GTA_b3095,
		GAME_VERSION_GAME_b3570,
		GAME_VERSION_GTA_b3570,
	};

	struct PedStaticInfo
	{
		CPed* Ped;
		int iIndex = -1;
		int NetId;
		bool bIsLocalPlayer;
		bool bIsNPC;
		std::string Name;
		uint64_t crSkeletonData;
		bool bIsFriend = false;
		uint64_t PlayerInfoAddr; // Endere�o do PlayerInfo
		uint32_t IpAddress;      // IP para gerar nome �nico

		std::unordered_map<unsigned int, unsigned> MaskToBoneId;
	};

	struct Entity
	{
		PedStaticInfo StaticInfo;

		Vector3D Cordinates;
		bool Visible;
		ImVec2 HeadPos;
	};

	struct LocalPEDInfo
	{
		CPed* Ped;
		int iIndex;
		ImVec2 ScreenPos;
		Vector3D WorldPos;
	};

	struct VehicleInfo
	{
		CVehicle* Vehicle;
		std::string Name;
		uint64_t ModelInfo;
		int iIndex = -1;
		uintptr_t ptr;
	};
	float GetPlayerHeading();

	class FivemSDK
	{
	public:
		void Intialize();
		bool UpdateEntities();

		bool UpdateVehicles();
		HANDLE GetProcHandle() const { return ProcHandle; }

		DWORD GetPid() { return Pid; }
		uint64_t GetModuleBase() { return ModuleBase; };
		uint64_t GetModuleBaseSize() { return ModuleBaseSize; };
		uint64_t GetResquestRagdoll() { return RequestRagdoll; };
		LocalPEDInfo GetLocalPlayerInfo() { 
			std::scoped_lock lock(LockLists);
			return LocalPlayerInfo; 
		}
		CCamGameplayDirector* GetCamGameplayDirector() { return pCamGameplayDirector; }
		std::vector<Entity> GetEntitiyList() { 
			std::scoped_lock lock(LockLists);
			return EntityList; 
		}
		std::vector<VehicleInfo> GetVehicleList() { 
			std::scoped_lock lock(LockLists2);
			return VehicleList; 
		}
		void GetEntitiyListSnapshot(std::vector<Entity>& out) {
			std::scoped_lock lock(LockLists);
			out = EntityList;
		}
		void GetVehicleListSnapshot(std::vector<VehicleInfo>& out) {
			std::scoped_lock lock(LockLists2);
			out = VehicleList;
		}
		uint64_t GetHandleBulletAddress() { return HandleBullet; }
		uint64_t GetCanCombatRollAddress() { return CanCombatRoll; }
		uint64_t GetUpdateCamBasePositionAddress() { return UpdateCamBasePosition; }
		uint64_t GetBlipListAddress() { return BlipList; }

		CPed* GetAimingEntity();
		bool IsPlayerAiming();
		Vector3D GetBonePosVec3(Entity& Ped, unsigned int Mask);
		bool GetPedBoneIndex(Entity& Ped, unsigned int Mask, unsigned int& newIdx);
		void network_request_control_of_entity(uint64_t entity, uint64_t localplayer);

		ImVec2 GetClosestHitBox(Entity Ped);
		bool FindClosestEntity(float Fov, int MaxDistance, bool NPC, bool ClosestFov, Entity* Output);

		void ProcessCameraMovement(Vector3D WorldPosition, int SmoothHorizontal, int SmoothVertical);

		void TeleportToObject(uintptr_t Object, uintptr_t Navigation, uintptr_t ModelInfo, Vector3D Position, Vector3D VisualPosition, bool Stop);

		void SpectatePed(uint64_t Ped, bool Toggle);

		ImVec2 WorldToScreen(Vector3D Pos);
		ImVec2 WorldToScreen(Vector3D Pos, Matrix4x4 ViewMatrix);
		void UpdateViewMatrix();
		Matrix4x4 GetViewMatrix();
		bool IsOnScreen(ImVec2 Pos);

		std::string GetPlayerName(uint64_t PeerAddress, int GameNetId);
		int GetGameVersion() { return GameVersion; }

		bool IsInitialized() { return bIsIntialized; }

		bool m_safety = false;
		std::unordered_map<int, PedStaticInfo> FriendList;
		std::unordered_map<CPed*, PedStaticInfo> AllEntitesList;
		std::mutex LockLists2;

		HANDLE ProcHandle;

	private:
		bool bIsIntialized = false;

		uint64_t World;                    // 48 8B 05 ? ? ? ? 33 D2 48 8B 40 08 8A CA 48 85 C0 74 16 48 8B
		uint64_t ReplayInterface;          // 48 8B 05 ?? ?? ?? ?? 66 89 0D ?? ?? ?? ?? 4C 89 2C D0
		uint64_t ViewPort;                 // 48 8B 15 ? ? ? ? 48 8D 2D ? ? ? ? 48 8B CD
		uint64_t Camera;			       // 48 8B 05 ? ? ? ? 38 98 ? ? ? ? 8A C3
		uint64_t bIsPlayerAiming;          //
		uint64_t PlayerAimingAt;           // 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? 48 85 C9 74 05 E8 ?? ?? ?? ?? 8A CB
		uint64_t HandleBullet;             // F3 41 0F 10 19 F3 41 0F 10 41 04
		uint64_t GameplayCamHolder;        // 4C 89 2D ? ? ? ? E8 ? ? ? ? E8
		uint64_t GameplayCamTarget;        // 48 83 EC 38 0F 29 74 24 ? 0F 28 F0 0F 2F 35 ? ? ? ? 73 !!TA EM CIMA ESSA CARALHA DE SIG
		uint64_t CanCombatRoll;            // 48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F1 48 8B 89 ? ? ? ? 8B 81 < b2802 > 48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 48 8B 89 ? ? ? ? BF ? ? ? ? 8B 81
		uint64_t UpdateCamBasePosition;    // 48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 55 48 8B EC 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 48 8D 72
		uint64_t BlipList;                 // 4C 8D 35 ? ? ? ? 3B 35 ? ? ? ? 74 ? 49 8B 3E
		uint64_t NetIdToNamesPtr;		   // 48 8B CA 48 0F 45 C8
		uint64_t CitizemPlayerNamesModule;
		uint64_t RequestRagdoll;

		DWORD Pid;
		uint64_t ModuleBase;
		uint64_t ModuleBaseSize;

		std::string ModuleName;
		std::string FivemFolder;
		std::string CrashoMetryLocation;
		std::string ServerIp;
		std::string ServerPort;
		nlohmann::json PlayersInfo;
		std::unordered_map<int, std::string> PlayerIdToName;
		bool LanGame;
		int GameVersion;
		int RealGameVersion;
	private:
		CWorld* pWorld;
		CPed* pLocalPlayer;
		CReplayInterface* pReplayInterface;
		CPedInterface* pPedInterface;
		CVehicleInterface* pVehicleInterface;
		CCamGameplayDirector* pCamGameplayDirector;

		uint64_t pViewPort;
		Matrix4x4 CachedViewMatrix;

	private:
		std::mutex LockLists;
		std::vector<Entity> EntityList;
		LocalPEDInfo LocalPlayerInfo;
		std::vector<VehicleInfo> VehicleList;

	};

	inline FivemSDK g_Fivem;
}
