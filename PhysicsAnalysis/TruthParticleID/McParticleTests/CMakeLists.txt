################################################################################
# Package: McParticleTests
################################################################################

# Declare the package name:
atlas_subdir( McParticleTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaPython
                          Control/StoreGate
                          Event/EventKernel
                          Event/NavFourMom
                          GaudiKernel
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          PhysicsAnalysis/TruthParticleID/McParticleAlgs
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleKernel
                          PhysicsAnalysis/TruthParticleID/McParticleUtils )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( McParticleTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps StoreGateLib SGtests EventKernel NavFourMom GaudiKernel GeneratorObjects McParticleEvent McParticleKernel McParticleUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.ref )
