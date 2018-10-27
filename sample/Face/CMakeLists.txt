cmake_minimum_required( VERSION 3.6 )

# Require C++11 (or later)
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Create Project
project( NuiTrack )
add_executable( Face nuitrack.h nuitrack.cpp parser.h main.cpp )

# Set StartUp Project
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "Face" )

# Find Package
# NuiTrack
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH} )
set( NuiTrack_DIR "C:/Program Files/NuitrackSDK/Nuitrack" CACHE PATH "Path to NuiTrack directory." )
find_package( NuiTrack REQUIRED )

# OpenCV
set( OpenCV_DIR "C:/Program Files/opencv/build" CACHE PATH "Path to OpenCV config directory." )
find_package( OpenCV REQUIRED )

# Boost (boost::property_tree is header only library)
set( BOOST_ROOT "C:/Program Files/boost" )
#set( Boost_USE_STATIC_LIBS ON ) # Static Link Libraries ( libboost_* )
#set( Boost_USE_MULTITHREADED ON ) # Multi Thread Libraries ( *-mt-* )
#set( Boost_USE_STATIC_RUNTIME OFF ) # Static Runtime Libraries ( *-s* )
find_package( Boost REQUIRED )

# OpenMP
find_package( OpenMP )

if( NuiTrack_FOUND AND OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${NuiTrack_INCLUDE_DIR} )
  include_directories( ${OpenCV_INCLUDE_DIRS} )
  include_directories( ${Boost_INCLUDE_DIRS} )

  # Additional Dependencies
  target_link_libraries( Face ${NuiTrack_LIBRARIES} )
  target_link_libraries( Face ${OpenCV_LIBS} )
endif()

if( OpenMP_FOUND )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
endif()