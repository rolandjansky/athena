################################################################################
# Package: xAODJet
################################################################################

# Declare the package name:
atlas_subdir( xAODJet )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTrigger
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CxxUtils
                          Control/SGTools
                          Event/xAOD/xAODCaloEvent )

# External dependencies:
find_package( ROOT COMPONENTS GenVector Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODJet
                   Root/*.cxx
                   PUBLIC_HEADERS xAODJet
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks xAODBTagging xAODBase xAODCore xAODPFlow xAODTrigger
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CxxUtils SGTools xAODCaloEvent )

atlas_add_dictionary( xAODJetDict
                      xAODJet/xAODJetDict.h
                      xAODJet/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBTagging xAODBase xAODCore xAODPFlow xAODTrigger TestTools CxxUtils SGTools xAODCaloEvent xAODJet
                      EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( xAODJet_Jet_test
                SOURCES
                test/xAODJet_Jet_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBTagging xAODBase xAODCore xAODPFlow xAODTrigger TestTools CxxUtils SGTools xAODCaloEvent xAODJet )

