# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoSimulation )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Do not use --as-needed for the linking:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( L1TopoSimulationLib
   L1TopoSimulation/*.h
   INTERFACE
   PUBLIC_HEADERS L1TopoSimulation
   LINK_LIBRARIES GaudiKernel )

atlas_add_component( L1TopoSimulation
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib L1TopoConfig L1TopoCoreSim L1TopoEvent L1TopoInterfaces L1TopoRDO L1TopoSimulationLib StoreGateLib TrigConfBase TrigConfInterfaces TrigConfL1Data TrigInterfacesLib TrigT1CaloEventLib TrigT1CaloUtilsLib TrigT1Interfaces TrigT1Result xAODTrigCalo xAODTrigger )

atlas_add_component( L1TopoSimulationTest
   src/test/*.h src/test/*.cxx src/test/components/*.cxx src/AthenaL1TopoHistSvc.h src/AthenaL1TopoHistSvc.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib GaudiKernel L1TopoConfig L1TopoCoreSim L1TopoEvent L1TopoInterfaces StoreGateLib TrigConfBase TrigConfInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
