# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MM_Digitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Graf )

# Component(s) in the package:
atlas_add_library( MM_DigitizationLib
                   src/*.cxx
                   PUBLIC_HEADERS MM_Digitization
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel GeoPrimitives HitManagement Identifier MagFieldConditions MagFieldElements MuonDigitContainer MuonIdHelpersLib MuonSimData MuonSimEvent NSWCalibToolsLib PileUpToolsLib xAODEventInfo MuonCondData
                   PRIVATE_LINK_LIBRARIES AtlasHepMCLib GeneratorObjects MuonAGDDDescription MuonReadoutGeometry PathResolver TrkDetDescrUtils TrkEventPrimitives TrkSurfaces )

atlas_add_component( MM_Digitization
                     src/components/*.cxx
                     LINK_LIBRARIES MM_DigitizationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
