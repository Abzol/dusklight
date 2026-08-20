#include "dusk/gamemode.hpp"
#include "dusk/config.hpp"
#include "JSystem/JUtility/JUTGamePad.h"
#include "aurora/lib/logging.hpp"
#include "m_Do/m_Do_MemCard.h"
#include "dusk/ui/prelaunch.hpp"

namespace dusk::gamemode {

GameModeManager g_GameModeManager;

aurora::Module DuskGameModeLog("dusk::gamemode");

GameModeManager::GameModeManager() {
    registerGameMode(GameMode(kVanillaGameModeId,"Vanilla","gczelda2"));
    mCurrentGameModeId = kVanillaGameModeId;
}

void GameModeManager::setGameModeToPrevious() {
    // Gets the value from the settings of the last played gamemode id and sets that to the current gamemode (if registered)
    GameModeId id = dusk::getSettings().game.lastSelectedGameModeId;
    if (mRegisteredGameModes.find(id) == mRegisteredGameModes.end()) {
        setCurrentGameMode(kVanillaGameModeId);
        return;
    }
    setCurrentGameMode(id);
}

void GameModeManager::registerGameMode(const GameMode& gamemode) {
    if (gamemode.getId().empty()) {
        DuskGameModeLog.fatal("No gamemode id specified in GameModeManager::registerGameMode!");
    }
    if (gamemode.getSaveName().empty()) {
        DuskGameModeLog.fatal("No save name provided for gamemode {}", gamemode.getId());
    }
    if (gamemode.getFullName().empty()) {
        DuskGameModeLog.fatal("No Name Specified for gamemode {}", gamemode.getId());
    }

    if (mRegisteredGameModes.find(gamemode.getId()) != mRegisteredGameModes.end()) {
        DuskGameModeLog.warn("Attempting to register gamemode {} when it is already registered!", gamemode.getId());
        return;
    }

    mRegisteredGameModes.emplace(gamemode.getId(),gamemode);
    dusk::ui::Prelaunch::rebuild_menu_buttons();
}

void GameModeManager::unregisterGameMode(const GameModeId& gamemodeId) {
    const auto& it = mRegisteredGameModes.find(gamemodeId);
    if (it == mRegisteredGameModes.end()) {
        DuskGameModeLog.warn(
            "Attempting to unregister gamemode of id {} that isn't registered!", gamemodeId);
        return;
    }

    if (mCurrentGameModeId == gamemodeId) {
        // We need to be careful if we are unregistering a running gamemode, the easiest way is just
        // to reset the game back to title as vanilla;
        ui::prelaunch_state().showPrelaunchOnReset = true;
        JUTGamePad::C3ButtonReset::sResetSwitchPushing = true;
        setCurrentGameMode(kVanillaGameModeId);
    }
    mRegisteredGameModes.erase(it);
    dusk::ui::Prelaunch::rebuild_menu_buttons();
}

void GameModeManager::setCurrentGameMode(const GameModeId& id) {
    if (mCurrentGameModeId == id) {
        return;
    }
    const GameMode* currentGameMode = getCurrentGameMode();
    if (currentGameMode) {
        currentGameMode->invokeOnDeactivatedFunction();
    }
    if (mRegisteredGameModes.find(id) == mRegisteredGameModes.end()) {
        DuskGameModeLog.warn("Attempting to set current game mode to {} when it hasn't been registered!", id);
    }

    mCurrentGameModeId = id;
    dusk::getSettings().game.lastSelectedGameModeId.setValue(id);
    dusk::config::save();

    currentGameMode = getCurrentGameMode();
    if (currentGameMode) {
        // Set the loaded save file to our gamemode's save name
        mDoMemCd_SetFileName(currentGameMode->mSaveName);
        currentGameMode->invokeOnActivatedFunction();
    }
}

};  // namespace dusk::gamemode
