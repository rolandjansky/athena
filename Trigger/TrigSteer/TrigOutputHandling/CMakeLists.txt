################################################################################
# Package: TrigOutputHandling
################################################################################

# Declare the package name:
atlas_subdir( TrigOutputHandling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Control/AthViews
                          Control/RootUtils
                          Trigger/TrigEvent/TrigSteeringEvent
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigEgamma
			  Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTracking
                          Trigger/TrigDataAccess/TrigSerializeResult
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTrigMissingET
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTrigBphys
                          Event/xAOD/xAODJet
			  Event/xAOD/xAODBTagging
                          Trigger/TrigSteer/DecisionHandling
                          Control/AthenaMonitoringKernel
                          Trigger/TrigMonitoring/TrigCostMonitorMT
                          Trigger/TrigAlgorithms/TrigPartialEventBuilding
                          Trigger/TrigDataAccess/TrigSerializeTP
                          Trigger/TrigConfiguration/TrigConfData
                          Control/AthContainersRoot)


find_package( tdaq-common COMPONENTS eformat )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigOutputHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigOutputHandling
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}  GaudiKernel AthViews AthenaBaseComps RootUtils TrigSteeringEvent TrigSerializeResultLib TrigConfData
                   xAODTrigCalo xAODTrigEgamma xAODEgamma xAODBTagging xAODTrigger xAODTracking xAODTrigMuon xAODMuon xAODTau xAODTrigBphys xAODTrigMissingET DecisionHandlingLib AthenaMonitoringKernelLib TrigPartialEventBuildingLib TrigSerializeTPLib AthContainersRoot )

atlas_add_component( TrigOutputHandling
                     src/components/*.cxx
                     LINK_LIBRARIES TrigOutputHandlingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( void_record_test
      SOURCES test/void_record_test.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} xAODTrigger
      AthLinks AthenaKernel StoreGateLib GaudiKernel TestTools xAODCore
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 300
      )


atlas_add_test( serial_deserial_test
      SOURCES test/serial_deserial_test.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} xAODTrigEgamma xAODEgamma TrigSteeringEvent
            AthLinks AthenaKernel StoreGateLib GaudiKernel TestTools xAODCore TrigOutputHandlingLib
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 300
      )

atlas_add_test( schema_evolution_test
      SOURCES test/schema_evolution_test.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} xAODTrigEgamma xAODEgamma
            AthLinks AthenaKernel StoreGateLib GaudiKernel TestTools xAODCore TrigOutputHandlingLib
      ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 300
      )
