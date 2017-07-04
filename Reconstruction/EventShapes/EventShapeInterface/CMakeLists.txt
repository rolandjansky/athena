# $Id: CMakeLists.txt 777094 2016-10-07 12:33:15Z krasznaa $
################################################################################
# Package: EventShapeInterface
################################################################################

# Declare the package name:
atlas_subdir( EventShapeInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   Event/xAOD/xAODEventShape )

# Component(s) in the package:
atlas_add_library( EventShapeInterface
   EventShapeInterface/*.h
   INTERFACE
   PUBLIC_HEADERS EventShapeInterface
   LINK_LIBRARIES AsgTools xAODBase xAODEventShape )

atlas_add_dictionary( EventShapeInterfaceDict
   EventShapeInterface/EventShapeInterfaceDict.h
   EventShapeInterface/selection.xml
   LINK_LIBRARIES EventShapeInterface )
