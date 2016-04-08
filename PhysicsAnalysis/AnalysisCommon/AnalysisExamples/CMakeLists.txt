################################################################################
# Package: AnalysisExamples
################################################################################

# Declare the package name:
atlas_subdir( AnalysisExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/Navigation
                          Control/SGTools
                          Control/StoreGate
                          Event/NavFourMom
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Generators/GenAnalysisTools/TruthHelper
                          Generators/GeneratorObjects
                          PhysicsAnalysis/AnalysisCommon/AnalysisTools
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          PhysicsAnalysis/AnalysisCommon/CompositeParticleEvent
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/JetTagging/JetTagEvent
                          PhysicsAnalysis/JetTagging/JetTagInfo
                          PhysicsAnalysis/MuonID/MuonIDEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleKernel
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaEvent
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( AnalysisExamples
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel Navigation SGTools StoreGateLib SGtests NavFourMom xAODEventInfo xAODTracking GaudiKernel TruthHelper GeneratorObjects AthAnalysisToolsLib AnalysisUtilsLib ParticleEvent AnalysisTriggerEvent JetTagEvent JetTagInfo MuonIDEvent McParticleEvent McParticleKernel JetEvent muonEvent Particle ParticleTruth RecoToolInterfaces egammaEvent VxVertex TrkVKalVrtFitterLib TrigDecisionToolLib TrigInDetEvent TrigParticle TrigSteeringEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

