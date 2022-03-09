# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloTrkMuIdTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( CaloTrkMuIdTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent AthenaBaseComps StoreGateLib xAODTracking GaudiKernel ICaloTrkMuIdTools RecoToolInterfaces TrkExInterfaces CaloDetDescrLib CaloGeoHelpers CaloIdentifier CaloUtilsLib xAODCaloEvent ParticleCaloExtension TileDetDescr PathResolver TrkSurfaces TrkCaloExtension TrkParameters AthOnnxruntimeServiceLib FourMomUtils AthenaKernel CxxUtils)

# Install files from the package:
atlas_install_joboptions( share/*.py )
