#include <Geode/Geode.hpp>
#include <Windows.h>
#include <TlHelp32.h>
#include <string.h>
#include <Geode/modify/AppDelegate.hpp>

using namespace geode::prelude;

std::vector<int> discordPIDs;

void updateDiscordPriority() {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (stricmp(entry.szExeFile, "discord.exe") == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ParentProcessID);

                HANDLE cProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, entry.th32ProcessID);
                log::debug("Discord: {}", entry.th32ProcessID);
                if (GetPriorityClass(cProcess) != IDLE_PRIORITY_CLASS) {
                    log::debug("SetPriorityClass()");
                    SetPriorityClass(cProcess, IDLE_PRIORITY_CLASS);
                }
                CloseHandle(cProcess);
                
                CloseHandle(hProcess);
            }
        }
    }

    CloseHandle(snapshot);
}

class $modify(AppDelegate) {
    void applicationWillEnterForeground() {
        AppDelegate::applicationWillEnterForeground();

        if (Mod::get()->getSettingValue<bool>("activate-mod")) {
            updateDiscordPriority();
        }
    }
};