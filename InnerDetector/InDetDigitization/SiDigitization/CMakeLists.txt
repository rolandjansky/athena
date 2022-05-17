# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiDigitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core RIO Hist )

# Component(s) in the package:
atlas_add_library( SiDigitization
                   src/SiChargedDiode.cxx
                   src/SiChargedDiodeCollection.cxx
                   src/SiSurfaceCharge.cxx
                   src/InducedChargeModel.cxx
                   PUBLIC_HEADERS SiDigitization
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthAllocators Identifier GaudiKernel ReadoutGeometryBase InDetSimEvent HitManagement
                   #These are needed only because of InducedChargeModel
                   #Can be removed once this is moved elsewhere
                   InDetConditionsSummaryService MagFieldConditions PathResolver SiPropertiesToolLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )
