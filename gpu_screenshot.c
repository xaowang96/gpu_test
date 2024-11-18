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

#include "gpu_screenshot.h"
#include "gpu_assert.h"
#include "gpu_buffer.h"
#include "gpu_log.h"
#include "gpu_utils.h"
#include <png.h>
#include <stdio.h>

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int save_buffer_to_file(const struct gpu_buffer_s* buffer, const char* path);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int gpu_screenshot(const char* dirpath, const char* name, const struct gpu_buffer_s* buffer)
{
    GPU_ASSERT_NULL(dirpath);
    GPU_ASSERT_NULL(name);
    GPU_ASSERT_NULL(buffer);

    int retval;
    char path[256];
    char time_str[64];

    GPU_LOG_INFO("Taking screenshot of '%s' ...", name);

    gpu_get_localtime_str(time_str, sizeof(time_str));
    snprintf(path, sizeof(path), "%s/screenshot_%s_%s.png", dirpath, name, time_str);

    retval = save_img_file(buffer, path);

    if (retval > 0) {
        GPU_LOG_INFO("Screenshot saved to %s", path);
    } else {
        GPU_LOG_ERROR("Failed to save screenshot: %d", retval);
    }

    return retval;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int save_img_file(struct gpu_test_context_s* ctx, const char* path)
{
    png_image image;
    int retval;

    /* Construct the PNG image structure. */

    memset(&image, 0, sizeof(image));

    image.version = PNG_IMAGE_VERSION;
    image.width = ctx->xres;
    image.height = ctx->yres;
    image.format = PNG_FORMAT_BGRA;

    /* Write the PNG image. */

    retval = png_image_write_to_file(&image, path, 0, ctx->fbmem, ctx->stride, NULL);

    return retval;
}
