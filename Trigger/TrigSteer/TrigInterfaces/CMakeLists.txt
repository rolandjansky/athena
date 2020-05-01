# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInterfaces )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_library( TrigInterfacesLib
   TrigInterfaces/*.h src/*.cxx
   PUBLIC_HEADERS TrigInterfaces
   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES AthenaBaseComps AthContainers AthLinks GaudiKernel
   TrigSteeringEvent TrigStorageDefinitions AthenaMonitoringLib
   TrigNavigationLib
   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} TrigTimeAlgsLib
   AthenaKernel xAODTrigger )

atlas_add_test( Templates_test
   SOURCES test/Templates_test.cxx
   LINK_LIBRARIES AthContainers AthLinks GaudiKernel TrigSteeringEvent
   xAODTrigger TrigInterfacesLib
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( MonitoredAlgo_test
   SOURCES test/MonitoredAlgo_test.cxx
   LINK_LIBRARIES AthContainers TrigInterfacesLib
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( HLTCheck_test
   SOURCES test/HLTCheck_test.cxx
   LINK_LIBRARIES TestTools TrigInterfacesLib
   POST_EXEC_SCRIPT nopost.sh )
