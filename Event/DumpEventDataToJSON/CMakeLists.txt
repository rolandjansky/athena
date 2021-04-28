# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DumpEventDataToJSON )

# External dependencies:
find_package( nlohmann_json )

# Component(s) in the package:
atlas_add_component( DumpEventDataToJSON
   src/*.cxx
   src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps CaloEvent GaudiKernel StoreGateLib TrkExInterfaces MuonPrepRawData InDetPrepRawData TrkTrack nlohmann_json::nlohmann_json xAODCaloEvent xAODEventInfo xAODJet xAODMuon xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
