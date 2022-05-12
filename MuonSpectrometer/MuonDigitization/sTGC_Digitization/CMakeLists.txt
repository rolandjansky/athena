# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( sTGC_Digitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Hist RIO )

# Component(s) in the package:
atlas_add_component( sTGC_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel PileUpToolsLib Identifier xAODEventInfo GaudiKernel MuonSimEvent HitManagement GeneratorObjects MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonSimData PathResolver TrkDetDescrUtils TrkSurfaces TrkEventPrimitives NSWCalibToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/*.dat )
atlas_install_runtime( share/*.config )

