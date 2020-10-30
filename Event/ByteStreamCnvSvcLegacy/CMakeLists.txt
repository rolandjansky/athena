# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamCnvSvcLegacy )

# External dependencies:
find_package( Boost COMPONENTS system )
find_package( UUID )
find_package( tdaq-common COMPONENTS EventStorage eformat eformat_write ers 
   RawFileName )
find_package( ZLIB )

# Component(s) in the package:
atlas_add_library( ByteStreamCnvSvcLegacy
   ByteStreamCnvSvcLegacy/offline_eformat/old/*.h src/offline_eformat/old/*.cxx
   ByteStreamCnvSvcLegacy/offline_EventStorage/*.h
   src/offline_EventStorage/*.cxx
   PUBLIC_HEADERS ByteStreamCnvSvcLegacy
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${UUID_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}
   ${CMAKE_CURRENT_SOURCE_DIR}/src/offline_EventStorage
   LINK_LIBRARIES CxxUtils ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${UUID_LIBRARIES} ${ZLIB_LIBRARIES} )
