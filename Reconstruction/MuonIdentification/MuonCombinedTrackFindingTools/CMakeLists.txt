################################################################################
# Package: MuonCombinedTrackFindingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedTrackFindingTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/MuonClusterization
                          MuonSpectrometer/MuonReconstruction/MuonPatternFinders/MuonPatternFinderTools/MuonHoughPatternTools
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLayerHough
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
			  MuonSpectrometer/MuonCnv/MuonPrepRawDataProviderTools
                          Reconstruction/MuonIdentification/MuGirlStau
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Reconstruction/MuonIdentification/MuonSegmentTaggers/MuonSegmentTaggerToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkUtilityPackages/TrkDriftCircleMath )

# Component(s) in the package:
atlas_add_component( MuonCombinedTrackFindingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps EventPrimitives xAODTracking xAODTruth MdtCalibData MdtCalibSvcLib MuonDetDescrUtils MuonIdHelpersLib MuonClusterizationLib MuonHoughPatternToolsLib MuonCompetingRIOsOnTrack MuonLayerEvent MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonLayerHough MuGirlStauLib MuidInterfaces MuonCombinedEvent MuonCombinedToolInterfaces MuonPrepRawDataProviderToolsLib TrkEventPrimitives TrkParameters TrkTrack TrkExInterfaces TrkToolInterfaces TrkDriftCircleMath )

