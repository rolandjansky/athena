################################################################################
# Package: McParticleTools
################################################################################

# Declare the package name:
atlas_subdir( McParticleTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/SGTools
                          Control/AthContainers
                          Control/StoreGate
                          Event/EventKernel
                          GaudiKernel
                          Generators/GenAnalysisTools/TruthHelper
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleKernel
                          PhysicsAnalysis/TruthParticleID/McParticleUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( McParticleTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthContainers StoreGateLib SGtests EventKernel GaudiKernel TruthHelper GeneratorObjects McParticleEvent McParticleKernel McParticleUtils SGTools )
