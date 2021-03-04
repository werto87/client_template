set(TEST_TARGET_NAME _test)

get_target_property(TARGET_SOURCES ${TARGET_NAME} SOURCES)
get_target_property(TARGET_LIBS ${TARGET_NAME} LINK_LIBRARIES)
add_executable(${TEST_TARGET_NAME} 
"main.cxx"
"test.cxx"
"constant.hxx"
${TARGET_SOURCES}
 )

 if (CMAKE_BINARY_DIR)
 target_compile_definitions(${TEST_TARGET_NAME} PRIVATE PATH_TO_BINARY="${CMAKE_BINARY_DIR}")
 endif()

target_link_libraries(${TEST_TARGET_NAME}
${PROJECT_WARNINGS}
${PROJECT_OPTIONS}
 CONAN_PKG::catch2
${TARGET_LIBS}
)
target_include_directories(${TEST_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

add_test(NAME ${TEST_TARGET_NAME}
         COMMAND ${TEST_TARGET_NAME} -r junit) 
