# $Id: CMakeLists.txt 744251 2016-05-02 15:37:06Z krasznaa $
################################################################################
# Package: xAODEventShape
################################################################################

# Declare the package name:
atlas_subdir( xAODEventShape )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Event/xAOD/xAODCore )

# Component(s) in the package:
atlas_add_library( xAODEventShape
   xAODEventShape/*.h xAODEventShape/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEventShape
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_dictionary( xAODEventShapeDict
   xAODEventShape/xAODEventShapeDict.h
   xAODEventShape/selection.xml
   LINK_LIBRARIES xAODEventShape )
