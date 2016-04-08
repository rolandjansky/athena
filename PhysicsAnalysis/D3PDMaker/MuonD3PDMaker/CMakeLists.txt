################################################################################
# Package: MuonD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( MuonD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/DataModel
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Event/EventInfo
                          Event/EventKernel
                          Event/FourMomUtils
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonSimEvent
                          MuonSpectrometer/MuonTruthAlgs
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/D3PDMaker/TriggerD3PDMaker
                          PhysicsAnalysis/MCTruthClassifier
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigAnalysis/TrigObjectMatching
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( MuonD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils DataModel SGTools StoreGateLib SGtests AtlasDetDescr EventInfo EventKernel FourMomUtils xAODMuon xAODTracking xAODTruth GaudiKernel MuonCalibITools MuonIdHelpersLib MuonPattern MuonPrepRawData MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSimEvent AnalysisTriggerEvent D3PDMakerUtils TriggerD3PDMakerLib MCTruthClassifierLib McParticleEvent muonEvent TrkGeometry TrkEventPrimitives TrkPrepRawData TrkSegment TrkTrackSummary TrkTruthData TrkExInterfaces TrkToolInterfaces TrigObjectMatchingLib TrigInDetEvent TrigMuonEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

