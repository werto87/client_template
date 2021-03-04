# set(CMAKE_CXX_COMPILER "/usr/local/bin/clang++")
# set(CMAKE_C_COMPILER "/usr/local/bin/clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld -stdlib=libc++ -lc++abi -fcoroutines-ts -lc++experimental")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld -fcoroutines")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -v")
cmake_minimum_required(VERSION 3.17)
project(socketgame VERSION 1.0.0
  DESCRIPTION socketgame
  LANGUAGES CXX
  ) 
cmake_policy(SET CMP0015 NEW)




########## enable cache system
include(cmake/Cache.cmake)

#set(CMAKE_EXECUTABLE_SUFFIX ".html")
########## link against the libs to apply the configurations to the target
# compiler warnings
set(PROJECT_WARNINGS project_warning)
add_library(${PROJECT_WARNINGS} INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_WARNINGS})
# compiler options
set(PROJECT_OPTIONS project_options)


add_library(${PROJECT_OPTIONS} INTERFACE)
target_compile_features(${PROJECT_OPTIONS} INTERFACE cxx_std_20)
#target_compile_options(${PROJECT_OPTIONS} INTERFACE -ftime-trace)
target_precompile_headers(${PROJECT_OPTIONS} INTERFACE 
)


#  include(cmake/Sanitizers.cmake)
#  enable_sanitizers(${PROJECT_OPTIONS})

# allow for static analysis options
#include(cmake/StaticAnalyzers.cmake) 
 
######## conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


####### create executable
set(TARGET_SERVER_NAME server)
add_executable(${TARGET_SERVER_NAME})

set(TARGET_CLIENT_NAME client)
add_executable(${TARGET_CLIENT_NAME})


INCLUDE_DIRECTORIES(/home/walde/projects/convenienceFunctionForSoci/convenienceFunctionForSoci)
LINK_DIRECTORIES(/home/walde/projects/convenienceFunctionForSoci/build/lib)



######## add sub dir
add_subdirectory(src) 


######## add enable_testing
OPTION(BUILD_TESTS "enable tests" ON)
IF(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test) 
ENDIF(BUILD_TESTS)
####### include what you use
#find_program(iwyu_path NAMES include-what-you-use iwyu)
#set_property(TARGET ${TARGET_SERVER_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
# set_property(TARGET ${TARGET_CLIENT_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

####### link
target_include_directories(${TARGET_SERVER_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${TARGET_SERVER_NAME} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  

target_link_libraries(${TARGET_SERVER_NAME} PRIVATE ${PROJECT_WARNINGS} ${PROJECT_OPTIONS}
CONAN_PKG::confu_soci
CONAN_PKG::boost
CONAN_PKG::fmt
CONAN_PKG::magic_enum
CONAN_PKG::imgui-sfml
CONAN_PKG::soci
CONAN_PKG::sqlite3
)


target_include_directories(${TARGET_CLIENT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${TARGET_CLIENT_NAME} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  

target_link_libraries(${TARGET_CLIENT_NAME} PRIVATE ${PROJECT_WARNINGS} ${PROJECT_OPTIONS} 
CONAN_PKG::confu_soci
CONAN_PKG::boost
CONAN_PKG::fmt
CONAN_PKG::imgui-sfml
CONAN_PKG::magic_enum
CONAN_PKG::soci
CONAN_PKG::sqlite3
)



#print all variables used in cmake
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
