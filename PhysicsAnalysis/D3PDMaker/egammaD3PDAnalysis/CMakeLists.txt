# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaD3PDAnalysis )

# Component(s) in the package:
atlas_add_component( egammaD3PDAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthenaBaseComps AthenaKernel CaloEvent CaloGeoHelpers D3PDMakerInterfaces EventKernel GaudiKernel LArCablingLib MCTruthClassifierLib RecoToolInterfaces StoreGateLib TrkCaloExtension egammaInterfacesLib xAODCaloEvent xAODEgamma xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
