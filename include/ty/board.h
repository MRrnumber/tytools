/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TY_BOARD_H
#define TY_BOARD_H

#include "common.h"
#include "device.h"

TY_C_BEGIN

struct ty_firmware;

typedef struct ty_board_manager ty_board_manager;
typedef struct ty_board ty_board;

typedef struct ty_board_model ty_board_model;
typedef struct ty_board_mode ty_board_mode;

typedef enum ty_board_capability {
    TY_BOARD_CAPABILITY_IDENTIFY = 1,
    TY_BOARD_CAPABILITY_UPLOAD   = 2,
    TY_BOARD_CAPABILITY_RESET    = 4,
    TY_BOARD_CAPABILITY_SERIAL   = 8,
    TY_BOARD_CAPABILITY_REBOOT   = 16
} ty_board_capability;

typedef enum ty_board_state {
    TY_BOARD_STATE_DROPPED,
    TY_BOARD_STATE_CLOSED,
    TY_BOARD_STATE_ONLINE
} ty_board_state;

typedef enum ty_board_event {
    TY_BOARD_EVENT_ADDED,
    TY_BOARD_EVENT_CHANGED,
    TY_BOARD_EVENT_CLOSED,
    TY_BOARD_EVENT_DROPPED
} ty_board_event;

enum {
    TY_BOARD_UPLOAD_NOCHECK = 1
};

typedef int ty_board_manager_callback_func(ty_board *board, ty_board_event event, void *udata);
typedef int ty_board_manager_wait_func(ty_board_manager *manager, void *udata);

TY_PUBLIC extern const ty_board_mode *ty_board_modes[];
TY_PUBLIC extern const ty_board_model *ty_board_models[];

TY_PUBLIC int ty_board_manager_new(ty_board_manager **rmanager);
TY_PUBLIC void ty_board_manager_free(ty_board_manager *manager);

TY_PUBLIC void ty_board_manager_set_udata(ty_board_manager *manager, void *udata);
TY_PUBLIC void *ty_board_manager_get_udata(const ty_board_manager *manager);

TY_PUBLIC void ty_board_manager_get_descriptors(const ty_board_manager *manager, struct ty_descriptor_set *set, int id);

TY_PUBLIC int ty_board_manager_register_callback(ty_board_manager *manager, ty_board_manager_callback_func *f, void *udata);
TY_PUBLIC void ty_board_manager_deregister_callback(ty_board_manager *manager, int id);

TY_PUBLIC int ty_board_manager_refresh(ty_board_manager *manager);
TY_PUBLIC int ty_board_manager_wait(ty_board_manager *manager, ty_board_manager_wait_func *f, void *udata, int timeout);

TY_PUBLIC int ty_board_manager_list(ty_board_manager *manager, ty_board_manager_callback_func *f, void *udata);

TY_PUBLIC const ty_board_mode *ty_board_find_mode(const char *name);
TY_PUBLIC const ty_board_model *ty_board_find_model(const char *name);

TY_PUBLIC const char *ty_board_mode_get_name(const ty_board_mode *mode);
TY_PUBLIC const char *ty_board_mode_get_desc(const ty_board_mode *mode);

TY_PUBLIC const char *ty_board_model_get_name(const ty_board_model *model);
TY_PUBLIC const char *ty_board_model_get_mcu(const ty_board_model *model);
TY_PUBLIC const char *ty_board_model_get_desc(const ty_board_model *model);
TY_PUBLIC size_t ty_board_model_get_code_size(const ty_board_model *model);

TY_PUBLIC ty_board *ty_board_ref(ty_board *teensy);
TY_PUBLIC void ty_board_unref(ty_board *teensy);

TY_PUBLIC void ty_board_set_udata(ty_board *board, void *udata);
TY_PUBLIC void *ty_board_get_udata(const ty_board *board);

TY_PUBLIC ty_board_manager *ty_board_get_manager(const ty_board *board);

TY_PUBLIC ty_board_state ty_board_get_state(const ty_board *board);

TY_PUBLIC ty_device *ty_board_get_device(const ty_board *board);
static inline const char *ty_board_get_location(const ty_board *board)
{
    return ty_device_get_location(ty_board_get_device(board));
}
static inline const char *ty_board_get_path(const ty_board *board)
{
    return ty_device_get_path(ty_board_get_device(board));
}
TY_PUBLIC ty_handle *ty_board_get_handle(const ty_board *board);

TY_PUBLIC const ty_board_mode *ty_board_get_mode(const ty_board *board);
TY_PUBLIC const ty_board_model *ty_board_get_model(const ty_board *board);

TY_PUBLIC uint64_t ty_board_get_serial_number(const ty_board *board);

TY_PUBLIC uint32_t ty_board_get_capabilities(const ty_board *board);
static inline bool ty_board_has_capability(const ty_board *board, ty_board_capability cap)
{
    return ty_board_get_capabilities(board) & cap;
}

TY_PUBLIC int ty_board_wait_for(ty_board *board, ty_board_capability capability, int timeout);

TY_PUBLIC int ty_board_control_serial(ty_board *board, uint32_t rate, uint16_t flags);

TY_PUBLIC ssize_t ty_board_read_serial(ty_board *board, char *buf, size_t size);
TY_PUBLIC ssize_t ty_board_write_serial(ty_board *board, const char *buf, size_t size);

TY_PUBLIC int ty_board_upload(ty_board *board, struct ty_firmware *firmware, uint16_t flags);
TY_PUBLIC int ty_board_reset(ty_board *board);

TY_PUBLIC int ty_board_reboot(ty_board *board);

TY_PUBLIC const ty_board_model *ty_board_test_firmware(const struct ty_firmware *f);

TY_C_END

#endif
