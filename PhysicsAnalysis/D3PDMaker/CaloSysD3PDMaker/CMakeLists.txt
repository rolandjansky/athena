# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloSysD3PDMaker )

# Component(s) in the package:
atlas_add_component( CaloSysD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel CaloDetDescrLib CaloDmDetDescr CaloEvent CaloGeoHelpers CaloIdentifier CaloSimEvent D3PDMakerUtils GaudiKernel Identifier LArCablingLib LArElecCalib LArIdentifier LArRawEvent LArRecEvent LArSimEvent StoreGateLib TileConditionsLib TileEvent TileIdentifier TileSimEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

