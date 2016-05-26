# $Id: CMakeLists.txt 749875 2016-05-26 13:40:34Z krasznaa $
################################################################################
# Package: xAODJet
################################################################################

# Declare the package name:
atlas_subdir( xAODJet )

# Extra dependencies, when we are not in a standalone build:
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/SGTools )
   set( extra_libs SGTools )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODBTagging
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODPFlow
   Event/xAOD/xAODTrigger
   PRIVATE
   Control/CxxUtils
   Event/xAOD/xAODCaloEvent
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core GenVector )

# Component(s) in the package:
atlas_add_library( xAODJet
   xAODJet/*.h xAODJet/*.icc xAODJet/versions/*.h xAODJet/versions/*.icc
   Root/*.cxx
   PUBLIC_HEADERS xAODJet
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBTagging
   xAODBase xAODCore xAODPFlow xAODTrigger
   PRIVATE_LINK_LIBRARIES CxxUtils xAODCaloEvent )

atlas_add_dictionary( xAODJetDict
   xAODJet/xAODJetDict.h
   xAODJet/selection.xml
   LINK_LIBRARIES xAODJet
   EXTRA_FILES Root/dict/*.cxx )

# Test(s) in the package:
atlas_add_test( xAODJet_Jet_test
   SOURCES test/xAODJet_Jet_test.cxx
   LINK_LIBRARIES xAODCore xAODCaloEvent xAODJet CxxUtils ${extra_libs} )
