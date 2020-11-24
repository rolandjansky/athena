# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloD3PDMaker )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CaloD3PDMaker
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloInterfaceLib CaloEvent CaloIdentifier CaloUtilsLib AthContainers AthenaBaseComps AthenaKernel xAODCaloEvent GaudiKernel LArRecEvent D3PDMakerUtils PrimaryDPDMakerLib TileEvent TileIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
