# $Id: CMakeLists.txt 730358 2016-03-16 12:18:50Z krasznaa $
################################################################################
# Package: McParticleEvent
################################################################################

# Declare the package name:
atlas_subdir( McParticleEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/CLIDSvc
   Control/DataModel
   Control/Navigation
   Event/EventKernel
   Event/NavFourMom
   Generators/GeneratorObjects
   PhysicsAnalysis/AnalysisCommon/ParticleEvent
   Tracking/TrkEvent/VxVertex
   PRIVATE
   AtlasTest/TestTools
   GaudiKernel )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( McParticleEvent
   McParticleEvent/*.h src/*.cxx
   PUBLIC_HEADERS McParticleEvent
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} DataModel Navigation
   EventKernel NavFourMom GeneratorObjects ParticleEvent VxVertex
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} )

atlas_add_dictionary( McParticleEventDict
   McParticleEvent/McParticleEventDict.h McParticleEvent/selection.xml
   LINK_LIBRARIES McParticleEvent
   NAVIGABLES TruthParticleContainer 
   ELEMENT_LINKS McEventCollection TruthEtIsolationsContainer )

# Helper variable for the test:
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Test(s) in the package:
atlas_add_test( TruthParticle_test
   SOURCES test/TruthParticle_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorObjects
   TestTools GaudiKernel McParticleEvent
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}"
   EXTRA_PATTERNS "^StoreGateSvc +DEBUG Property update for OutputLevel" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
