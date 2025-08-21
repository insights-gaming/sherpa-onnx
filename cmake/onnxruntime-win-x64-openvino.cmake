# Copyright (c)  2022-2023  Xiaomi Corporation
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_VS_PLATFORM_NAME: ${CMAKE_VS_PLATFORM_NAME}")

if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
  message(FATAL_ERROR "This file is for Windows only. Given: ${CMAKE_SYSTEM_NAME}")
endif()

if(NOT (CMAKE_VS_PLATFORM_NAME STREQUAL X64 OR CMAKE_VS_PLATFORM_NAME STREQUAL x64))
  message(FATAL_ERROR "This file is for Windows x64 only. Given: ${CMAKE_VS_PLATFORM_NAME}")
endif()

# if(NOT BUILD_SHARED_LIBS)
#   message(FATAL_ERROR "This file is for building shared libraries. BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
# endif()

if(NOT SHERPA_ONNX_ENABLE_OPENVINO)
  message(FATAL_ERROR "This file is for OpenVINO. Given SHERPA_ONNX_ENABLE_OPENVINO: ${SHERPA_ONNX_ENABLE_OPENVINO}")
endif()

if(location_onnxruntime_header_dir AND location_onnxruntime_lib)
    message("Use preinstall onnxruntime with openvino: ${location_onnxruntime_lib}")
else()

    set(onnxruntime_URL  "https://github.com/intel/onnxruntime/releases/download/v5.2/nuget-local-artifacts.zip")
    set(onnxruntime_HASH "SHA256=9870fae76d3cd2a23e2e2a19fb3f6a2ec1898f3e27e19dc8f05fd04a9289e7de")

    # If you don't have access to the Internet,
    # please download onnxruntime to one of the following locations.
    # You can add more if you want.
    set(possible_file_locations
        $ENV{HOME}/Downloads/Microsoft.ML.OnnxRuntime.OpenVino.1.17.1-dev-20240313-0625-e39929291.nupkg
        ${PROJECT_SOURCE_DIR}/Microsoft.ML.OnnxRuntime.OpenVino.1.17.1-dev-20240313-0625-e39929291.nupkg
        ${PROJECT_BINARY_DIR}/Microsoft.ML.OnnxRuntime.OpenVino.1.17.1-dev-20240313-0625-e39929291.nupkg
        /tmp/Microsoft.ML.OnnxRuntime.OpenVino.1.17.1-dev-20240313-0625-e39929291.nupkg
    )

    foreach(f IN LISTS possible_file_locations)
      if(EXISTS ${f})
        set(onnxruntime_nupkg_PATH "${f}")
        file(TO_CMAKE_PATH "${onnxruntime_nupkg_PATH}" onnxruntime_nupkg_PATH)
        message(STATUS "Found local downloaded onnxruntime openvino nupkg: ${onnxruntime_nupkg_PATH}")
        break()
      endif()
    endforeach()
    
    FetchContent_Declare(onnxruntime
      URL      ${onnxruntime_URL}
      URL_HASH ${onnxruntime_HASH}
    )

    FetchContent_GetProperties(onnxruntime)
    if(NOT onnxruntime_POPULATED)
      message(STATUS "Downloading onnxruntime from ${onnxruntime_URL}")
      FetchContent_Populate(onnxruntime)
    endif()
    message(STATUS "onnxruntime is downloaded to ${onnxruntime_SOURCE_DIR}")
    
    file(ARCHIVE_EXTRACT
      INPUT "${onnxruntime_SOURCE_DIR}/Microsoft.ML.OnnxRuntime.OpenVino.1.17.1-dev-20240313-0625-e39929291.nupkg"
      DESTINATION ${onnxruntime_BINARY_DIR}
    )

    message(STATUS "onnxruntime openvino is extracted to ${onnxruntime_BINARY_DIR}")

    find_library(location_onnxruntime onnxruntime
      PATHS
      "${onnxruntime_BINARY_DIR}/runtimes/win-x64/native"
      NO_CMAKE_SYSTEM_PATH
    )

    message(STATUS "location_onnxruntime: ${location_onnxruntime}")

    add_library(onnxruntime SHARED IMPORTED)

    set_target_properties(onnxruntime PROPERTIES
      IMPORTED_LOCATION ${location_onnxruntime}
      INTERFACE_INCLUDE_DIRECTORIES "${onnxruntime_BINARY_DIR}/build/native/include"
    )

    set_property(TARGET onnxruntime
      PROPERTY
        IMPORTED_IMPLIB "${onnxruntime_BINARY_DIR}/runtimes/win-x64/native/onnxruntime.lib"
    )

    file(COPY ${onnxruntime_BINARY_DIR}/runtimes/win-x64/native/onnxruntime.dll
      DESTINATION
        ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}
    )

    file(GLOB onnxruntime_lib_files "${onnxruntime_BINARY_DIR}/runtimes/win-x64/native/onnxruntime.*")

    message(STATUS "onnxruntime lib files: ${onnxruntime_lib_files}")

    if(SHERPA_ONNX_ENABLE_PYTHON)
      install(FILES ${onnxruntime_lib_files} DESTINATION ..)
    else()
      install(FILES ${onnxruntime_lib_files} DESTINATION lib)
    endif()

    install(FILES ${onnxruntime_lib_files} DESTINATION bin)

endif()

