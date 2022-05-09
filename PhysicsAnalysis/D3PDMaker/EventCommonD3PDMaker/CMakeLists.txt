# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventCommonD3PDMaker )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Physics Core )

# Component(s) in the package:
atlas_add_component( EventCommonD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel Navigation EventBookkeeperMetaData EventKernel FourMomUtils NavFourMom xAODBase xAODEventInfo GaudiKernel ParticleEvent D3PDMakerInterfaces D3PDMakerUtils JetEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
