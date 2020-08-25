# $Id: CMakeLists.txt 778359 2016-10-13 19:24:10Z krasznaa $
################################################################################
# Package: JetRec
################################################################################

# Declare the package name:
atlas_subdir( JetRec )

# Extra dependencies, based on the environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

set( mon_deps )
set( mon_lib )
if( NOT GENERATIONBASE AND NOT XAOD_STANDALONE )
   set( mon_deps Control/AthenaMonitoringKernel )
   set( mon_lib AthenaMonitoringKernelLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthLinks
  Control/AthContainers
  ${mon_deps}
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODJet
  Event/xAOD/xAODMuon
  Reconstruction/EventShapes/EventShapeInterface
  Reconstruction/Jet/JetEDM
  Reconstruction/Jet/JetInterface
  PRIVATE
  Control/CxxUtils
  Event/xAOD/xAODBase
  Event/xAOD/xAODCore
  Event/xAOD/xAODEventInfo
  Event/xAOD/xAODTracking
  ${extra_deps} )

# External dependencies:
find_package( FastJet COMPONENTS fastjetplugins fastjettools siscone
  siscone_spherical )
find_package( FastJetContrib COMPONENTS VariableR RecursiveTools )
find_package( ROOT COMPONENTS Core )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( JetRecLib
  JetRec/*.h Root/*.h Root/*.cxx
  PUBLIC_HEADERS JetRec
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} ${FASTJET_LIBRARIES} AthLinks AthContainers AsgTools
  xAODCaloEvent xAODJet xAODMuon EventShapeInterface JetEDM
  JetInterface  ${mon_lib} AsgDataHandlesLib
  PRIVATE_LINK_LIBRARIES CxxUtils xAODBase xAODCore
  xAODEventInfo xAODTracking )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetRec
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps JetInterface JetRecLib ${mon_lib} )
endif()

atlas_add_dictionary( JetRecDict
   JetRec/JetRecDict.h
   JetRec/selection.xml
   LINK_LIBRARIES JetRecLib ${mon_lib} )

# Test(s) in the package:
atlas_add_test( TestTests
 SOURCES
 # test/all_tests.cx
 test/TestTest.cxx
 INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
 LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} JetRecLib )

if ( NOT XAOD_STANDALONE )
atlas_add_test(JetTests
  SOURCES
  test/JetTest.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} JetRecLib )
endif()

if ( NOT XAOD_STANDALONE )
atlas_add_test(PseudoJetContainerOffline
  SOURCES
  test/PseudoJetContainerOfflineTest.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} JetRecLib )
endif()

if ( NOT GENERATIONBASE AND NOT XAOD_STANDALONE )
atlas_add_test(PseudoJetContainerTrigger
  SOURCES
  test/PseudoJetContainerTriggerTest.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} JetRecLib )
endif()

if ( NOT XAOD_STANDALONE )
atlas_add_test(ExtractorUnitTests
  SOURCES
    test/ExtractorTest.cxx
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} JetRecLib )
endif()

atlas_add_test(JetClustererTest
  SOURCES
    test/JetClustererTest.cxx
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} JetRecLib )

if ( NOT GENERATIONBASE AND NOT XAOD_STANDALONE )
   atlas_add_test(JetRecAlg_Test SCRIPT share/JetRecAlgTestCfg.py )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
