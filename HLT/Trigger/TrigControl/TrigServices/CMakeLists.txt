################################################################################
# Package: TrigServices
################################################################################

# Declare the package name:
atlas_subdir( TrigServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamCnvSvc
                          Event/ByteStreamData
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODCnvInterfaces
                          GaudiKernel
                          HLT/Trigger/TrigControl/TrigKernel
                          Trigger/TrigDataAccess/TrigDataAccessMonitoring
                          Trigger/TrigDataAccess/TrigROBDataProviderSvc
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigSteer/TrigOutputHandling )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq COMPONENTS omniORB4 omnithread ipc is owl )
find_package( tdaq-common COMPONENTS CTPfragment eformat eformat_write hltinterface )

# Component(s) in the package:
atlas_add_library( TrigServicesLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigServices
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities ByteStreamCnvSvcLib ByteStreamData EventInfo xAODEventInfo GaudiKernel TrigKernel TrigSteeringEvent StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData_test TrigDataAccessMonitoringLib TrigROBDataProviderSvcLib TrigOutputHandlingLib )

atlas_add_component( TrigServices
                     src/components/*.cxx
                     LINK_LIBRARIES TrigServicesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
