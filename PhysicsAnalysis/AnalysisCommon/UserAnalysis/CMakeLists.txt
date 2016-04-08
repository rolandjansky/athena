################################################################################
# Package: UserAnalysis
################################################################################

# Declare the package name:
atlas_subdir( UserAnalysis )

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
                          Calorimeter/CaloEvent
                          Event/NavFourMom
                          Event/xAOD/xAODEventInfo
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/JetTagging/JetTagInfo
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MissingETEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( UserAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel AthAnalysisToolsLib UserAnalysisUtilsLib TrigDecisionToolLib TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle CaloEvent NavFourMom xAODEventInfo AnalysisTriggerEvent JetTagInfo McParticleEvent JetEvent MissingETEvent muonEvent Particle egammaEvent tauEvent VxVertex TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( UserAnalysis )
atlas_install_joboptions( share/*.py )

