# $Id: CMakeLists.txt 780947 2016-10-28 10:01:38Z salderwe $
################################################################################
# Package: L1TopoSimulation
################################################################################

# Declare the package name:
atlas_subdir( L1TopoSimulation )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
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
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib
   TrigInterfacesLib TrigT1CaloEventLib TrigT1CaloUtilsLib
   AthenaBaseComps GaudiKernel TrigConfBase TrigConfL1Data L1TopoConfig
   L1TopoCoreSim L1TopoEvent L1TopoInterfaces L1TopoAlgorithms TrigT1Interfaces
   TrigT1Result TrigConfInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
