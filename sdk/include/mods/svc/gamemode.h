#pragma once

#include <mods/api.h>
#include <mods/svc/config.h>

#define GAMEMODE_SERVICE_ID "dev.twilitrealm.dusklight.gamemode"
#define GAMEMODE_SERVICE_MAJOR 1u
#define GAMEMODE_SERVICE_MINOR 0u

typedef struct {
    const char* gamemodeId;
    const char* fullName;
    const char saveName[32];        // Should be unique. GCI Filenames are limited to 31 characters
    void (*onActivatedFunction)();  // Called when the gamemode is selected
    void (*onDeactivatedFunction)();  // Called when the gamemode is deselected
    void (*onPlayFunction)();         // Called when play is pressed on the prelaunch menu
    void (*onSaveLoadedFunction)();   // Called whenever a savefile is loaded
    void (*onNewSaveFunction)();      // Called when a new save is created
    void (*onNewSaveSelectFunction)(bool* out_proceedToNameSelect,
        bool* out_returnToFileSelect);  // Set out_proceedToNameSelect to true once any UI flows are
                                        // completed
    void (*onGameResetFunction)();      // Called when the game is reset
    void (*onTickFunction)();           // Called on every tick
} GamemodeDesc;

typedef struct GamemodeService {
    ServiceHeader header;
    ModResult (*register_gamemode)(ModContext* ctx, const GamemodeDesc* desc);
    ModResult (*unregister_gamemode)(ModContext* ctx, const char* id);
    ModResult (*is_active)(ModContext* ctx, const char* gamemodeId, bool* out_active);
} GamemodeService;

#ifdef __cplusplus
#include "mods/service.hpp"

template <>
struct mods::ServiceTraits<GamemodeService> {
    static constexpr const char* id = GAMEMODE_SERVICE_ID;
    static constexpr uint16_t major_version = GAMEMODE_SERVICE_MAJOR;
    static constexpr uint16_t minor_version = GAMEMODE_SERVICE_MINOR;
};
#endif
