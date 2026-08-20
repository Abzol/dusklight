#pragma once
#include <functional>
#include <map>
#include "d/d_file_select.h"

namespace dusk::gamemode {
using GameModeId = std::string;

constexpr const char* kVanillaGameModeId = "vanilla";

// This class holds the definition for the gamemode and various function pointers to call
class GameMode {
public:
    GameMode(const GameModeId& id, const std::string& fullName, const std::string& saveName) {
        mId = id;
        mFullName = fullName;
        mSaveName = saveName;
    }
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
    
    void invokeOnNewSaveSelectFunction(bool* out_proceedToNameSelect, bool* out_returnToFileSelect) const {
        if (mOnNewSaveSelectFunction) {
            mOnNewSaveSelectFunction(out_proceedToNameSelect, out_returnToFileSelect);
        }else {
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
    std::function<void(bool* out_proceedToNameSelect, bool* out_returnToFileSelect)> mOnNewSaveSelectFunction;
    std::function<void()> mOnGameResetFunction;
    std::function<void()> mOnTickFunction;
};

class GameModeManager {
public:
    GameModeManager();
    void registerGameMode(const GameMode& gamemode);
    void unregisterGameMode(const GameModeId& gamemodeId);

    const GameMode* getCurrentGameMode() const {
        const auto& it = mRegisteredGameModes.find(mCurrentGameModeId);
        return it != mRegisteredGameModes.end() ? &it->second : &mRegisteredGameModes.at(kVanillaGameModeId);
    }
    bool isCurrentGameMode(const GameModeId& id) const {
        const GameMode* gamemode = getCurrentGameMode();
        if (gamemode && gamemode->getId() == id) {
            return true;
        }
        return false;
    }
    void setCurrentGameMode(const GameModeId& id);
    void setGameModeToPrevious();

    std::map<GameModeId, GameMode>& getRegisteredGameModes() { return mRegisteredGameModes; }

private:
    GameModeId mCurrentGameModeId;
    std::map<GameModeId, GameMode> mRegisteredGameModes;
};

extern GameModeManager g_GameModeManager;

inline GameModeManager& getGameModeManager() {
    return g_GameModeManager;
}

};  // namespace dusk::gamemode
