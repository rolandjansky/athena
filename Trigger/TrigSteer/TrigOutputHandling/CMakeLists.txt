################################################################################
# Package: TrigOutputHandling
################################################################################

# Declare the package name:
atlas_subdir( TrigOutputHandling )

find_package( tdaq-common COMPONENTS eformat )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigOutputHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigOutputHandling
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel AthViews AthenaBaseComps RootUtils TrigSteeringEvent TrigSerializeResultLib TrigConfData xAODTrigCalo xAODTrigRinger xAODTrigEgamma xAODEgamma xAODBTagging xAODTrigger xAODTracking xAODTrigMuon xAODMuon xAODTau xAODTrigBphys xAODTrigMissingET DecisionHandlingLib AthenaMonitoringKernelLib TrigPartialEventBuildingLib TrigSerializeTPLib AthContainersRoot TrigCompositeUtilsLib TrigCostMonitorMTLib
		   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver )


atlas_add_component( TrigOutputHandling
                     src/components/*.cxx
                     LINK_LIBRARIES TrigOutputHandlingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( void_record_test
      SOURCES test/void_record_test.cxx
      LINK_LIBRARIES TrigOutputHandlingLib
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 300
      )

atlas_add_test( serial_deserial_test
      SOURCES test/serial_deserial_test.cxx
      LINK_LIBRARIES TrigOutputHandlingLib
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 300
      )

atlas_add_test( schema_evolution_test
      SOURCES test/schema_evolution_test.cxx
      LINK_LIBRARIES TrigOutputHandlingLib
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 300
      )
