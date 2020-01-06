################################################################################
# Package: TrigServices
################################################################################

# Declare the package name:
atlas_subdir( TrigServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaInterprocess
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          Control/CxxUtils
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          Event/EventInfoUtils
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          HLT/Trigger/TrigControl/TrigKernel
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigOutputHandling )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq COMPONENTS omniORB4 omnithread ipc is owl )
find_package( tdaq-common COMPONENTS CTPfragment eformat eformat_write hltinterface )
find_package( TBB )

# Component(s) in the package:
atlas_add_component( TrigServices
                     src/*.h src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS
                     ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES
                     ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} ${CORAL_LIBRARIES}
                     AthenaBaseComps AthenaInterprocess AthenaKernel AthenaMonitoringLib AthenaPoolUtilities
                     ByteStreamCnvSvcBaseLib ByteStreamData EventInfoUtils GaudiKernel StoreGateLib TrigKernel
                     TrigOutputHandlingLib TrigSteeringEvent xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
