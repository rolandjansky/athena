################################################################################
# Package: MuonRecToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonRecToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
			  Generators/GeneratorObjects
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
						  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonEDM_AssociationObjects
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLayerHough
                          MuonSpectrometer/MuonSimData
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
			  Tracking/TrkEvent/TrkSegment
                          Tracking/TrkTools/TrkCompetingRIOsOnTrackTool
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkUtilityPackages/TrkDriftCircleMath )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonRecToolInterfaces
                   PUBLIC_HEADERS MuonRecToolInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives Identifier EventPrimitives xAODTracking GeneratorObjects GaudiKernel MuonLayerEvent MuonPattern MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonEDM_AssociationObjects MuonLayerHough MuonSimData MuonCombinedEvent TrkEventPrimitives TrkMeasurementBase TrkParameters TrkPrepRawData TrkTrack TrkTruthData TrkToolInterfaces TrkDriftCircleMath MuonIdHelpersLib )

