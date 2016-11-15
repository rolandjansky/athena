# $Id: CMakeLists.txt 783840 2016-11-12 17:28:04Z ssnyder $
################################################################################
# Package: TrigNavigation
################################################################################

# Declare the package name:
atlas_subdir( TrigNavigation )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Control/AthToolSupport/AsgTools
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/CLIDSvc
   Control/SGTools
   Control/StoreGate
   GaudiKernel
   Trigger/TrigEvent/TrigNavStructure
   Trigger/TrigEvent/TrigStorageDefinitions
   PRIVATE
   AtlasTest/TestTools
   Control/AthContainersInterfaces
   Control/AthContainersRoot
   Control/RootUtils
   Control/CxxUtils
   Event/xAOD/xAODCore
   Trigger/TrigConfiguration/TrigConfHLTData
   Trigger/TrigDataAccess/TrigSerializeCnvSvc
   Trigger/TrigDataAccess/TrigSerializeResult )

# External dependencies:
find_package( Boost COMPONENTS regex )

# Component(s) in the package:
atlas_add_library( TrigNavigationLib
   TrigNavigation/*.h TrigNavigation/*.icc src/*.cxx
   PUBLIC_HEADERS TrigNavigation
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AthContainersRoot AthLinks AsgTools
   AthenaBaseComps AthenaKernel SGTools GaudiKernel TrigNavStructure
   TrigStorageDefinitions StoreGateLib
   PRIVATE_LINK_LIBRARIES xAODCore TrigConfHLTData TrigSerializeCnvSvcLib
   TrigSerializeResultLib )

atlas_add_component( TrigNavigation
   src/components/*.cxx
   LINK_LIBRARIES TrigNavigationLib )

atlas_add_dictionary( TrigNavigationDict
  TrigNavigation/TrigNavigationDict.h TrigNavigation/selection.xml
   EXTRA_FILES test/dict/*.cxx
   LINK_LIBRARIES TrigNavigationLib )

# Test(s) in the package:
foreach( name TriggerElement_test Holder_test Registration_test Ownership_test HLTNavigation_test RoICache_test )
#foreach( name TriggerElement_test Holder_test Registration_test Ownership_test
#      HLTNavigation_test RoICache_test )

   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} TrigNavigationLib AthContainers AthContainersRoot
      AthLinks AthenaKernel StoreGateLib GaudiKernel TestTools xAODCore
      TrigSerializeCnvSvcLib
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
      EXTRA_PATTERNS "^ClassIDSvc *VERBOSE|added entry for CLID|no dictionary for class|when retrieved"
       )

endforeach()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/test.txt share/navigation2dot.py )

