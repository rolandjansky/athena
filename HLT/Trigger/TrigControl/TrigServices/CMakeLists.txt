# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigServices )

# External dependencies:
find_package( Boost COMPONENTS filesystem regex )
find_package( CORAL COMPONENTS CoralBase CoralKernel )
find_package( ROOT COMPONENTS Core Tree Hist RIO )
find_package( tdaq COMPONENTS owl )
find_package( tdaq-common COMPONENTS CTPfragment eformat hltinterface ers )
find_package( TBB )

# Component(s) in the package:
atlas_add_component( TrigServices
                     src/*.h src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS
                     ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES
                     ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${TBB_LIBRARIES}
                     AthenaBaseComps AthenaInterprocess AthenaKernel AthenaMonitoringKernelLib AthenaPoolUtilities
                     ByteStreamCnvSvcBaseLib ByteStreamData CxxUtils EventInfoUtils GaudiKernel RDBAccessSvcLib StoreGateLib TrigCostMonitorLib TrigKernel
                     TrigDataAccessMonitoringLib TrigOutputHandlingLib TrigSteeringEvent TrigSteerMonitorLib
                     TrigTimeAlgsLib xAODEventInfo xAODTrigger TrigT1Result )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( test_TrigMessageSvc
                SOURCES test/test_TrigMessageSvc.cxx src/TrigMessageSvc.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${TBB_LIBRARIES} ${TDAQ-COMMON_LIBRARIES}
                AthenaInterprocess AthenaMonitoringKernelLib CxxUtils GaudiKernel TestTools
                POST_EXEC_SCRIPT nopost.sh
)
