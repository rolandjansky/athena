# $Id: CMakeLists.txt 744541 2016-05-03 15:55:30Z krasznaa $
################################################################################
# Package: xAODPFlow
################################################################################

# Declare the package name:
atlas_subdir( xAODPFlow )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODCore
   Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS Core Physics )

# Component(s) in the package:
atlas_add_library( xAODPFlow
   xAODPFlow/*.h xAODPFlow/versions/*.h xAODPFlow/versions/*.icc Root/*.cxx
   PUBLIC_HEADERS xAODPFlow
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase
   xAODCaloEvent xAODCore xAODTracking )

atlas_add_dictionary( xAODPFlowDict
   xAODPFlow/xAODPFlowDict.h
   xAODPFlow/selection.xml
   LINK_LIBRARIES xAODPFlow
   EXTRA_FILES Root/dict/*.cxx )
