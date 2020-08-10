# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ByteStreamData )

# External dependencies:
find_package( tdaq-common COMPONENTS DataReader )

# Treat tdaq-common as an optional dependency:
set( extra_inc )
set( extra_lib )
if( TDAQ-COMMON_FOUND )
   set( extra_inc INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} )
   set( extra_lib ${TDAQ-COMMON_LIBRARIES} )
endif()

# Component(s) in the package:
atlas_add_library( ByteStreamData
   ByteStreamData/*.h
   src/ROBData.cxx
   src/ByteStreamMetadata.cxx
   PUBLIC_HEADERS ByteStreamData
   ${extra_inc}
   LINK_LIBRARIES AthenaKernel AthContainers ${extra_lib} )

atlas_add_library( ByteStreamData_test
   src/ROBData_t.cxx
   NO_PUBLIC_HEADERS
   LINK_LIBRARIES ByteStreamData )

atlas_add_dictionary( ByteStreamDataDict
   ByteStreamData/ByteStreamDataDict.h
   ByteStreamData/selection.xml
   LINK_LIBRARIES ByteStreamData )
