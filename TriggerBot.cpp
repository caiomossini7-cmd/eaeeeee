// by HyperX
#include "TriggerBot.hpp"

#include <thread>

#include "../../Options.hpp"

namespace Cheat
{
	void TriggerBot::RunThread()
	{
		while (!g_Options.General.ShutDown)
		{
			std::this_thread::sleep_for(std::chrono::seconds(2 + g_Options.General.ThreadDelay));

			if (!g_Options.LegitBot.Trigger.Enabled)
				continue;

			if (!g_Fivem.GetLocalPlayerInfo().Ped)
				continue;

			static bool Shooting = false;

			bool CanShoot = false;

			if (!SafeCall(GetAsyncKeyState)(g_Options.LegitBot.Trigger.KeyBind))
			{
				if (Shooting)
				{
					INPUT input = { 0 };
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					SafeCall(SendInput)(1, &input, sizeof(INPUT));

					Shooting = false;
				}

				continue;
			}

			if (CanShoot && !Shooting)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(g_Options.LegitBot.Trigger.ReactionTime));

				INPUT input = { 0 };
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				SafeCall(SendInput)(1, &input, sizeof(INPUT));

				Shooting = true;
			}
			else if (Shooting && !CanShoot)
			{
				INPUT input = { 0 };
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				SafeCall(SendInput)(1, &input, sizeof(INPUT));

				Shooting = false;
			}
		}
	}
}