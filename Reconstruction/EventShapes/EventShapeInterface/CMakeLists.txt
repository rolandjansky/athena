################################################################################
# Package: EventShapeInterface
################################################################################

# Declare the package name:
atlas_subdir( EventShapeInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODEventShape )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EventShapeInterface
                   PUBLIC_HEADERS EventShapeInterface
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBase xAODEventShape )

atlas_add_dictionary( EventShapeInterfaceDict
                      EventShapeInterface/EventShapeInterfaceDict.h
                      EventShapeInterface/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBase xAODEventShape EventShapeInterface )

