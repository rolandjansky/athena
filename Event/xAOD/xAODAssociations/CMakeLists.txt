# $Id: CMakeLists.txt 744228 2016-05-02 14:34:11Z krasznaa $
################################################################################
# Package: xAODAssociations
################################################################################

# Declare the package name:
atlas_subdir( xAODAssociations )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODCore
   Event/xAOD/xAODTracking )

# Component(s) in the package:
atlas_add_library( xAODAssociations
   xAODAssociations/*.h Root/*.cxx
   PUBLIC_HEADERS xAODAssociations
   LINK_LIBRARIES AthContainers AthLinks xAODCaloEvent xAODCore xAODTracking )

atlas_add_dictionary( xAODAssociationsDict
   xAODAssociations/xAODAssociationsDict.h
   xAODAssociations/selection.xml
   LINK_LIBRARIES xAODAssociations
   EXTRA_FILES Root/dict/*.cxx )
