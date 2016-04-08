################################################################################
# Package: TrigL2MissingET
################################################################################

# Declare the package name:
atlas_subdir( TrigL2MissingET )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          DetectorDescription/IRegionSelector
                          Event/xAOD/xAODTrigMissingET
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArTools
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
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
atlas_add_library( TrigL2MissingETLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigL2MissingET
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier IRegionSelector xAODTrigMissingET GaudiKernel LArIdentifier LArRecEvent TrigSteeringEvent LArToolsLib TrigInterfacesLib TrigT2CaloCommonLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CxxUtils xAODEventInfo TrigCaloEvent TrigMissingEtEvent TrigT1Interfaces )

atlas_add_component( TrigL2MissingET
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier IRegionSelector xAODTrigMissingET GaudiKernel LArIdentifier LArRecEvent LArToolsLib TrigSteeringEvent TrigInterfacesLib CxxUtils xAODEventInfo TrigT2CaloCommonLib TrigCaloEvent TrigMissingEtEvent TrigNavigationLib TrigT1Interfaces TrigL2MissingETLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

