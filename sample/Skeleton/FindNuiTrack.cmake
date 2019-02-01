#.rst:
# FindNuiTrack
# ------------
#
# Find NuiTrack include dirs, and libraries
#
# Use this module by invoking find_package with the form::
#
#    find_package( NuiTrack [REQUIRED] )
#
# Results for users are reported in following variables::
#
#    NuiTrack_FOUND       - Return "TRUE" when NuiTrack found. Otherwise, Return "FALSE".
#    NuiTrack_INCLUDE_DIR - NuiTrack include directory.
#    NuiTrack_LIBRARIES   - NuiTrack library files.
#
# =============================================================================
#
# Copyright (c) 2018 Tsukasa SUGIURA
# Distributed under the MIT License.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# =============================================================================

find_path(
  NuiTrack_INCLUDE_DIR
  NAMES nuitrack/Nuitrack.h
  PATHS "${NuiTrack_DIR}"
        "$ENV{NuiTrack_DIR}"
        "$ENV{PROGRAMFILES}/NuitrackSDK/Nuitrack"
        "$ENV{PROGRAMW6432}/NuitrackSDK/Nuitrack"
        /usr /usr/local
  PATH_SUFFIXES include
)

set(SUFFIX)
set(NUITRACK_LIBRARY)
set(MIDDLEWARE_LIBRARY)
if(WIN32)
  if(NOT CMAKE_CL_64)
    set(SUFFIX win32)
  else()
    set(SUFFIX win64)
  endif()
  set(NUITRACK_LIBRARY nuitrack.lib)
  set(MIDDLEWARE_LIBRARY middleware.lib)
elseif(UNIX AND NOT APPLE)
  if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm.*|ARM.*)")
    set(SUFFIX linux64)
  else()
    set(SUFFIX linux_arm)
  endif()
  set(NUITRACK_LIBRARY libnuitrack.so)
  set(MIDDLEWARE_LIBRARY libmiddleware.so)
else()
  message(WARNING "can't support this platform in this find module yet.")
endif()

find_library(
  NuiTrack_NUITRACK_LIBRARY
  NAMES ${NUITRACK_LIBRARY}
  PATHS "${NuiTrack_DIR}"
        "$ENV{NuiTrack_DIR}"
        "$ENV{PROGRAMFILES}/NuitrackSDK/Nuitrack"
        "$ENV{PROGRAMW6432}/NuitrackSDK/Nuitrack"
        /usr /usr/local
  PATH_SUFFIXES lib/${SUFFIX}
)

find_library(
  NuiTrack_MIDDLEWARE_LIBRARY
  NAMES ${MIDDLEWARE_LIBRARY}
  PATHS "${NuiTrack_DIR}"
        "$ENV{NuiTrack_DIR}"
        "$ENV{PROGRAMFILES}/NuitrackSDK/Nuitrack"
        "$ENV{PROGRAMW6432}/NuitrackSDK/Nuitrack"
        /usr /usr/local
  PATH_SUFFIXES lib/${SUFFIX}
)

set(NuiTrack_LIBRARIES ${NuiTrack_NUITRACK_LIBRARY} ${NuiTrack_MIDDLEWARE_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NuiTrack DEFAULT_MSG NuiTrack_LIBRARIES NuiTrack_INCLUDE_DIR)
mark_as_advanced(NuiTrack_LIBRARIES NuiTrack_INCLUDE_DIR)
