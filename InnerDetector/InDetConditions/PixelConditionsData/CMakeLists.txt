# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelConditionsData )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PixelConditionsData
                   src/*.cxx
                   PUBLIC_HEADERS PixelConditionsData
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaKernel AthenaPoolUtilities GeoPrimitives Identifier InDetByteStreamErrors
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} )
