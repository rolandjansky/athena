# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEFMissingET )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist Matrix )
find_package( Eigen )

atlas_add_library( TrigEFMissingETLib 
   Root/*.cxx
   PUBLIC_HEADERS TrigEFMissingET
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${ROOT_LIBRARIES}
      AsgTools xAODTracking xAODTrigMissingET
   PRIVATE_LINK_LIBRARIES xAODBase
)

# Component(s) in the package:
atlas_add_component( TrigEFMissingET
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES}
      TrigEFMissingETLib AthenaBaseComps AsgTools StoreGateLib AthContainers GaudiKernel AthenaMonitoringKernelLib AthLinks
      CaloGeoHelpers CaloEvent CaloConditions CaloIdentifier 
      InDetTrackSelectionToolLib TrackVertexAssociationToolLib TrkCaloExtension RecoToolInterfaces JetEDM
      xAODCaloEvent xAODTrigMissingET xAODTracking xAODJet xAODBase xAODEventShape xAODPFlow
)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
