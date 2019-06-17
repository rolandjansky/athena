################################################################################
# Package: TrigL2MissingET
################################################################################

# Declare the package name:
atlas_subdir( TrigL2MissingET )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloIdentifier
                          DetectorDescription/IRegionSelector
                          Event/xAOD/xAODTrigMissingET
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArCabling
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigL2MissingET
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier IRegionSelector xAODTrigMissingET GaudiKernel LArIdentifier LArRecEvent LArCablingLib TrigSteeringEvent TrigInterfacesLib CxxUtils xAODEventInfo TrigT2CaloCommonLib TrigCaloEvent TrigMissingEtEvent TrigNavigationLib TrigT1Interfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

