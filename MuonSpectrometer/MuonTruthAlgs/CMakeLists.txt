################################################################################
# Package: MuonTruthAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonTruthAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonSimData
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthLinks
                          Control/AthContainers
                          DetectorDescription/AtlasDetDescr
                          Event/EventPrimitives
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonGeoModel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonTrackMakers/MuonTrackMakerUtils
                          PhysicsAnalysis/MCTruthClassifier
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces )

# External dependencies:
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( MuonTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPMC_LIBRARIES} StoreGateLib SGtests Identifier GaudiKernel MuonPrepRawData MuonRecToolInterfaces MuonSimData TrkTrack TrkTruthData TrkToolInterfaces AthenaBaseComps AthLinks AthContainers AtlasDetDescr EventPrimitives xAODMuon xAODTracking xAODTruth GeneratorObjects MuonReadoutGeometry MuonGeoModelLib MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPattern MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MCTruthClassifierLib Particle ParticleTruth TrkDetElementBase TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventUtils TrkMeasurementBase TrkParameters TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkExInterfaces TrkFitterInterfaces )

# Install files from the package:
atlas_install_headers( MuonTruthAlgs )

