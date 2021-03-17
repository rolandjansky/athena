# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MissingETD3PDMaker )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( MissingETD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps CaloEvent CaloGeoHelpers StoreGateLib EventKernel GaudiKernel D3PDMakerInterfaces D3PDMakerUtils JetEvent JetUtils MissingETEvent MissingETGoodnessLib Particle AthenaKernel FourMomUtils xAODMissingET muonEvent egammaEvent tauEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

