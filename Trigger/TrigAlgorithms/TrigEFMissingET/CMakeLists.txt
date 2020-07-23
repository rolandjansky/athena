# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEFMissingET )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib COMPONENTS SoftKiller ConstituentSubtractor )
find_package( ROOT COMPONENTS Core Hist Matrix )
find_package( tdaq-common COMPONENTS eformat )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( TrigEFMissingET
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${FASTJET_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaMonitoringKernelLib CaloConditions CaloDetDescrLib CaloEvent CaloGeoHelpers CaloIdentifier CaloInterfaceLib CxxUtils FourMomUtils GaudiKernel IRegionSelector Identifier InDetTrackSelectionToolLib JetEDM JetEvent LArCablingLib LArIdentifier LArRecConditions LArRecEvent PathResolver StoreGateLib TrigInterfacesLib TrigMissingEtEvent TrigParticle TrigSteeringEvent TrigT1Interfaces TrigT2CaloCommonLib TrigTimeAlgsLib xAODCaloEvent xAODCore xAODEventInfo xAODJet xAODMuon xAODPFlow xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
