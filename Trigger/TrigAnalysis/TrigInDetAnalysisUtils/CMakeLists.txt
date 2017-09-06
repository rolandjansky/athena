################################################################################
# Package: TrigInDetAnalysisUtils
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetAnalysisUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          GaudiKernel
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Particle
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
			  Event/xAOD/xAODMuon
			  Event/xAOD/xAODEgamma
			  Event/xAOD/xAODTau
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkTools/TrkParticleCreator
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigInDetAnalysis
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent )

# External dependencies:
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigInDetAnalysisUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetAnalysisUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel McParticleEvent muonEvent Particle egammaEvent tauEvent TrkParameters TrkTrack TrkTrackSummary TrkToolInterfaces TrigInDetAnalysis TrigInDetEvent TrigSteeringEvent TrigInDetTruthEvent StoreGateLib SGtests TrigDecisionToolLib )

