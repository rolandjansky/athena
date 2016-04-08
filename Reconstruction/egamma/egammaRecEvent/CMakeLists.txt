################################################################################
# Package: egammaRecEvent
################################################################################

# Declare the package name:
atlas_subdir( egammaRecEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/SGTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( egammaRecEvent
                   Root/*.cxx
                   PUBLIC_HEADERS egammaRecEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks SGTools xAODCaloEvent xAODTracking
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( egammaRecEventDict
                      egammaRecEvent/egammaRecEventDict.h
                      egammaRecEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks SGTools xAODCaloEvent xAODTracking egammaRecEvent )

