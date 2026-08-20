#include "mods/svc/game_mode.h"
#include "dusk/game_mode.hpp"

#include "config.hpp"
#include "registry.hpp"
#include "slot_map.hpp"

#include "aurora/lib/logging.hpp"
#include "dusk/mod_loader.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

namespace dusk::mods::svc::game_mode_impl {
namespace {

aurora::Module Log("dusk::mods::game_mode");

// Track which gamemodes are registered by which mods, allowing us to automatically unregister them
std::unordered_map<std::string, std::vector<std::string>> s_gameModesByMod;

std::string get_mod_game_mode_id(ModContext* ctx, const std::string& id) {
    // Include the mod ID to prevent clashes and normalize to lowercase
    std::string fullId = id + "_" + ctx->mod->metadata.id;
    std::transform(fullId.begin(), fullId.end(), fullId.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return fullId;
}

void game_mode_remove_mod(LoadedMod& mod) {
    const auto it = s_gameModesByMod.find(mod.metadata.id);
    if (it != s_gameModesByMod.end()) {
        for (const auto& id : it->second) {
            gamemode::getGameModeManager().unregisterGameMode(id);
        }
        s_gameModesByMod.erase(it);
    }
}
}  // namespace

ModResult register_game_mode(ModContext* ctx, const GameModeDesc* desc) {
    std::string id;
    if (!desc->gameModeId) {
        Log.error("Attempted to register a game mode with a null ID");
        return MOD_ERROR;
    }
    id = desc->gameModeId;
    if (id.empty()) {
        Log.error("Attempted to register a game mode with an empty ID");
        return MOD_ERROR;
    }
    id = get_mod_game_mode_id(ctx, id);

    std::string fullName;
    if (!desc->fullName) {
        Log.warn("Game mode {} has no display name; using its ID", id);
        fullName = id;
    } else {
        fullName = desc->fullName;
        if (fullName.empty()) {
            Log.warn("Game mode {} has an empty display name; using its ID", id);
            fullName = id;
        }
    }

    gamemode::GameMode mode{id, fullName, desc->saveName};
    if (desc->onActivatedFunction) {
        mode.mOnActivatedFunction = desc->onActivatedFunction;
    }
    if (desc->onDeactivatedFunction) {
        mode.mOnDeactivatedFunction = desc->onDeactivatedFunction;
    }
    if (desc->onPlayFunction) {
        mode.mOnPlayFunction = desc->onPlayFunction;
    }
    if (desc->onSaveLoadedFunction) {
        mode.mOnSaveLoadedFunction = desc->onSaveLoadedFunction;
    }
    if (desc->onNewSaveFunction) {
        mode.mOnNewSaveFunction = desc->onNewSaveFunction;
    }
    if (desc->onNewSaveSelectFunction) {
        mode.mOnNewSaveSelectFunction = desc->onNewSaveSelectFunction;
    }
    if (desc->onGameResetFunction) {
        mode.mOnGameResetFunction = desc->onGameResetFunction;
    }
    if (desc->onTickFunction) {
        mode.mOnTickFunction = desc->onTickFunction;
    }

    gamemode::getGameModeManager().registerGameMode(mode);
    s_gameModesByMod[ctx->mod->metadata.id].push_back(id);
    return MOD_OK;
}

ModResult unregister_game_mode(ModContext* ctx, const char* id) {
    std::string fullId = get_mod_game_mode_id(ctx, id);
    gamemode::getGameModeManager().unregisterGameMode(fullId);

    // Remove the game mode from the service registered game modes map
    auto it = s_gameModesByMod.find(ctx->mod->metadata.id);
    if (it != s_gameModesByMod.end()) {
        std::erase(it->second, fullId);
    }
    return MOD_OK;
}

ModResult is_active(ModContext* ctx, const char* gameModeId, bool* out_active) {
    *out_active =
        gamemode::getGameModeManager().isCurrentGameMode(get_mod_game_mode_id(ctx, gameModeId));
    return MOD_OK;
}

}  // namespace dusk::mods::svc::game_mode_impl

namespace dusk::mods::svc {
namespace {

constexpr GameModeService s_gamemodeService{
    .header = SERVICE_HEADER(GameModeService, GAMEMODE_SERVICE_MAJOR, GAMEMODE_SERVICE_MINOR),
    .register_game_mode = game_mode_impl::register_game_mode,
    .unregister_game_mode = game_mode_impl::unregister_game_mode,
    .is_active = game_mode_impl::is_active,
};

}  // namespace

constinit const ServiceModule g_gamemodeModule{
    .id = GAMEMODE_SERVICE_ID,
    .majorVersion = GAMEMODE_SERVICE_MAJOR,
    .minorVersion = GAMEMODE_SERVICE_MINOR,
    .service = &s_gamemodeService,
    .modDeactivating = game_mode_impl::game_mode_remove_mod,
};

}  // namespace dusk::mods::svc
