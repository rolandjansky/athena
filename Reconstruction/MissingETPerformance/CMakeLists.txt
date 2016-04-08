################################################################################
# Package: MissingETPerformance
################################################################################

# Declare the package name:
atlas_subdir( MissingETPerformance )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorObjects
                          LArCalorimeter/LArRecConditions
                          PhysicsAnalysis/AnalysisCommon/AnalysisTools
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Jet/JetUtils
                          Reconstruction/MissingETEvent
                          Reconstruction/MissingETGoodness
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Event/FourMom
                          Event/FourMomUtils
                          Event/xAOD/xAODEventInfo
                          Generators/GenAnalysisTools/TruthHelper
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigConfiguration/TrigConfL1Data )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Graf Gpad Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf3d Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( MissingETPerformanceLib
                   src/*.cxx
                   PUBLIC_HEADERS MissingETPerformance
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloEvent AthenaBaseComps SGTools GaudiKernel GeneratorObjects LArRecConditions AnalysisTriggerEvent McParticleEvent JetEvent JetUtils MissingETEvent muonEvent Particle egammaEvent TrigCaloEvent TrigInDetEvent TrigMissingEtEvent TrigMuonEvent StoreGateLib SGtests AthAnalysisToolsLib MissingETGoodnessLib TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPMC_LIBRARIES} CaloGeoHelpers FourMom FourMomUtils xAODEventInfo TruthHelper tauEvent VxVertex TrigConfL1Data )

atlas_add_component( MissingETPerformance
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} CaloConditions CaloEvent AthenaBaseComps SGTools StoreGateLib SGtests GaudiKernel GeneratorObjects LArRecConditions AthAnalysisToolsLib AnalysisTriggerEvent McParticleEvent JetEvent JetUtils MissingETEvent MissingETGoodnessLib muonEvent Particle egammaEvent TrigDecisionToolLib TrigCaloEvent TrigInDetEvent TrigMissingEtEvent TrigMuonEvent CaloGeoHelpers FourMom FourMomUtils xAODEventInfo TruthHelper tauEvent VxVertex TrigConfL1Data MissingETPerformanceLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

