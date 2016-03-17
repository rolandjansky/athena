# $Id: CMakeLists.txt 730743 2016-03-17 15:28:57Z krasznaa $
################################################################################
# Package: ByteStreamStoragePlugins
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamStoragePlugins )

# External dependencies:
find_package( Boost )
find_package( CASTOR COMPONENTS rfio )
find_package( Xrootd COMPONENTS PosixPreload )
find_package( dcache_client )
find_package( tdaq-common COMPONENTS ers )

# Make sure that libraries are linked correctly:
string( REPLACE "-Wl,--as-needed" "" CMAKE_SHARED_LINKER_FLAGS
   "${CMAKE_SHARED_LINKER_FLAGS}" )

# Component(s) in the package:
atlas_add_library( fReadCastor
   src/fReadCastor.h src/fReadCastor.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CASTOR_INCLUDE_DIRS}
   ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CASTOR_LIBRARIES}
   ${Boost_LIBRARIES} )

atlas_add_library( fReadXRootD
   src/fReadXRootD.h src/fReadXRootD.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${XROOTD_INCLUDE_DIRS}
   ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${XROOTD_LIBRARIES}
   ${Boost_LIBRARIES}
   PRIVATE_DEFINITIONS -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
   -D_FILE_OFFSET_BITS=64 )

atlas_add_library( fReaddCache
   src/fReaddCache.h src/fReaddCache.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   ${DCACHE_CLIENT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${DCACHE_CLIENT_LIBRARIES}
   ${Boost_LIBRARIES} )
