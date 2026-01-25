#==============================================================================
# VneUtils Tests
#==============================================================================

# Use external googletest submodule
if(EXISTS "${CMAKE_SOURCE_DIR}/external/googletest/CMakeLists.txt")
    message(STATUS "Using Google Test from external/googletest")
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest ${CMAKE_BINARY_DIR}/external/googletest)
    include(GoogleTest)
    set(GTEST_LIBRARIES gtest_main gmock)
else()
    # Fallback to FetchContent
    message(STATUS "Fetching Google Test")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
    set(GTEST_LIBRARIES gtest_main gmock)
endif()

# Test executable
add_executable(TestVneUtils
    main.cpp
    command_line_parser_test.cpp
)

target_include_directories(TestVneUtils PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(TestVneUtils PRIVATE
    vne::utils
    ${GTEST_LIBRARIES}
)

# Register tests with CTest
gtest_discover_tests(TestVneUtils
    TEST_PREFIX "vneutils."
)

# Set output directory
set_target_properties(TestVneUtils PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
