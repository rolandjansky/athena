################################################################################
# Package: xAODTau
################################################################################

# Declare the package name:
atlas_subdir( xAODTau )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/SGTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread GenVector )

# Component(s) in the package:
atlas_add_library( xAODTau
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTau
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks SGTools xAODBase xAODCore xAODJet xAODPFlow xAODTracking xAODTruth
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_dictionary( xAODTauDict
                      xAODTau/xAODTauDict.h
                      xAODTau/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks SGTools xAODBase xAODCore xAODJet xAODPFlow xAODTracking xAODTruth TestTools xAODTau
                      EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( xAODTau_TauJet_test
                SOURCES
                test/xAODTau_TauJet_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks SGTools xAODBase xAODCore xAODJet xAODPFlow xAODTracking xAODTruth TestTools xAODTau )

