## TODO: GLOB is not a good way to collect files. Use explicit file list instead

cmake_minimum_required(VERSION 3.5)

add_library(microlite INTERFACE)

if(IDF_TARGET STREQUAL "esp32s3")
    set(MICROLITE_PLATFORM "ESP32S3")
endif()

if (MICROLITE_PLATFORM STREQUAL "ESP32" OR MICROLITE_PLATFORM STREQUAL "ESP32S3")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

endif()

set(tflite_dir "${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite")
set(tfmicro_dir "${tflite_dir}/micro")
set(tfmicro_frontend_dir "${tflite_dir}/experimental/microfrontend/lib")
set(tfmicro_kernels_dir "${tfmicro_dir}/kernels")

file(GLOB srcs_micro
        "${tfmicro_dir}/*.cc"
        "${tfmicro_dir}/*.c")

file(GLOB src_micro_frontend
        "${tfmicro_frontend_dir}/*.c"
        "${tfmicro_frontend_dir}/*.cc")

file(GLOB srcs_tflite_bridge
        "${tfmicro_dir}/tflite_bridge/*.c"
        "${tfmicro_dir}/tflite_bridge/*.cc")

file(GLOB srcs_kernels
        "${tfmicro_kernels_dir}/*.c"
        "${tfmicro_kernels_dir}/*.cc")

# remove sources which will be provided by esp_nn
list(REMOVE_ITEM srcs_kernels
        "${tfmicro_kernels_dir}/add.cc"
        "${tfmicro_kernels_dir}/conv.cc"
        "${tfmicro_kernels_dir}/depthwise_conv.cc"
        "${tfmicro_kernels_dir}/fully_connected.cc"
        "${tfmicro_kernels_dir}/mul.cc"
        "${tfmicro_kernels_dir}/pooling.cc"
        "${tfmicro_kernels_dir}/softmax.cc")

FILE(GLOB esp_nn_kernels
        "${tfmicro_kernels_dir}/esp_nn/*.cc")

set(priv_req esp-nn)

# include component requirements which were introduced after IDF version 4.1
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER "4.1")
    list(APPEND priv_req esp_timer driver)
endif()

idf_component_register(
        SRCS "${lib_srcs}"
        INCLUDE_DIRS "." "third_party/gemmlowp"
        "third_party/flatbuffers/include"
        "third_party/ruy"
        "third_party/kissfft"
        REQUIRES ${pub_req}
        PRIV_REQUIRES ${priv_req})

# Reduce the level of paranoia to be able to compile TF sources
target_compile_options(${COMPONENT_LIB} PRIVATE
        -Wno-error=attributes
        -Wno-maybe-uninitialized
        -Wno-missing-field-initializers
        -Wno-error=sign-compare
        -Wno-error=double-promotion
        -Wno-type-limits)

if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
    target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=stringop-overread)
endif()

# enable ESP-NN optimizations by Espressif
target_compile_options(${COMPONENT_LIB} PRIVATE -DESP_NN)

set(common_flags -DTF_LITE_STATIC_MEMORY -DTF_LITE_DISABLE_X86_NEON -O3
        -Wstrict-aliasing -Wno-unused-parameter -Wall -Wextra -Wvla
        -Wsign-compare -Wdouble-promotion -Wswitch -Wunused-function
        -Wmissing-field-initializers -ffunction-sections -fdata-sections
        -Wshadow -Wunused-variable -fno-unwind-tables -fmessage-length=0)

target_compile_options(${COMPONENT_LIB} PRIVATE ${common_flags} -fmessage-length=0 -Wno-nonnull)
target_compile_options(${COMPONENT_LIB} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:
        ${common_flags} -std=c++11 -fno-rtti -fno-exceptions
        -fno-threadsafe-statics -Werror -Wno-return-type
        -Wno-strict-aliasing -std=gnu++14 >)

# avoid issue of compiler internal error caused in `depthwise_conv_flaot.h
set_source_files_properties(${tfmicro_kernels_dir}/esp_nn/depthwise_conv.cc
        PROPERTIES COMPILE_FLAGS -O2)

target_compile_options(${COMPONENT_LIB} INTERFACE -DTF_LITE_STATIC_MEMORY)
target_link_libraries(${COMPONENT_LIB} PRIVATE -lm)


target_sources(microlite INTERFACE
    "${srcs_micro}"
    "${srcs_kernels}"
    "${srcs_tflite_bridge}"
    "${esp_nn_kernels}"
    "${src_micro_frontend}"
    "${tflite_dir}/kernels/kernel_util.cc"
    "${tflite_dir}/micro/memory_planner/greedy_memory_planner.cc"
    "${tflite_dir}/micro/memory_planner/linear_memory_planner.cc"
    "${tflite_dir}/micro/arena_allocator/non_persistent_arena_buffer_allocator.cc"
    "${tflite_dir}/micro/arena_allocator/persistent_arena_buffer_allocator.cc"
    "${tflite_dir}/micro/arena_allocator/recording_single_arena_buffer_allocator.cc"
    "${tflite_dir}/micro/arena_allocator/single_arena_buffer_allocator.cc"
    "${tflite_dir}/core/c/common.cc"
    "${tflite_dir}/core/api/error_reporter.cc"
    "${tflite_dir}/core/api/flatbuffer_conversions.cc"
    "${tflite_dir}/core/api/op_resolver.cc"
    "${tflite_dir}/core/api/tensor_utils.cc"
    "${tflite_dir}/kernels/internal/common.cc"
    "${tflite_dir}/kernels/internal/quantization_util.cc"
    "${tflite_dir}/kernels/internal/portable_tensor_utils.cc"
    "${tflite_dir}/kernels/internal/tensor_utils.cc"
    "${tflite_dir}/kernels/internal/tensor_ctypes.cc"
    "${tflite_dir}/kernels/internal/reference/portable_tensor_utils.cc"
    "${tflite_dir}/kernels/internal/reference/comparisons.cc"
    "${tflite_dir}/schema/schema_utils.cc"
)

target_include_directories(microlite INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/tflm
    ${CMAKE_CURRENT_LIST_DIR}/tflm/third_party/kissfft
    ${CMAKE_CURRENT_LIST_DIR}/tflm/third_party/kissfft/tools
    ${CMAKE_CURRENT_LIST_DIR}/tflm/third_party/flatbuffers/include
    ${CMAKE_CURRENT_LIST_DIR}/tflm/third_party/gemmlowp
    ${CMAKE_CURRENT_LIST_DIR}/tflm/third_party/ruy
)

target_compile_definitions(microlite INTERFACE
    MODULE_MICROLITE_ENABLED=1
    TF_LITE_STATIC_MEMORY=1
    TF_LITE_MCU_DEBUG_LOG
    NDEBUG
)

target_compile_options(microlite INTERFACE
    -Wno-error
    -Wno-error=float-conversion
    -Wno-error=nonnull
    -Wno-error=double-promotion
    -Wno-error=pointer-arith
    -Wno-error=unused-const-variable
    -Wno-error=sign-compare
    -fno-rtti
    -fno-exceptions
    -O3
    -Wno-error=maybe-uninitialized
)


target_link_libraries(usermod INTERFACE microlite)
