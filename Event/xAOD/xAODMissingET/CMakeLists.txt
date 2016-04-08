################################################################################
# Package: xAODMissingET
################################################################################

# Declare the package name:
atlas_subdir( xAODMissingET )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          PRIVATE
                          AtlasTest/TestTools
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODMissingET
                   Root/*.cxx
                   PUBLIC_HEADERS xAODMissingET
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AthLinks xAODBase xAODCore xAODJet
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools xAODCaloEvent xAODTracking xAODTruth )

atlas_add_dictionary( xAODMissingETDict
                      xAODMissingET/xAODMissingETDict.h
                      xAODMissingET/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore xAODJet TestTools xAODCaloEvent xAODTracking xAODTruth xAODMissingET
                      EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( edm_test
                SOURCES
                test/edm_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore xAODJet TestTools xAODCaloEvent xAODTracking xAODTruth xAODMissingET )

