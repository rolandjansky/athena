################################################################################
# Package: MuonTrackSteeringTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackSteeringTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrackFindingEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerUtils
                          MuonSpectrometer/MuonReconstruction/MuonTrackMakers/MuonTrackMakerUtils
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkUtilityPackages/TrkDriftCircleMath )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonTrackSteeringTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier EventPrimitives GaudiKernel MagFieldInterfaces MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonTrackFindingEvent MuonRecHelperToolsLib MuonRecToolInterfaces MuonSegmentMakerUtils TrkDetDescrUtils TrkGeometry TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkParameters TrkPseudoMeasurementOnTrack TrkSegment TrkTrack TrkTrackSummary TrkExInterfaces TrkExUtils TrkFitterInterfaces TrkToolInterfaces TrkDriftCircleMath )

# Install files from the package:
atlas_install_headers( MuonTrackSteeringTools )
atlas_install_joboptions( share/*.py )

