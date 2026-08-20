#pragma once

#include "d/d_file_select.h"

#include <functional>
#include <map>
#include <string>
#include <utility>

namespace dusk::gamemode {
using GameModeId = std::string;

constexpr const char* kVanillaGameModeId = "vanilla";
constexpr const char* kDefaultGameModeSaveName = "gczelda2";

// Holds a game mode definition and its lifecycle callbacks.
class GameMode {
public:
    GameMode(GameModeId id, std::string fullName, std::string saveName = {})
        : mId{std::move(id)}, mFullName{std::move(fullName)},
          mSaveName{saveName.empty() ? kDefaultGameModeSaveName : std::move(saveName)} {}
    const GameModeId& getId() const { return mId; }
    const std::string& getFullName() const { return mFullName; }
    const std::string& getSaveName() const { return mSaveName; }

    GameModeId mId;
    std::string mFullName;
    std::string mSaveName;

    void invokeOnActivatedFunction() const {
        if (mOnActivatedFunction) {
            mOnActivatedFunction();
        }
    }

    void invokeOnDeactivatedFunction() const {
        if (mOnDeactivatedFunction) {
            mOnDeactivatedFunction();
        }
    }

    void invokeOnPlayFunction() const {
        if (mOnPlayFunction) {
            mOnPlayFunction();
        }
    }

    void invokeOnSaveLoadedFunction() const {
        if (mOnSaveLoadedFunction) {
            mOnSaveLoadedFunction();
        }
    }

    void invokeOnNewSaveFunction() const {
        if (mOnNewSaveFunction) {
            mOnNewSaveFunction();
        }
    }

    void invokeOnNewSaveSelectFunction(
        bool* out_proceedToNameSelect, bool* out_returnToFileSelect) const {
        if (mOnNewSaveSelectFunction) {
            mOnNewSaveSelectFunction(out_proceedToNameSelect, out_returnToFileSelect);
        } else {
            *out_proceedToNameSelect = true;
        }
    }

    void invokeOnGameResetFunction() const {
        if (mOnGameResetFunction) {
            mOnGameResetFunction();
        }
    }

    void invokeOnTickFunction() const {
        if (mOnTickFunction) {
            mOnTickFunction();
        }
    }

    std::function<void()> mOnActivatedFunction;
    std::function<void()> mOnDeactivatedFunction;
    std::function<void()> mOnPlayFunction;
    std::function<void()> mOnSaveLoadedFunction;
    std::function<void()> mOnNewSaveFunction;
    std::function<void(bool* out_proceedToNameSelect, bool* out_returnToFileSelect)>
        mOnNewSaveSelectFunction;
    std::function<void()> mOnGameResetFunction;
    std::function<void()> mOnTickFunction;
};

class GameModeManager {
public:
    GameModeManager();
    void registerGameMode(const GameMode& gameMode);
    void unregisterGameMode(const GameModeId& gameModeId);

    const GameMode* getCurrentGameMode() const {
        const auto& it = mRegisteredGameModes.find(mCurrentGameModeId);
        return it != mRegisteredGameModes.end() ? &it->second :
                                                  &mRegisteredGameModes.at(kVanillaGameModeId);
    }
    bool isCurrentGameMode(const GameModeId& id) const {
        const GameMode* gameMode = getCurrentGameMode();
        if (gameMode && gameMode->getId() == id) {
            return true;
        }
        return false;
    }
    void setCurrentGameMode(const GameModeId& id);
    void setGameModeToPrevious();

    const std::map<GameModeId, GameMode>& getRegisteredGameModes() const {
        return mRegisteredGameModes;
    }

private:
    GameModeId mCurrentGameModeId;
    std::map<GameModeId, GameMode> mRegisteredGameModes;
};

extern GameModeManager g_GameModeManager;

inline GameModeManager& getGameModeManager() {
    return g_GameModeManager;
}

}  // namespace dusk::gamemode
