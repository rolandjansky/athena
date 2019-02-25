# $Id: CMakeLists.txt 744481 2016-05-03 13:36:43Z krasznaa $
################################################################################
# Package: xAODBTagging
################################################################################

# Declare the package name:
atlas_subdir( xAODBTagging )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODTracking
   Control/AthLinks 
   Control/CxxUtils)

# Component(s) in the package:
atlas_add_library( xAODBTagging
   xAODBTagging/*.h xAODBTagging/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODBTagging
   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCore xAODTracking CxxUtils)

atlas_add_dictionary( xAODBTaggingDict
   xAODBTagging/xAODBTaggingDict.h
   xAODBTagging/selection.xml
   LINK_LIBRARIES xAODBTagging
   EXTRA_FILES Root/dict/*.cxx )
