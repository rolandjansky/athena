# $Id: CMakeLists.txt 744242 2016-05-02 15:24:50Z krasznaa $
################################################################################
# Package: xAODCutFlow
################################################################################

# Declare the package name:
atlas_subdir( xAODCutFlow )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODCore )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_library( xAODCutFlow
   xAODCutFlow/*.h Root/*.cxx
   PUBLIC_HEADERS xAODCutFlow
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AthContainers AthLinks xAODCore
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODCutFlowDict
   xAODCutFlow/xAODCutFlowDict.h
   xAODCutFlow/selection.xml
   LINK_LIBRARIES xAODCutFlow
   EXTRA_FILES Root/dict/*.cxx )
