################################################################################
# Package: TrigServices
################################################################################

# Declare the package name:
atlas_subdir( TrigServices )

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
                     AthenaBaseComps AthenaInterprocess AthenaKernel AthenaMonitoringKernelLib AthenaPoolUtilities
                     ByteStreamCnvSvcBaseLib ByteStreamData EventInfoUtils GaudiKernel RDBAccessSvcLib StoreGateLib TrigKernel
                     TrigOutputHandlingLib TrigSteeringEvent xAODEventInfo xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( test_TrigMessageSvc
                SOURCES test/test_TrigMessageSvc.cxx src/TrigMessageSvc.cxx
                INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} GaudiKernel TestTools
                AthenaKernel AthenaInterprocess AthenaMonitoringKernelLib
                POST_EXEC_SCRIPT nopost.sh
)
