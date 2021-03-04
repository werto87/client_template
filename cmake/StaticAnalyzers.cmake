option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF) #does not work with c++20 features
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF) #does not work with c++20 features and precompiled headers together
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)

if(ENABLE_CPPCHECK)
      message("check enabled")
  find_program(CPPCHECK cppcheck)
  if(CPPCHECK)
    message("cpp check found")
        message(${CMAKE_CURRENT_SOURCE_DIR})
    set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK}
        --suppress=missingInclude
        --enable=all
        --inline-suppr
        --inconclusive
	--output-file=cppcheck_result.txt
	--quiet
	--template="{file}::{line}::{message}"
	-i
        ${CMAKE_SOURCE_DIR}/imgui/lib)
  else()
    message(SEND_ERROR "cppcheck requested but executable not found")
  endif()
endif()

if(ENABLE_CLANG_TIDY)
        message("clang-tidy")
  find_program(CLANGTIDY clang-tidy)
  if(CLANGTIDY)
          message("found clang-tidy")
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} -extra-arg=-Wno-unknown-warning-option)
  else()
    message(SEND_ERROR "clang-tidy requested but executable not found")
  
  endif()
else()
    message("clang tidy false")
endif()

if(ENABLE_INCLUDE_WHAT_YOU_USE)
  find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
  if(INCLUDE_WHAT_YOU_USE)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
  else()
    message(SEND_ERROR "include-what-you-use requested but executable not found")
  endif()
endif()
