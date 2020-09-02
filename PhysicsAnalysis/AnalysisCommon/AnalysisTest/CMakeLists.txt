################################################################################
# Package: AnalysisTest
################################################################################

# Declare the package name:
atlas_subdir( AnalysisTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/StoreGate
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventKernel
                          Event/FourMom
                          Generators/GeneratorObjects
                          PhysicsAnalysis/AnalysisCommon/AnalysisAssociation
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/TruthParticleID/McParticleUtils
                          Reconstruction/Jet/JetEvent 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( CLHEP )
find_package( CppUnit )

# Component(s) in the package:
atlas_add_component( AnalysisTest
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS  ${CLHEP_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIRS}
                     LINK_LIBRARIES AtlasHepMCLib ${CLHEP_LIBRARIES} ${CPPUNIT_LIBRARIES} AthContainers StoreGateLib SGtests NavFourMom GaudiKernel McParticleEvent muonEvent Particle egammaEvent tauEvent VxVertex TestTools AthenaBaseComps AthenaKernel EventKernel FourMom GeneratorObjects AnalysisAssociation AnalysisUtilsLib AnalysisTriggerEvent McParticleUtils JetEvent )

# Install files from the package:
atlas_install_headers( AnalysisTest )
atlas_install_joboptions( share/*.py )

