# $Id: CMakeLists.txt 766963 2016-08-08 20:07:09Z ggach $
################################################################################
# Package: xAODForward
################################################################################

# Declare the package name:
atlas_subdir( xAODForward )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODCore
   Event/xAOD/xAODTrigL1Calo )

# Component(s) in the package:
atlas_add_library( xAODForward
   xAODForward/*.h xAODForward/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODForward
   LINK_LIBRARIES AthContainers xAODCore xAODTrigL1Calo )

atlas_add_dictionary( xAODForwardDict
   xAODForward/xAODForwardDict.h
   xAODForward/selection.xml
   LINK_LIBRARIES AthLinks xAODForward
   EXTRA_FILES Root/dict/*.cxx )
