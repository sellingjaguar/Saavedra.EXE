// Luis Silva e filhos

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <algorithm>

#include "offsets.hpp"

using namespace std;
using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

uintptr_t moduleBase;
DWORD procId;
HWND hwnd;
HANDLE hProcess;

int saavedra = 69; //junk code OMG undetectable hash brute force generator

//weapon ids

//pistols
int usp_s = 262205;
int usp_s_2 = 61;
int p2000 = 262167;
int p2000_2 = 32;
int dual_berettas = 2;
int p250 = 36;
int five_seven = 3;
int deagle = 1;
int glock = 4;
int tec9 = 30;
int cz75 = 262207;
int cz75_2 = 63;

//heavy
int nova = 35;
int xm = 25;
int mag7 = 27;
int sawed_off = 29;
int m249 = 14;
int negev = 28;

//smg
int mp9 = 34;
int mp7 = 262177;
int mp7_2 = 33;
int ump = 24;
int p90 = 19;
int bizon = 26;
int mp5 = 262167;
int mp5_2 = 23;
int mac10 = 17;

//rifles
int famas = 10;
int m4a4 = 16;
int m4a1s = 262204;
int m4a1s_2 = 60;
int ak47 = 7;
int scout = 40;
int ssg = 39;
int aug = 8;
int awp = 9;
int scar = 38;
int g3 = 262155;
int g3_2 = 11;

//grenades
int flash = 43;
int he = 44;
int smoke = 45;
int molotov = 46;
int decoy = 47;
int incendiary = 48;

//other stuff
int zeus = 31;
int knife_T = 59;
int knife_CT = 42;
int c4 = 49;

//memory stuff

uintptr_t GetModuleBaseAddress(const char* modName) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!strcmp(modEntry.szModule, modName)) {
					CloseHandle(hSnap);
					return (uintptr_t)modEntry.modBaseAddr;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
}

template<typename T> T RPM(SIZE_T address) {
	T buffer;
	ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
	return buffer;
}

template<typename T> void WPM(SIZE_T address, T buffer) {
	WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), NULL);
}

uintptr_t getLocalPlayer() {
	return RPM<uintptr_t>(moduleBase + dwLocalPlayer);
}

//glow
struct glowStructEnemyFull {
	float red = 0.f;
	float green = 1.f;
	float blue = 0.f;
	float alpha = 1.f;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}glowEnmFull;

struct glowStructEnemyMed {
	float red = 1.f;
	float green = 1.f;
	float blue = 0.f;
	float alpha = 1.f;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}glowEnmMed;

struct glowStructEnemyLow {
	float red = 1.f;
	float green = 0.f;
	float blue = 0.f;
	float alpha = 1.f;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}glowEnmLow;

struct glowStructEnemyDef {
	float red = 0.f;
	float green = 0.f;
	float blue = 1.f;
	float alpha = 1.f;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}glowEnmDef;

void glow() {

	uintptr_t dwGlowManager = RPM<uintptr_t>(moduleBase + dwGlowObjectManager);
	int LocalTeam = RPM<int>(getLocalPlayer() + m_iTeamNum);

	for (int i = 1; i < 32; i++) {

		uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
		int iGlowIndx = RPM<int>(dwEntity + m_iGlowIndex);
		int EnmHealth = RPM<int>(dwEntity + m_iHealth); if (EnmHealth < 1 || EnmHealth > 100) continue;
		int Dormant = RPM<int>(dwEntity + m_bDormant); if (Dormant) continue;
		int EntityTeam = RPM<int>(dwEntity + m_iTeamNum);

		if (LocalTeam != EntityTeam)
		{
			//health based glow
			if (EnmHealth > 60) {
				WPM<glowStructEnemyFull>(dwGlowManager + (iGlowIndx * 0x38) + 0x4, glowEnmFull);
			}
			else if (EnmHealth > 15){
				WPM<glowStructEnemyMed>(dwGlowManager + (iGlowIndx * 0x38) + 0x4, glowEnmMed);
			}
			else {
				WPM<glowStructEnemyLow>(dwGlowManager + (iGlowIndx * 0x38) + 0x4, glowEnmLow);
			}
		}
	}
}

//chams
void chams() {

	int LocalTeam = RPM<int>(getLocalPlayer() + m_iTeamNum);

	for (int i = 1; i < 32; i++) {

		uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + dwEntityList + i * 0x10);

		int EntityTeam = RPM<int>(dwEntity + m_iTeamNum);

		//enemy
		if (LocalTeam != EntityTeam) {
			WPM<int>(dwEntity + m_clrRender, 0);			//R
			WPM<int>(dwEntity + m_clrRender + 0x1, 255);	//G
			WPM<int>(dwEntity + m_clrRender + 0x2, 0);		//B
		}
		else {
			WPM<int>(dwEntity + m_clrRender, 0);			//R
			WPM<int>(dwEntity + m_clrRender + 0x1, 0);	    //G
			WPM<int>(dwEntity + m_clrRender + 0x2, 0);		//B
		}

	}
}

//radar
void radar(){
	for (int i = 1; i < 32; i++) {

		uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + dwEntityList + i * 0x10);

		WPM<int>(dwEntity + m_bSpotted, 1);

	}
}

//trigger bot
int scopeable[] = { awp, scout, scar, g3, g3_2 };

bool valid_shoot(int id) {

	int scoping = RPM<int>(getLocalPlayer() + m_bIsScoped);

	if (std::any_of(std::begin(scopeable), std::end(scopeable), [=](int n) {return n == id; }) && !scoping) {
		return false;
	}
	else {
		return true;
	}

}

void trigger_bot() {

	int myTeam = RPM<int>(getLocalPlayer() + m_iTeamNum);

	int crosshairID = RPM<int>(getLocalPlayer() + m_iCrosshairId);
	int entityTeam = RPM<int>(moduleBase + dwEntityList + ((crosshairID - 1) * 0x10));
	int crosshairTeam = RPM<int>(entityTeam + m_iTeamNum);

	int weapon = RPM<int>(getLocalPlayer() + m_hActiveWeapon);
	int weapon_ent = RPM<int>(moduleBase + dwEntityList + ((weapon & 0xFFF) - 1) * 0x10);
	int weapon_id = RPM<int>(weapon_ent + m_iItemDefinitionIndex);


	if (crosshairID > 0 && crosshairID < 32 && myTeam != crosshairTeam	&& valid_shoot(weapon_id)) {
		//WPM<int>(moduleBase + dwForceAttack, 6);
		mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, 0, 0);
	}

}

//no flash
void no_flash() {
	
	int flash_value = getLocalPlayer() + m_flFlashMaxAlpha;
	WPM<float>(flash_value, 0.0);

}

//auto pistol
int x = 0;
int pistols[] = {glock, usp_s, usp_s_2, p2000, p2000_2, p250, cz75, cz75_2, tec9, five_seven, deagle, dual_berettas };
void auto_pistol() {

	if (GetKeyState(VK_LBUTTON) < 0)
	{

		int weapon = RPM<int>(getLocalPlayer() + m_hActiveWeapon);
		int weapon_ent = RPM<int>(moduleBase + dwEntityList + ((weapon & 0xFFF) - 1) * 0x10);
		int weapon_id = RPM<int>(weapon_ent + m_iItemDefinitionIndex);

		if (std::any_of(std::begin(pistols), std::end(pistols), [=](int n) {return n == weapon_id; })) {
			WPM<int>(moduleBase + dwForceAttack, 6);
		}

	}

}

//bhop
void bhop() {

	int buffer;

	uintptr_t localPlayer = RPM<uintptr_t>(moduleBase + dwEntityList);

	int flags = RPM<int>(localPlayer + m_fFlags);

	if (flags & 1) {

		buffer = 5;

	}
	else {

		buffer = 4;

	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {

		WPM(moduleBase + dwForceJump, buffer);

	}

}

//int invalid_zoom[] = { usp_s, usp_s_2, glock, famas, m4a1s, m4a1s_2, aug, ssg, awp, scout, scar, g3, g3_2, flash, he, decoy, molotov, incendiary, smoke};

void fov_changer() {

	int fov = RPM<int>(getLocalPlayer() + m_iFOV);

	if (GetKeyState(VK_LEFT))
	{

		WPM<int>(getLocalPlayer() + m_iFOV, fov - 1);
		Sleep(10);

	}
	else if (GetKeyState(VK_RIGHT))
	{

		WPM<int>(getLocalPlayer() + m_iFOV, fov + 1);
		Sleep(10);

	}

}

void cmd_menu(bool a, bool b, bool c, bool d, bool e, bool f ,bool g) {
	system("cls");
	system("color A");
	cout << "Saavedra.EXE (not python edition)\n" << endl;
	cout << "F1 - Wallhack    - " << int(a) << endl;
	cout << "F2 - Radar       - " << int(b) << endl;
	cout << "F3 - TriggerBot  - " << int(c) << endl;
	cout << "F4 - NoFlash     - " << int(d) << endl;
	cout << "F5 - AutoPistol  - " << int(e) << " broken" << endl;
	cout << "F6 - BHop        - " << int(f) << endl;
	cout << "F7 - Fov changer - " << int(g) << " broken" << endl;
	cout << "\nEND to close" << endl;
}

int main() {
	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(hwnd, &procId);
	moduleBase = GetModuleBaseAddress("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	//cheat loop
	bool glow_ = false, radar_ = false, trigger_ = false, no_flash_ = false, auto_pistol_ = false, bhop_ = false, zoom_ = false;

	cmd_menu(glow_, radar_, trigger_, no_flash_, auto_pistol_, bhop_, zoom_);

	while (!GetAsyncKeyState(VK_END))
	{
		chams();

		if (glow_) {
			glow();
		}

		if (radar_) {
			radar();
		}

		if (trigger_) {
			trigger_bot();
		}

		if (no_flash_) {
			no_flash();
		}

		/*if (auto_pistol_) {
			auto_pistol();
		}*/

		if (bhop_) {
			bhop();
		}

		/*if (zoom_) {
			fov_changer();
		}*/

		//toggles
		if (GetAsyncKeyState(VK_F1)) {
			glow_ = !glow_;
			cmd_menu(glow_, radar_, trigger_, no_flash_, auto_pistol_, bhop_, zoom_);
			Sleep(100);
		}
		else if (GetAsyncKeyState(VK_F2)) {
			radar_ = !radar_;
			cmd_menu(glow_, radar_, trigger_, no_flash_, auto_pistol_, bhop_, zoom_);
			Sleep(100);
		}
		else if (GetAsyncKeyState(VK_F3)) {
			trigger_ = !trigger_;
			cmd_menu(glow_, radar_, trigger_, no_flash_, auto_pistol_, bhop_, zoom_);
			Sleep(100);
		}
		else if (GetAsyncKeyState(VK_F4)) {
			no_flash_ = !no_flash_;
			cmd_menu(glow_, radar_, trigger_, no_flash_, auto_pistol_, bhop_, zoom_);
			Sleep(100);
		}
		/*else if (GetAsyncKeyState(VK_F5)) {
			auto_pistol_ = !auto_pistol_;
			cmd_menu(glow_, radar_, trigger_, no_flash_, auto_pistol_, bhop_, zoom_);
			Sleep(100);
		}*/
		else if (GetAsyncKeyState(VK_F6)) {
			bhop_ = !bhop_;
			cmd_menu(glow_, radar_, trigger_, no_flash_, auto_pistol_, bhop_, zoom_);
			Sleep(100);
		}
		/*else if (GetAsyncKeyState(VK_F7)) {
			zoom_ = !zoom_;
			cmd_menu(glow_, radar_, trigger_, no_flash_, auto_pistol_, bhop_, zoom_);
			Sleep(100);
		}*/
	}
}
