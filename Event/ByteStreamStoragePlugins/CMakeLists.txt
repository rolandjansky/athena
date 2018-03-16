# $Id: CMakeLists.txt 780174 2016-10-25 07:53:15Z elmsheus $
################################################################################
# Package: ByteStreamStoragePlugins
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamStoragePlugins )

# External dependencies:
find_package( Boost COMPONENTS system )
find_package( CASTOR COMPONENTS rfio )
find_package( Xrootd COMPONENTS Posix PosixPreload )
find_package( dcap )
find_package( tdaq-common COMPONENTS ers )
find_package( Davix )

# Make sure that libraries are linked correctly:
atlas_disable_as_needed()

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
   ${DCAP_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${DCAP_LIBRARIES}
   ${Boost_LIBRARIES} )

atlas_add_library( fReadDavix
   src/fReadDavix.h src/fReadDavix.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   ${DAVIX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${DAVIX_LIBRARIES}
   ${Boost_LIBRARIES} )
