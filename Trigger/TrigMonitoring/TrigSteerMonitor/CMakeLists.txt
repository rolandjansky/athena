# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSteerMonitor )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigSteerMonitor
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaInterprocess AthenaKernel AthenaMonitoringKernelLib AthenaMonitoringLib EventInfo GaudiKernel StoreGateLib TrigCompositeUtilsLib TrigConfData TrigConfHLTData TrigConfInterfaces TrigConfL1Data TrigDataAccessMonitoringLib TrigInterfacesLib TrigMonitorBaseLib TrigNavigationLib TrigSteeringEvent TrigSteeringLib TrigT1Interfaces TrigT1Result xAODEventInfo xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py )
