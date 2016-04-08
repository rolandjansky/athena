################################################################################
# Package: xAODAssociations
################################################################################

# Declare the package name:
atlas_subdir( xAODAssociations )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODAssociations
                   Root/*.cxx
                   PUBLIC_HEADERS xAODAssociations
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks xAODCaloEvent xAODCore xAODTracking
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODAssociationsDict
                      xAODAssociations/xAODAssociationsDict.h
                      xAODAssociations/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODCaloEvent xAODCore xAODTracking xAODAssociations
                      EXTRA_FILES Root/dict/*.cxx )

