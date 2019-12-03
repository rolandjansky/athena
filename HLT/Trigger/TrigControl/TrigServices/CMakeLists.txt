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
                          Event/EventInfoUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODCnvInterfaces
                          GaudiKernel
                          HLT/Trigger/TrigControl/TrigKernel
                          Trigger/TrigDataAccess/TrigDataAccessMonitoring
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaInterprocess
                          Control/CxxUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigSteer/TrigOutputHandling )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq COMPONENTS omniORB4 omnithread ipc is owl )
find_package( tdaq-common COMPONENTS CTPfragment eformat eformat_write hltinterface )
find_package( TBB )

# Component(s) in the package:
atlas_add_library( TrigServicesLib
                   NO_PUBLIC_HEADERS
                   src/*.cxx
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities ByteStreamCnvSvcLib ByteStreamData EventInfoUtils xAODEventInfo GaudiKernel TrigKernel TrigSteeringEvent StoreGateLib ByteStreamCnvSvcBaseLib ByteStreamData_test TrigDataAccessMonitoringLib TrigOutputHandlingLib
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaInterprocess )

atlas_add_component( TrigServices
                     src/components/*.cxx
                     LINK_LIBRARIES TrigServicesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
