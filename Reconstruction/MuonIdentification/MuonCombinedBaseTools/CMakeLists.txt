################################################################################
# Package: MuonCombinedBaseTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedBaseTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
			  InnerDetector/InDetConditions/BeamSpotConditionsData
        Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
			  PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Control/AthLinks
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/FourMomUtils
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODMuonCnv
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerUtils
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Reconstruction/MuonIdentification/ICaloTrkMuIdTools
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/MuonIdentification/MuonSegmentTaggers/MuonSegmentTaggerToolInterfaces
                          Reconstruction/MuonIdentification/muonEvent
			  Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/ParticleTruth
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/RecoTools/TrackToCalo
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces 
                          Generators/AtlasHepMC
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
			  )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonCombinedBaseTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib CaloEvent AthLinks AthenaBaseComps Identifier EventPrimitives FourMomUtils xAODCaloEvent xAODEventInfo xAODMuon xAODTracking xAODTruth GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSegmentMakerUtils MuonSelectorToolsLib ICaloTrkMuIdTools MuidInterfaces MuonCombinedEvent MuonCombinedToolInterfaces muonEvent MuidEvent ParticleTruth RecoToolInterfaces TrackToCaloLib TrkGeometry TrkSurfaces TrkCaloExtension TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkParameters TrkParametersIdentificationHelpers TrkSegment TrkTrack TrkTrackSummary TrkExInterfaces TrkToolInterfaces BeamSpotConditionsData MagFieldElements MagFieldConditions xAODMuonCnvLib MuonSegmentTaggerToolInterfaces )

