# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_Cabling )

# External dependencies:
find_package( tdaq-common )

atlas_add_library( TRT_CablingLib
                   TRT_Cabling/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_Cabling
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel ${TDAQ-COMMON_LIBRARIES} )

# Component(s) in the package:
atlas_add_component( TRT_Cabling
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel AthenaBaseComps CxxUtils Identifier InDetIdentifier TRT_ReadoutGeometry PathResolver TRT_CablingLib )

# Install files from the package:
atlas_install_runtime( share/*.dat )

