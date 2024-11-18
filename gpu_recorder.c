/*
 * MIT License
 * Copyright (c) 2023 - 2024 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*********************
 *      INCLUDES
 *********************/

#include "gpu_recorder.h"
#include "gpu_test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct gpu_recorder_s {
    struct gpu_test_context_s* ctx;
    int fd;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

struct gpu_recorder_s* gpu_recorder_create(struct gpu_test_context_s* ctx, const char* name)
{
    struct gpu_recorder_s* recorder;
    char path[256];
    char filename[64];
    gpu_get_localtime_str(filename, sizeof(filename));

    snprintf(path, sizeof(path), "%s/report_%s_%s.csv",
        ctx->param.output_dir, name, filename);

    int fd = open(path, O_CREAT | O_WRONLY | O_CLOEXEC, 0666);
    if (fd < 0) {
        GPU_LOG_ERROR("open %s failed: %d", path, errno);
        return NULL;
    }

    recorder = calloc(1, sizeof(struct gpu_recorder_s));
    GPU_ASSERT_NULL(recorder);
    recorder->fd = fd;
    recorder->ctx = ctx;
    GPU_LOG_INFO("recorder file: %s created, fd = %d", path, fd);
    return recorder;
}

void gpu_recorder_delete(struct gpu_recorder_s* recorder)
{
    GPU_ASSERT_NULL(recorder);
    close(recorder->fd);
    memset(recorder, 0, sizeof(struct gpu_recorder_s));
    free(recorder);
    GPU_LOG_INFO("recorder deleted");
}

int gpu_recorder_write_string(struct gpu_recorder_s* recorder, const char* str)
{
    GPU_ASSERT_NULL(recorder);
    GPU_ASSERT_NULL(str);
    size_t len = strlen(str);
    size_t written = 0;

    while (written < len) {
        ssize_t ret = write(recorder->fd, str + written, len - written);

        if (ret < 0) {
            GPU_LOG_ERROR("write failed: %d", errno);
            return -1;
        }

        written += ret;
    }

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
