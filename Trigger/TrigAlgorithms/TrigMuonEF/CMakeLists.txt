# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonEF )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_component( TrigMuonEF
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib CaloEvent CscClusterizationLib CxxUtils EventPrimitives FourMomUtils GaudiKernel IRegionSelector InDetTrackSelectionToolLib MuidInterfaces MuonCablingData MuonCnvToolInterfacesLib MuonCombinedEvent MuonCombinedToolInterfaces MuonIdHelpersLib MuonPattern MuonPrepRawData MuonRIO_OnTrack MuonReadoutGeometry MuonRecToolInterfaces MuonSegment MuonSegmentCombinerToolInterfaces MuonSegmentMakerToolInterfaces Particle RecoToolInterfaces RPC_CondCablingLib StoreGateLib TrigCompositeUtilsLib TrigConfHLTData TrigInDetEvent TrigInterfacesLib TrigMuonEvent TrigMuonToolInterfaces TrigNavigationLib TrigParticle TrigSteeringEvent TrigTimeAlgsLib TrkEventPrimitives TrkParameters TrkSegment TrkToolInterfaces TrkTrack TrkTrackSummary xAODCaloEvent xAODMuon xAODTracking xAODTrigMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
