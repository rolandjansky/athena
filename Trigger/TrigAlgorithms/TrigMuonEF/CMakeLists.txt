################################################################################
# Package: TrigMuonEF
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonEF )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          DetectorDescription/IRegionSelector
                          Event/EventPrimitives
                          Event/FourMomUtils
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTracking
			  GaudiKernel 
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool			 
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/CscClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentCombiners/MuonSegmentCombinerTools/MuonSegmentCombinerToolInterfaces			  
			  PhysicsAnalysis/AnalysisCommon/PATCore
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Reconstruction/Particle
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigMuonToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

include_directories(src)

# Component(s) in the package:
atlas_add_component( TrigMuonEF
                     src/SegmentCache.cxx
                     src/TrigMuonEFStandaloneTrackTool.cxx
                     src/TrigMuonEFTrackIsolation.cxx
                     src/TrigMuonEFTrackIsolationTool.cxx
                     src/TrigMuonEFRoiAggregator.cxx
                     src/InDetTrkRoiMaker.cxx
                     src/TrigMuonEFFSRoiMaker.cxx
                     src/TrigMuonEFRoiUtils.cxx
                     src/TrigMuonEFCaloIsolation.cxx
                     src/TrigMuonEFIDTrackRoiMaker.cxx
                     src/TrigMuonEFTrackIsolationAlgMT.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TrigMuonEvent TrigSteeringEvent CaloEvent AthenaBaseComps AthenaKernel StoreGateLib SGtests IRegionSelector EventPrimitives FourMomUtils xAODCaloEvent xAODMuon xAODTracking GaudiKernel MuonReadoutGeometry MuonIdHelpersLib CscClusterizationLib MuonPattern MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecToolInterfaces MuidInterfaces MuonCombinedEvent MuonCombinedToolInterfaces Particle PATCoreLib RecoToolInterfaces TrkEventPrimitives TrkParameters TrkSegment TrkTrack TrkTrackSummary TrkToolInterfaces TrigConfHLTData TrigCaloEvent TrigInDetEvent TrigNavigationLib TrigInterfacesLib TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigMuonEF )
atlas_install_python_modules( python/TrigMuonEFConfig.py python/TrigMuonEFCosmicConfig.py python/TrigMuonEFMonitoring.py python/TrigMuonEFConfigDb.py )
atlas_install_joboptions( share/test_TrigMuonEF.py )

