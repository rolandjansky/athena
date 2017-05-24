# $Id: CMakeLists.txt 801071 2017-03-20 22:11:25Z delsart $
################################################################################
# Package: xAODJet
################################################################################

# Declare the package name:
atlas_subdir( xAODJet )

# Optional dependencies:
set( extra_deps )
set( extra_libs )
set( extra_private_deps )
set( extra_test_libs )

# Extra dependencies, when we are not in a standalone build:
if( NOT XAOD_STANDALONE )
   set( extra_private_deps Control/SGTools )
   set( extra_test_libs SGTools )
else()
   set( extra_private_deps Control/xAODRootAccess )
   set( extra_test_libs xAODRootAccess )
endif()

if( NOT SIMULATIONBASE )
   set( extra_deps Event/xAOD/xAODBTagging Event/xAOD/xAODTrigger Event/xAOD/xAODPFlow)
   set( extra_libs xAODBTagging xAODPFlow xAODTrigger )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   ${extra_deps}
   PRIVATE
   Control/CxxUtils
   Event/xAOD/xAODCaloEvent
   ${extra_private_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core GenVector )

# Component(s) in the package:
atlas_add_library( xAODJet
   xAODJet/*.h xAODJet/*.icc xAODJet/versions/*.h xAODJet/versions/*.icc
   Root/*.cxx
   PUBLIC_HEADERS xAODJet
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks
   xAODBase xAODCore ${extra_libs}
   PRIVATE_LINK_LIBRARIES CxxUtils xAODCaloEvent )

atlas_add_dictionary( xAODJetDict
   xAODJet/xAODJetDict.h
   xAODJet/selection.xml
   LINK_LIBRARIES xAODJet
   EXTRA_FILES Root/dict/*.cxx )

# Test(s) in the package:
atlas_add_test( xAODJet_Jet_test
   SOURCES test/xAODJet_Jet_test.cxx
   LINK_LIBRARIES xAODCore xAODCaloEvent xAODJet CxxUtils ${extra_test_libs} )
