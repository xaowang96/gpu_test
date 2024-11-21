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

#include "vg_lite_test_context.h"
#include "../gpu_assert.h"
#include "../gpu_context.h"
#include "../gpu_recorder.h"
#include "../gpu_screenshot.h"
#include "../gpu_tick.h"
#include "vg_lite_test_path.h"
#include "vg_lite_test_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct vg_lite_test_context_s {
    struct gpu_test_context_s* gpu_ctx;
    vg_lite_buffer_t target_buffer;
    vg_lite_buffer_t src_buffer;
    struct vg_lite_test_path_s* path;
    vg_lite_matrix_t matrix;
    uint32_t prepare_tick;
    uint32_t finish_tick;
    char remark_text[128];
    void* user_data;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void vg_lite_test_context_cleanup(struct vg_lite_test_context_s* ctx);
static void vg_lite_test_context_record(struct vg_lite_test_context_s* ctx, const struct vg_lite_test_item_s* item, vg_lite_error_t error);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

struct vg_lite_test_context_s* vg_lite_test_context_create(struct gpu_test_context_s* gpu_ctx)
{
    struct vg_lite_test_context_s* ctx = malloc(sizeof(struct vg_lite_test_context_s));
    GPU_ASSERT_NULL(ctx);
    memset(ctx, 0, sizeof(struct vg_lite_test_context_s));
    ctx->gpu_ctx = gpu_ctx;
    vg_lite_identity(&ctx->matrix);
    vg_lite_scale(
        gpu_ctx->param.img_width / (float)GPU_TEST_DESIGN_WIDTH,
        gpu_ctx->param.img_height / (float)GPU_TEST_DESIGN_HEIGHT,
        &ctx->matrix);

    GPU_ASSERT_NULL(gpu_ctx);
    vg_lite_test_buffer_alloc(
        &ctx->target_buffer,
        ctx->gpu_ctx->param.img_width,
        ctx->gpu_ctx->param.img_height,
        VG_LITE_BGRA8888,
        VG_LITE_TEST_STRIDE_AUTO);

    if (ctx->gpu_ctx->recorder) {
        gpu_recorder_write_string(ctx->gpu_ctx->recorder,
            "Testcase,"
            "Instructions,"
            "Target Format,Source Format,"
            "Target Address,Source Address,"
            "Target Area,Source Area,"
            "Prepare Time(ms),Finish Time(ms),"
            "Result,"
            "Remark"
            "\n");
    }

    return ctx;
}

void vg_lite_test_context_destroy(struct vg_lite_test_context_s* ctx)
{
    GPU_ASSERT_NULL(ctx);
    vg_lite_test_buffer_free(&ctx->target_buffer);

    /* Check if the context is clean */
    GPU_ASSERT(ctx->path == NULL);
    GPU_ASSERT(ctx->src_buffer.memory == NULL);

    memset(ctx, 0, sizeof(struct vg_lite_test_context_s));
    free(ctx);
}

bool vg_lite_test_context_run_item(struct vg_lite_test_context_s* ctx, const struct vg_lite_test_item_s* item)
{
    if (item->feature != gcFEATURE_BIT_VG_NONE && !vg_lite_query_feature(item->feature)) {
        GPU_LOG_WARN("Skipping test case: %s (feature %s not supported)", item->name, vg_lite_test_feature_string(item->feature));
        vg_lite_test_context_record(ctx, item, VG_LITE_NOT_SUPPORT);
        return true;
    }

    GPU_LOG_INFO("Running test case: %s", item->name);

    uint32_t start_tick = gpu_tick_get();
    vg_lite_error_t error = item->on_setup(ctx);
    ctx->prepare_tick = gpu_tick_elaps(start_tick);

    if (error == VG_LITE_SUCCESS) {
        start_tick = gpu_tick_get();
        error = vg_lite_finish();
        ctx->finish_tick = gpu_tick_elaps(start_tick);
    }

    if (item->on_teardown) {
        item->on_teardown(ctx);
    }

    if (error == VG_LITE_SUCCESS) {
        GPU_LOG_INFO("Test case '%s' PASS", item->name);
    } else {
        GPU_LOG_ERROR("Test case '%s' FAILED: %d (%s)", item->name, error, vg_lite_test_error_string(error));
    }

    vg_lite_test_context_record(ctx, item, error);

    if (ctx->gpu_ctx->param.screenshot_en) {
        struct gpu_buffer_s screenshot_buffer;
        vg_lite_test_vg_buffer_to_gpu_buffer(&screenshot_buffer, &ctx->target_buffer);
        gpu_screenshot(ctx->gpu_ctx->param.output_dir, item->name, &screenshot_buffer);
    }

    vg_lite_test_context_cleanup(ctx);

    return error == VG_LITE_SUCCESS;
}

vg_lite_buffer_t* vg_lite_test_context_get_target_buffer(struct vg_lite_test_context_s* ctx)
{
    GPU_ASSERT_NULL(ctx);
    return &ctx->target_buffer;
}

vg_lite_buffer_t* vg_lite_test_context_get_src_buffer(struct vg_lite_test_context_s* ctx)
{
    GPU_ASSERT_NULL(ctx);
    return &ctx->src_buffer;
}

void vg_lite_test_context_set_transform(struct vg_lite_test_context_s* ctx, const vg_lite_matrix_t* matrix)
{
    GPU_ASSERT_NULL(ctx);
    ctx->matrix = *matrix;
}

void vg_lite_test_context_get_transform(struct vg_lite_test_context_s* ctx, vg_lite_matrix_t* matrix)
{
    GPU_ASSERT_NULL(ctx);
    *matrix = ctx->matrix;
}

struct vg_lite_test_path_s* vg_lite_test_context_init_path(struct vg_lite_test_context_s* ctx, vg_lite_format_t format)
{
    GPU_ASSERT_NULL(ctx);
    /* Check if the path is already created */
    GPU_ASSERT(ctx->path == NULL);

    ctx->path = vg_lite_test_path_create(format);
    return ctx->path;
}

struct vg_lite_test_path_s* vg_lite_test_context_get_path(struct vg_lite_test_context_s* ctx)
{
    GPU_ASSERT_NULL(ctx);
    GPU_ASSERT_NULL(ctx->path);
    return ctx->path;
}

void vg_lite_test_context_set_user_data(struct vg_lite_test_context_s* ctx, void* user_data)
{
    GPU_ASSERT_NULL(ctx);
    ctx->user_data = user_data;
}

void* vg_lite_test_context_get_user_data(struct vg_lite_test_context_s* ctx)
{
    GPU_ASSERT_NULL(ctx);
    return ctx->user_data;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void vg_lite_test_context_cleanup(struct vg_lite_test_context_s* ctx)
{
    GPU_ASSERT_NULL(ctx);

    /* Clear the target buffer */
    size_t target_size = ctx->target_buffer.stride * ctx->target_buffer.height;
    memset(ctx->target_buffer.memory, 0, target_size);

    ctx->remark_text[0] = '\0';
    ctx->prepare_tick = 0;
    ctx->finish_tick = 0;
    ctx->user_data = NULL;

    if (ctx->src_buffer.memory) {
        vg_lite_test_buffer_free(&ctx->src_buffer);
    }

    if (ctx->path) {
        vg_lite_test_path_destroy(ctx->path);
        ctx->path = NULL;
    }
}

static void vg_lite_test_context_record(struct vg_lite_test_context_s* ctx, const struct vg_lite_test_item_s* item, vg_lite_error_t error)
{
    GPU_ASSERT_NULL(ctx);
    GPU_ASSERT_NULL(item);

    if (!ctx->gpu_ctx->recorder) {
        return;
    }

    char result[256];
    snprintf(result, sizeof(result),
        "%s,"
        "%s,"
        "%s,%s,"
        "%p,%p,"
        "%dx%d,%dx%d,"
        "%0.3f,"
        "%0.3f,"
        "%s,"
        "%s\n",
        item->name,
        item->instructions,
        vg_lite_test_buffer_format_string(ctx->target_buffer.format),
        vg_lite_test_buffer_format_string(ctx->src_buffer.format),
        ctx->target_buffer.memory,
        ctx->src_buffer.memory,
        (int)ctx->target_buffer.width,
        (int)ctx->target_buffer.height,
        (int)ctx->src_buffer.width,
        (int)ctx->src_buffer.height,
        ctx->prepare_tick / 1000.0f,
        ctx->finish_tick / 1000.0f,
        vg_lite_test_error_string(error),
        ctx->remark_text);

    gpu_recorder_write_string(ctx->gpu_ctx->recorder, result);
}
