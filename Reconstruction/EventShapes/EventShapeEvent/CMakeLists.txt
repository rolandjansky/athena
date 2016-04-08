################################################################################
# Package: EventShapeEvent
################################################################################

# Declare the package name:
atlas_subdir( EventShapeEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Event/EventKernel
                          Event/NavFourMom )

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EventShapeEvent
                   src/*.cxx
                   PUBLIC_HEADERS EventShapeEvent
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES} DataModel SGTools EventKernel NavFourMom
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( EventShapesDict
                      EventShapeEvent/EventShapeEventDict.h
                      EventShapeEvent/selection.xml
                      INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${FASTJET_LIBRARIES} ${ROOT_LIBRARIES} DataModel SGTools EventKernel NavFourMom EventShapeEvent )

