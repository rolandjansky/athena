# $Id: CMakeLists.txt 780092 2016-10-24 15:34:49Z krasznaa $
################################################################################
# Package: TrigSteerMonitor
################################################################################

# Declare the package name:
atlas_subdir( TrigSteerMonitor )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   GaudiKernel
   Control/AthenaKernel
   Control/AthenaMonitoring
   Event/EventInfo
   Trigger/TrigConfiguration/TrigConfInterfaces
   Trigger/TrigDataAccess/TrigDataAccessMonitoring
   Trigger/TrigMonitoring/TrigMonitorBase
   Trigger/TrigSteer/TrigSteering
   Control/StoreGate
   Event/xAOD/xAODEventInfo
   MagneticField/MagFieldInterfaces
   LumiBlock/LumiBlockComps
   Trigger/TrigConfiguration/TrigConfHLTData
   Trigger/TrigConfiguration/TrigConfL1Data
   Trigger/TrigEvent/TrigNavigation
   Trigger/TrigEvent/TrigSteeringEvent
   Trigger/TrigSteer/TrigInterfaces
   Trigger/TrigT1/TrigT1Interfaces
   Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigSteerMonitor
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES}
   ${TDAQ-COMMON_LIBRARIES} AthenaKernel AthenaMonitoringLib EventInfo
   GaudiKernel TrigDataAccessMonitoringLib TrigMonitorBaseLib TrigSteeringLib
   StoreGateLib MagFieldInterfaces LumiBlockCompsLib TrigConfHLTData
   TrigConfL1Data TrigNavigationLib TrigSteeringEvent TrigInterfacesLib
   TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
