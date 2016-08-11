/*
 * ty, a collection of GUI and command-line tools to manage Teensy devices
 *
 * Distributed under the MIT license (see LICENSE.txt or http://opensource.org/licenses/MIT)
 * Copyright (c) 2015 Niels Martignène <niels.martignene@gmail.com>
 */

#include "ty/firmware.h"
#include "ty/task.h"
#include "main.h"

static int upload_flags = 0;
static const char *firmware_format = NULL;

static void print_upload_usage(FILE *f)
{
    fprintf(f, "usage: %s upload [options] <firmwares>\n\n", executable_name);

    print_common_options(f);
    fprintf(f, "\n");

    fprintf(f, "Upload options:\n"
               "   -w, --wait               Wait for the bootloader instead of rebooting\n"
               "       --nocheck            Force upload even if the board is not compatible\n"
               "       --noreset            Do not reset the device once the upload is finished\n"
               "   -f, --format <format>    Firmware file format (autodetected by default)\n\n"
               "You can pass multiple firmwares, and the first compatible one will be used.\n");

    fprintf(f, "Supported firmware formats: ");
    for (const ty_firmware_format *format = ty_firmware_formats; format->name; format++)
        fprintf(f, "%s%s", format != ty_firmware_formats ? ", " : "", format->name);
    fprintf(f, ".\n");
}

int upload(int argc, char *argv[])
{
    ty_optline_context optl;
    char *opt;
    ty_board *board = NULL;
    ty_firmware *fws[TY_UPLOAD_MAX_FIRMWARES];
    unsigned int fws_count;
    ty_task *task = NULL;
    int r;

    ty_optline_init_argv(&optl, argc, argv);
    while ((opt = ty_optline_next_option(&optl))) {
        if (strcmp(opt, "--help") == 0) {
            print_upload_usage(stdout);
            return EXIT_SUCCESS;
        } else if (strcmp(opt, "--wait") == 0 || strcmp(opt, "-w") == 0) {
            upload_flags |= TY_UPLOAD_WAIT;
        } else if (strcmp(opt, "--nocheck") == 0) {
            upload_flags |= TY_UPLOAD_NOCHECK;
        } else if (strcmp(opt, "--noreset") == 0) {
            upload_flags |= TY_UPLOAD_NORESET;
        } else if (strcmp(opt, "--format") == 0 || strcmp(opt, "-f") == 0) {
            firmware_format = ty_optline_get_value(&optl);
            if (!firmware_format) {
                ty_log(TY_LOG_ERROR, "Option '--format' takes an argument");
                print_upload_usage(stderr);
                return EXIT_FAILURE;
            }
        } else if (!parse_common_option(&optl, opt)) {
            print_upload_usage(stderr);
            return EXIT_FAILURE;
        }
    }

    fws_count = 0;
    while ((opt = ty_optline_consume_non_option(&optl))) {
        if (fws_count >= TY_COUNTOF(fws)) {
            ty_log(TY_LOG_WARNING, "Too many firmwares, considering only %zu files", TY_COUNTOF(fws));
            break;
        }

        r = ty_firmware_load(opt, firmware_format, &fws[fws_count]);
        if (!r)
            fws_count++;
    }
    if (!fws_count) {
        ty_log(TY_LOG_ERROR, "Missing valid firmware filename");
        print_upload_usage(stderr);
        return EXIT_FAILURE;
    }

    r = get_board(&board);
    if (r < 0)
        goto cleanup;

    r = ty_upload(board, fws, fws_count, upload_flags, &task);
    for (unsigned int i = 0; i < fws_count; i++)
        ty_firmware_unref(fws[i]);
    if (r < 0)
        goto cleanup;

    r = ty_task_join(task);

cleanup:
    ty_task_unref(task);
    ty_board_unref(board);
    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}