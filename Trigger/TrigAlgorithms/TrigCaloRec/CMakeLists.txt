################################################################################
# Package: TrigCaloRec
################################################################################

# Declare the package name:
atlas_subdir( TrigCaloRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          DetectorDescription/IRegionSelector
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloRec
                          Calorimeter/CaloUtils
                          Event/EventKernel
                          Event/NavFourMom
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODTrigL1Calo
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArCabling
                          Trigger/TrigT1/TrigT1CaloByteStream
                          Trigger/TrigT1/TrigT1Interfaces )

# Component(s) in the package:
atlas_add_library( TrigCaloRecLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigCaloRec
                   LINK_LIBRARIES CaloEvent AthenaBaseComps AthenaKernel IRegionSelector xAODCaloEvent GaudiKernel TrigCaloEvent TrigSteeringEvent AthenaMonitoringLib TrigT2CaloCommonLib TrigInterfacesLib TrigTimeAlgsLib CaloDetDescrLib CaloRecLib CaloUtilsLib LArCablingLib
                   PRIVATE_LINK_LIBRARIES CaloGeoHelpers CaloIdentifier EventKernel NavFourMom xAODTrigCalo xAODHIEvent xAODTrigL1Calo LArIdentifier TrigT1Interfaces )

atlas_add_component( TrigCaloRec
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent AthenaBaseComps AthenaKernel AthenaMonitoringLib IRegionSelector xAODCaloEvent GaudiKernel TrigT2CaloCommonLib TrigCaloEvent TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib CaloDetDescrLib CaloGeoHelpers CaloIdentifier CaloRecLib CaloUtilsLib EventKernel NavFourMom xAODTrigCalo xAODHIEvent xAODTrigL1Calo LArIdentifier LArCablingLib TrigT1Interfaces TrigCaloRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

