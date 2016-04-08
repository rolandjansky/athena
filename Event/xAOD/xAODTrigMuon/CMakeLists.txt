################################################################################
# Package: xAODTrigMuon
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMuon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Physics Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODTrigMuon
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigMuon
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODBase xAODCore xAODTracking
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( xAODTrigMuonDict
                      xAODTrigMuon/xAODTrigMuonDict.h
                      xAODTrigMuon/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODBase xAODCore xAODTracking AthLinks xAODTrigMuon
                      EXTRA_FILES Root/dict/*.cxx )

