################################################################################
# Package: TrigOnlineMonitor
################################################################################

# Declare the package name:
atlas_subdir( TrigOnlineMonitor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/ByteStreamData
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Event/ByteStreamCnvSvcBase
                          Event/EventInfo
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigDataAccess/TrigROBDataProviderSvc
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigMonitoring/TrigMonitorBase
                          Trigger/TrigSteer/TrigSteering
                          Trigger/TrigT1/L1Topo/L1TopoConfig
                          Trigger/TrigT1/L1Topo/L1TopoRDO
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result
                          Trigger/TrigT1/TrigT1ResultByteStream )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )


# Component(s) in the package:
atlas_add_component( TrigOnlineMonitor
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps ByteStreamData ByteStreamData_test GaudiKernel AthenaKernel ByteStreamCnvSvcBaseLib EventInfo TrigConfL1Data TrigROBDataProviderSvcLib TrigSteeringEvent TrigMonitorBaseLib TrigSteeringLib L1TopoConfig L1TopoRDO TrigT1Interfaces TrigT1Result TrigT1ResultByteStreamLib )

# Install files from the package:
atlas_install_headers( TrigOnlineMonitor )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/plotL1TopoROBMonHist.py )

