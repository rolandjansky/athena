# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackCaloClusterRecValidation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread EG )

# Component(s) in the package:
atlas_add_component( TrackCaloClusterRecValidation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthenaMonitoringLib GaudiKernel InDetPhysValMonitoringLib JetCalibToolsLib StoreGateLib TrkValHistUtils xAODBase xAODCaloEvent xAODCore xAODEventInfo xAODJet xAODPFlow xAODParticleEvent xAODTracking xAODTruth )

# Install files from the package:
atlas_install_runtime( share/*.xml)

