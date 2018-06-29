cmake_minimum_required( VERSION 3.6 )

# Require C++11 (or later)
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Create Project
project( NuiTrack )
add_executable( Skeleton nuitrack.h nuitrack.cpp main.cpp )

# Set StartUp Project
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "Skeleton" )

# Find Package
# NuiTrack
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH} )
set( NuiTrack_DIR "C:/Program Files/NuitrackSDK/Nuitrack" CACHE PATH "Path to NuiTrack directory." )
find_package( NuiTrack REQUIRED )

# OpenCV
set( OpenCV_DIR "C:/Program Files/opencv/build" CACHE PATH "Path to OpenCV config directory." )
find_package( OpenCV REQUIRED )

# OpenMP
find_package( OpenMP )

if( NuiTrack_FOUND AND OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${NuiTrack_INCLUDE_DIR} )
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  # Additional Dependencies
  target_link_libraries( Skeleton ${NuiTrack_LIBRARIES} )
  target_link_libraries( Skeleton ${OpenCV_LIBS} )
endif()

if( OpenMP_FOUND )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
endif()