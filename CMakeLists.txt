cmake_minimum_required (VERSION 3.0)
project (MainProject)

add_executable(MainProject main/ctpl_stl.h main/csv.h main/csv.cpp main/testgraph.cpp main/stdafx.cpp main/stdafx.h main/SymSnap.cpp main/SymSnap.h main/targetver.h snap/snap-core/Snap.cpp snap/snap-core/Snap.h main/csv.cpp)

add_executable(Evaluation main/csv.h main/csv.cpp Evaluation/directedModularity.cpp main/stdafx.cpp main/stdafx.h main/SymSnap.cpp main/SymSnap.h main/targetver.h snap/snap-core/Snap.cpp)

add_executable(Refine main/stdafx.cpp main/stdafx.h main/targetver.h snap/snap-core/Snap.cpp datasetRefine/refinePapersById.cpp)

include_directories(snap/glib-core snap/snap-core snap/snap-adv snap/snap-exp clustering/lib Eigen spectra/include)

find_package(OpenMP)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if(OPENMP_FOUND)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
# Whether to enable SSE
set(SSE_FLAGS "")
option(FCL_USE_X64_SSE "Whether FCL should x64 SSE instructions" ON)
if(FCL_USE_X64_SSE)
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(SSE_FLAGS -mfpmath=sse -msse -msse2 -msse3 -mssse3)
  elseif(MSVC)
    set(SSE_FLAGS /arch:SSE2)
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SSE_FLAGS}")
  
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall")



