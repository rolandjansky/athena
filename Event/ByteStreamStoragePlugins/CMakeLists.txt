# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamStoragePlugins )

# External dependencies:
find_package( Boost COMPONENTS system )
find_package( Xrootd COMPONENTS Posix PosixPreload )
find_package( tdaq-common COMPONENTS ers )
find_package( Davix )

# Make sure that libraries are linked correctly:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( fReadXRootD
   src/fReadXRootD.h src/fReadXRootD.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${XROOTD_INCLUDE_DIRS}
   ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${XROOTD_LIBRARIES}
   ${Boost_LIBRARIES}
   PRIVATE_DEFINITIONS -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
   -D_FILE_OFFSET_BITS=64 )

atlas_add_library( fReadDavix
   src/fReadDavix.h src/fReadDavix.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   ${DAVIX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${DAVIX_LIBRARIES}
   ${Boost_LIBRARIES} CxxUtils )
