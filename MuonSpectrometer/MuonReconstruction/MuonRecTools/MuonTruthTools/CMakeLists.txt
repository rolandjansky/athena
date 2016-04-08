################################################################################
# Package: MuonTruthTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTruthTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonTrackMakers/MuonTrackMakerUtils
                          MuonSpectrometer/MuonSimData
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTruthTracks/TrkTruthTrackInterfaces )

# External dependencies:
find_package( Eigen )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonTruthTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier EventPrimitives GaudiKernel MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSimData TrkDetElementBase TrkSurfaces TrkMeasurementBase TrkParameters TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkTruthData TrkExInterfaces TrkFitterInterfaces )

