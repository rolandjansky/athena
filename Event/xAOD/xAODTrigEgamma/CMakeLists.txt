################################################################################
# Package: xAODTrigEgamma
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigEgamma )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread GenVector )

# Component(s) in the package:
atlas_add_library( xAODTrigEgamma
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigEgamma
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCaloEvent xAODCore xAODEgamma xAODTracking xAODTrigCalo
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODTrigEgammaDict
                      xAODTrigEgamma/xAODTrigEgammaDict.h
                      xAODTrigEgamma/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCaloEvent xAODCore xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma
                      EXTRA_FILES Root/dict/*.cxx )

