################################################################################
# Package: L1TopoSimulation
################################################################################
# Author: Joerg Stelzer
################################################################################

# Declare the package name:
atlas_subdir( L1TopoSimulation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/L1Topo/L1TopoConfig
                          Trigger/TrigT1/L1Topo/L1TopoCoreSim
                          Trigger/TrigT1/L1Topo/L1TopoEvent
                          Trigger/TrigT1/L1Topo/L1TopoInterfaces
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloUtils
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
set( CMAKE_MODULE_LINKER_FLAGS    "" )

# Component(s) in the package:
atlas_add_library( L1TopoSimulationLib
                   src/*.cxx
                   PUBLIC_HEADERS L1TopoSimulation
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaMonitoringLib TrigInterfacesLib TrigT1CaloEventLib TrigT1CaloUtilsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrigConfBase TrigConfL1Data L1TopoConfig L1TopoCoreSim L1TopoEvent L1TopoInterfaces L1TopoAlgorithms TrigT1Interfaces TrigT1Result )

atlas_add_component( L1TopoSimulation
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib GaudiKernel TrigConfBase TrigConfL1Data TrigInterfacesLib L1TopoConfig L1TopoCoreSim L1TopoEvent L1TopoInterfaces L1TopoAlgorithms TrigT1CaloEventLib TrigT1CaloUtilsLib TrigT1Interfaces TrigT1Result L1TopoSimulationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

