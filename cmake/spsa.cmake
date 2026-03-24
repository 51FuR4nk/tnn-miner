if(USE_ASTRO_SPSA)
  set(SPSA_OS_PREFIX "linux")
  if(WIN32)
    set(SPSA_OS_PREFIX "win")
  endif()
  set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} -flto -DUSE_ASTRO_SPSA=ON")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -DUSE_ASTRO_SPSA=ON")
  #set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} -DUSE_ASTRO_SPSA=ON")
  #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DUSE_ASTRO_SPSA=ON")

  if(EXISTS ${PROJECT_SOURCE_DIR}/lib/astrospsa)
    set(SPSA_LIB_DIR ${PROJECT_SOURCE_DIR}/lib/astrospsa)
  else()
    include(FetchContent)
    ## Fetch the static library
    FetchContent_Declare(
        astrospsa
        GIT_REPOSITORY https://gitlab.com/Tritonn204/astro-spsa.git
        GIT_TAG        830e33e67cc861b1086915031fecc2e8058f6d00
        # GIT_REPOSITORY https://gitlab.com/dirkerdero/astro-spsa-dirker.git
        # GIT_TAG        1a2acdd2ed187a9b565c33028dba4052a89f875b
    )
    FetchContent_MakeAvailable(astrospsa)
    set(SPSA_LIB_DIR ${astrospsa_SOURCE_DIR})
  endif()
  cmake_print_variables(SPSA_LIB_DIR)
  include_directories(${SPSA_LIB_DIR})

  set(SPSA_ARCH_CANDIDATES "${CPU_ARCHTARGET}")

  if(TARGET_ARCH STREQUAL "amd64")
    if(CPU_ARCHTARGET STREQUAL "goldmont")
      list(APPEND SPSA_ARCH_CANDIDATES "x86-64-v2")
    endif()
  endif()

  list(REMOVE_DUPLICATES SPSA_ARCH_CANDIDATES)

  foreach(SPSA_ARCH_CANDIDATE IN LISTS SPSA_ARCH_CANDIDATES)
    set(SPSA_CANDIDATE_PATH "${SPSA_LIB_DIR}/libastroSPSA_${SPSA_OS_PREFIX}_${TARGET_ARCH}${CMAKE_LIB_SUFFIX}_${SPSA_ARCH_CANDIDATE}.a")
    if(EXISTS "${SPSA_CANDIDATE_PATH}")
      set(SPSA_FULL_LIB_PATH "${SPSA_CANDIDATE_PATH}")
      break()
    endif()

    set(SPSA_CANDIDATE_PATH "${SPSA_LIB_DIR}/libastroSPSA_${SPSA_OS_PREFIX}_${TARGET_ARCH}_${SPSA_ARCH_CANDIDATE}.a")
    if(EXISTS "${SPSA_CANDIDATE_PATH}")
      set(SPSA_FULL_LIB_PATH "${SPSA_CANDIDATE_PATH}")
      break()
    endif()
  endforeach()

  if(NOT SPSA_FULL_LIB_PATH AND EXISTS "${SPSA_LIB_DIR}/libastroSPSA_${SPSA_OS_PREFIX}_${TARGET_ARCH}${CMAKE_LIB_SUFFIX}.a")
    set(SPSA_FULL_LIB_PATH "${SPSA_LIB_DIR}/libastroSPSA_${SPSA_OS_PREFIX}_${TARGET_ARCH}${CMAKE_LIB_SUFFIX}.a")
  elseif(NOT SPSA_FULL_LIB_PATH AND EXISTS "${SPSA_LIB_DIR}/libastroSPSA_${SPSA_OS_PREFIX}_${TARGET_ARCH}.a")
    set(SPSA_FULL_LIB_PATH "${SPSA_LIB_DIR}/libastroSPSA_${SPSA_OS_PREFIX}_${TARGET_ARCH}.a")
  elseif(NOT SPSA_FULL_LIB_PATH AND EXISTS "${SPSA_LIB_DIR}/libastroSPSA_${SPSA_OS_PREFIX}.a")
    set(SPSA_FULL_LIB_PATH "${SPSA_LIB_DIR}/libastroSPSA_${SPSA_OS_PREFIX}.a")
  endif()

  if(SPSA_FULL_LIB_PATH)
    message(STATUS "Using SPSA library: ${SPSA_FULL_LIB_PATH}")
  else()
    message(FATAL_ERROR "SPSA lib was not found: ${SPSA_LIB_DIR}")
  endif()
endif()
