################################################################################
# Package: TrigStorageDefinitions
################################################################################

# Declare the package name:
atlas_subdir( TrigStorageDefinitions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          Control/Navigation
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigBphys
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
                          Event/xAOD/xAODTrigL1Calo
                          Event/xAOD/xAODTrigMinBias
                          Event/xAOD/xAODTrigMissingET
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigRinger
                          Event/xAOD/xAODTrigger
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrigStorageDefinitions
                   PUBLIC_HEADERS TrigStorageDefinitions
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} DataModel Navigation xAODBTagging xAODCaloEvent xAODEgamma xAODHIEvent xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigL1Calo xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODTrigRinger xAODTrigger )

atlas_add_test( TypeInformation_test
                SOURCES
                test/TypeInformation_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} DataModel Navigation xAODBTagging xAODCaloEvent xAODEgamma xAODHIEvent xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigL1Calo xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODTrigRinger xAODTrigger TestTools TrigStorageDefinitions )

atlas_add_test( ListMap_test
                SOURCES
                test/ListMap_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} DataModel Navigation xAODBTagging xAODCaloEvent xAODEgamma xAODHIEvent xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigL1Calo xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODTrigRinger xAODTrigger TestTools TrigStorageDefinitions )

atlas_add_test( NewEDMMap_test
                SOURCES
                test/NewEDMMap_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} DataModel Navigation xAODBTagging xAODCaloEvent xAODEgamma xAODHIEvent xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigL1Calo xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODTrigRinger xAODTrigger TestTools TrigStorageDefinitions )

atlas_add_test( Basic_test
                SOURCES
                test/Basic_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} DataModel Navigation xAODBTagging xAODCaloEvent xAODEgamma xAODHIEvent xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigL1Calo xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODTrigRinger xAODTrigger TestTools TrigStorageDefinitions )

