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

#ifndef VG_LITE_TEST_UTILS_H
#define VG_LITE_TEST_UTILS_H

/*********************
 *      INCLUDES
 *********************/

#include "../gpu_buffer.h"
#include "../gpu_log.h"
#include <vg_lite.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

#define VG_LITE_TEST_CHECK_ERROR(func)                                  \
    do {                                                                \
        vg_lite_error_t error = func;                                   \
        if (error != VG_LITE_SUCCESS) {                                 \
            GPU_LOG_ERROR("Execute '" #func "' error: %d", (int)error); \
            vg_lite_test_error_dump_info(error);                        \
        }                                                               \
    } while (0)

#define VG_LITE_TEST_STRIDE_AUTO 0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Dump the information of the VG Lite library.
 */
void vg_lite_test_dump_info(void);

/**
 * @brief Get the error string.
 * @param error The error code.
 * @return The error string.
 */
const char* vg_lite_test_error_string(vg_lite_error_t error);

/**
 * @brief Dump error information.
 * @param error The error code.
 */
void vg_lite_test_error_dump_info(vg_lite_error_t error);

/**
 * @brief Allocate a GPU buffer for VG Lite.
 * @param buffer The VG Lite buffer to be allocated.
 * @param width The width of the buffer.
 * @param height The height of the buffer.
 * @param format The format of the buffer.
 * @param stride The stride of the buffer. If it is VG_LITE_TEST_STRIDE_AUTO, the stride will be calculated automatically.
 */
void vg_lite_test_buffer_alloc(vg_lite_buffer_t* buffer, uint32_t width, uint32_t height, vg_lite_buffer_format_t format, uint32_t stride);

/**
 * @brief Free a GPU buffer for VG Lite.
 * @param buffer The VG Lite buffer to be freed.
 */
void vg_lite_test_buffer_free(vg_lite_buffer_t* buffer);

/**
 * @brief Copy a VG Lite buffer to a GPU buffer.
 * @param gpu_buffer The GPU buffer to be copied.
 * @param vg_buffer The VG Lite buffer to be copied.
 */
void vg_lite_test_vg_buffer_to_gpu_buffer(struct gpu_buffer_s* gpu_buffer, const vg_lite_buffer_t* vg_buffer);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*VG_LITE_TEST_UTILS_H*/
