################################################################################
# Package: TrigBphysValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigBphysValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/AnalysisTools
                          PhysicsAnalysis/AnalysisCommon/UserAnalysisUtils
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          PRIVATE
                          Event/EventInfo
                          Generators/GeneratorObjects
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( TrigBphysValidationLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigBphysValidation
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle StoreGateLib SGtests AthAnalysisToolsLib UserAnalysisUtilsLib TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} EventInfo GeneratorObjects AnalysisTriggerEvent McParticleEvent Particle ParticleTruth TrigT1Interfaces TrigInDetTruthEvent )

atlas_add_component( TrigBphysValidation
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel AthAnalysisToolsLib UserAnalysisUtilsLib TrigDecisionToolLib TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle EventInfo GeneratorObjects AnalysisTriggerEvent McParticleEvent Particle ParticleTruth TrigT1Interfaces TrigInDetTruthEvent TrigBphysValidationLib )

# Install files from the package:
atlas_install_joboptions( share/*.txt share/*.py )

