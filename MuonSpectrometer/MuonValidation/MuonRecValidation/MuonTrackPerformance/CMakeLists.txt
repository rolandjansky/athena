################################################################################
# Package: MuonTrackPerformance
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackPerformance )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonStationIndex
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/MuonClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLayerHough
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          MuonSpectrometer/MuonValidation/MuonRecValidation/MuonRecValidationNtuples
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/MuonIdentification/MuonSegmentTaggers/MuonSegmentTaggerToolInterfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonTrackPerformance
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps xAODEventInfo GaudiKernel MuonIdHelpersLib MuonStationIndexLib MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces TrkParameters TrkTrack TrkTrackSummary TrkTruthData TrkToolInterfaces StoreGateLib SGtests Identifier EventPrimitives xAODMuon xAODTracking xAODTruth MuonReadoutGeometry MuonClusterizationLib MuonCompetingRIOsOnTrack MuonLayerEvent MuonPrepRawData MuonRIO_OnTrack MuonLayerHough MuonRecValidationNtuples MuonCombinedEvent TrkMeasurementBase TrkPrepRawData TrkExInterfaces TrackRecordLib MuonSegmentMakerToolInterfaces MuonSegmentTaggerToolInterfaces )

# Install files from the package:
atlas_install_headers( MuonTrackPerformance )
atlas_install_joboptions( share/*.py )

