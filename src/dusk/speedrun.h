#pragma once
#include <aurora/aurora.h>
#include "dusk/gamemode.hpp"

#define DUSK_SPEEDRUN_GAMEMODE_ID "vanilla_speedrun"

namespace dusk::speedrun {

struct SpeedrunInfo {
    void startRun() {
        m_isRunStarted = true;
        m_rtaStartTimestamp = OSGetNativeTime();
        m_igtStartTimestamp = OSGetTime();
    }

    void stopRun() {
        m_isRunStarted = false;
        m_rtaTimer = OSGetNativeTime() - m_rtaStartTimestamp;
        if (!m_isPauseIGT) {
            m_igtTimer = OSGetTime() - m_igtStartTimestamp - m_totalLoadTime;
        }
    }

    void reset() {
        m_isRunStarted = false;
        m_rtaStartTimestamp = 0;
        m_rtaTimer = 0;
        m_igtStartTimestamp = 0;
        m_isPauseIGT = false;
        m_loadStartTimestamp = 0;
        m_totalLoadTime = 0;
        m_igtTimer = 0;
    }

    bool m_isRunStarted = false;
    OSTime m_rtaStartTimestamp = 0;
    OSTime m_rtaTimer = 0;
    OSTime m_igtStartTimestamp = 0;

    bool m_isPauseIGT = false;
    OSTime m_loadStartTimestamp = 0;
    OSTime m_totalLoadTime = 0;
    OSTime m_igtTimer = 0;
};

extern SpeedrunInfo g_speedrunInfo;

void registerSpeedrunGamemode();
void unregisterSpeedrunGamemode();
void resetForSpeedrunMode();
void restoreFromSpeedrunMode();

inline bool isActive() {
    return dusk::gamemode::getGamemodeManager().isCurrentGamemode(DUSK_SPEEDRUN_GAMEMODE_ID);
}

}  // namespace dusk
