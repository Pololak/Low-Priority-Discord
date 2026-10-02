#include <Geode/Geode.hpp>
#include <Windows.h>
#include <TlHelp32.h>
#include <string.h>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

void updateDiscordPriority() {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (_stricmp(entry.szExeFile, "discord.exe") == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ParentProcessID);

                if (hProcess) {
                    HANDLE cProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, entry.th32ProcessID);

                    if (cProcess) {
                        log::debug("DiscordPID: {}", entry.th32ProcessID);
                        if (GetPriorityClass(cProcess) != IDLE_PRIORITY_CLASS) {
                            log::debug("Set lowest priority to {} with PID {}", entry.szExeFile, entry.th32ParentProcessID);
                            SetPriorityClass(cProcess, IDLE_PRIORITY_CLASS);
                        }
                    }
                    CloseHandle(cProcess);
                }
                CloseHandle(hProcess);
            }
        }
    }

    CloseHandle(snapshot);
}

class $modify(AppDelegate) {
    void applicationWillEnterForeground() {
        AppDelegate::applicationWillEnterForeground();

        if (Mod::get()->getSettingValue<bool>("mod-enabled") && Mod::get()->getSettingValue<bool>("on-unminimize")) {
            updateDiscordPriority();
        }
    }
};

class $modify(PlayLayer) {
    void resetLevel() {
        PlayLayer::resetLevel();

        if (Mod::get()->getSettingValue<bool>("mod-enabled") && Mod::get()->getSettingValue<bool>("on-reset-level")) {
            updateDiscordPriority();
        }
    }
};
