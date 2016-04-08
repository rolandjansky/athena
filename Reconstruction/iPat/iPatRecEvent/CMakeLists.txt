################################################################################
# Package: iPatRecEvent
################################################################################

# Declare the package name:
atlas_subdir( iPatRecEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Reconstruction/iPat/iPatTrack )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( iPatRecEvent
                   src/iPatTrackContainer.cxx
                   PUBLIC_HEADERS iPatRecEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools iPatTrack
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( iPatTrackContainerDict
                      iPatRecEvent/iPatTrackContainerDict.h
                      iPatRecEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools iPatTrack iPatRecEvent )

