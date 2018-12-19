################################################################################
# Package: TrigT1CaloSim
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloSim )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloDetDescr
   Calorimeter/CaloEvent
   Calorimeter/CaloIdentifier
   Calorimeter/CaloTTDetDescr
   Control/AthenaBaseComps
   Control/AthContainers
   Control/PileUpTools
   Control/StoreGate
   Event/xAOD/xAODTrigL1Calo
   GaudiKernel
   LArCalorimeter/LArRawEvent
   LArCalorimeter/LArCabling
   TileCalorimeter/TileConditions
   TileCalorimeter/TileEvent
   Trigger/TrigConfiguration/TrigConfInterfaces
   Trigger/TrigConfiguration/TrigConfL1Data
   Trigger/TrigT1/TrigT1CaloCalibConditions
   Trigger/TrigT1/TrigT1CaloCalibToolInterfaces
   Trigger/TrigT1/TrigT1CaloEvent
   Trigger/TrigT1/TrigT1CaloMappingToolInterfaces
   Trigger/TrigT1/TrigT1CaloToolInterfaces
   Trigger/TrigT1/TrigT1CaloUtils
   Trigger/TrigT1/TrigT1Interfaces
   Trigger/TrigT1/TrigT1CaloByteStream
   PRIVATE
   Control/AthenaKernel
   Control/CxxUtils
   Event/EventInfo
   Event/EventInfoUtils
   Event/xAOD/xAODEventInfo
   LumiBlock/LumiBlockComps
   Tools/PathResolver
   Trigger/TrigT1/TrigT1CaloCondSvc )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigT1CaloSimLib
   TrigT1CaloSim/*.h src/*.cxx
   PUBLIC_HEADERS TrigT1CaloSim
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier CaloTTDetDescr
   AthenaBaseComps AthContainers xAODTrigL1Calo GaudiKernel LArRawEvent
   TileEvent TrigConfL1Data TrigT1CaloCalibConditions 
   TrigT1CaloCalibToolInterfaces TrigT1CaloToolInterfaces TrigT1Interfaces
   CaloDetDescrLib PileUpToolsLib StoreGateLib SGtests LArCablingLib
   TileConditionsLib TrigT1CaloEventLib TrigT1CaloUtilsLib LumiBlockCompsLib
   PRIVATE_LINK_LIBRARIES AthenaKernel CxxUtils EventInfo
   xAODEventInfo EventInfoUtils
   PathResolver TrigT1CaloCondSvcLib )

atlas_add_component( TrigT1CaloSim
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel TrigT1CaloSimLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
