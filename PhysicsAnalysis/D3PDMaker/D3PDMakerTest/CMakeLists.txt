################################################################################
# Package: D3PDMakerTest
################################################################################

# Declare the package name:
atlas_subdir( D3PDMakerTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthenaKernel
                          Control/Navigation
                          Event/FourMom
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          Event/EventKernel
                          Event/NavFourMom
                          GaudiKernel
                          MuonSpectrometer/MuonSimEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          Simulation/G4Sim/TrackRecord
                          TestPolicy )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( D3PDMakerTestLib
                   src/*.cxx
                   PUBLIC_HEADERS D3PDMakerTest
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthContainers Navigation FourMom ParticleEvent StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils EventKernel NavFourMom GaudiKernel MuonSimEvent D3PDMakerUtils )

atlas_add_component( D3PDMakerTest
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers Navigation FourMom ParticleEvent AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests EventKernel NavFourMom GaudiKernel MuonSimEvent D3PDMakerUtils D3PDMakerTestLib )

atlas_add_dictionary( D3PDMakerTestDict
                      D3PDMakerTest/D3PDMakerTestDict.h
                      D3PDMakerTest/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers Navigation FourMom ParticleEvent AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests EventKernel NavFourMom GaudiKernel MuonSimEvent D3PDMakerUtils D3PDMakerTestLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Tests
set( D3PDMAKERTEST_REFERENCE_TAG D3PDMakerTest/D3PDMakerTest-01-00-00 ) 


atlas_add_test( D3PDTest1
  SCRIPT test/D3PDTest1.sh
  POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/post_check.sh D3PDTest1"
  ENVIRONMENT "ATLAS_REFERENCE_TAG=${D3PDMAKERTEST_REFERENCE_TAG}" )
