################################################################################
# Package: L1Decoder
################################################################################

# Declare the package name:
atlas_subdir( L1Decoder )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          GaudiKernel
                          PRIVATE
                          Control/AthViews
                          Control/AthenaBaseComps
                          Control/AthenaMonitoringKernel
                          Event/xAOD/xAODTrigger
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigSteer/TrigCompositeUtils
                          Trigger/TrigTools/TrigTimeAlgs
                          Trigger/TrigMonitoring/TrigCostMonitorMT )

find_package( CLHEP )
find_package( nlohmann_json )


# Component(s) in the package:
atlas_add_library( L1DecoderLib
                   src/*.cxx
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PUBLIC_HEADERS L1Decoder
                   LINK_LIBRARIES GaudiKernel AthViews AthenaBaseComps AthenaMonitoringKernelLib 
                   xAODTrigger TrigConfHLTData TrigConfData TrigConfL1Data
                   TrigSteeringEvent TrigT1Interfaces TrigT1Result DecisionHandlingLib TrigCompositeUtilsLib
                   LINK_LIBRARIES ${CLHEP_LIBRARIES}
                   )
                   
atlas_add_component( L1Decoder
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES L1DecoderLib ${CLHEP_LIBRARIES}
                     )

atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Install files from the package:
atlas_install_headers( L1Decoder )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/test* )

atlas_add_test( L1DecoderNJOTest SCRIPT python -m L1Decoder.L1DecoderConfig
                PROPERTIES TIMEOUT 300 )

atlas_add_test( L1DecoderTest
                SCRIPT test/test_l1decoder.sh
                PROPERTIES TIMEOUT 1000               
              )

