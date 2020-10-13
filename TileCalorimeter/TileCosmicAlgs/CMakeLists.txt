# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileCosmicAlgs )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Minuit2 Core )

# Component(s) in the package:
atlas_add_component( TileCosmicAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps GaudiKernel CaloDetDescrLib CaloEvent CommissionEvent AthenaKernel EventContainers Identifier StoreGateLib TileDetDescr TileEvent TileIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( TileMuonFitterConfig_test
                SCRIPT python -m TileCosmicAlgs.TileMuonFitterConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
