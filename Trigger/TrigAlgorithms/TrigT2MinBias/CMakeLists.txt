################################################################################
# Package: TrigT2MinBias
################################################################################

# Declare the package name:
atlas_subdir( TrigT2MinBias )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Event/xAOD/xAODTrigMinBias
                          ForwardDetectors/ZDC/ZdcEvent
                          GaudiKernel
                          TileCalorimeter/TileEvent
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          DetectorDescription/IRegionSelector
                          Event/xAOD/xAODEventInfo
                          ForwardDetectors/ZDC/ZdcConditions
                          ForwardDetectors/ZDC/ZdcIdentifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          TileCalorimeter/TileIdentifier
                          Tracking/TrkEvent/TrkSpacePoint
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs
                          Event/xAOD/xAODTrigger )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigT2MinBias
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} xAODTrigger xAODTrigMinBias ZdcEvent GaudiKernel TileEvent TrigT2CaloCommonLib TrigCaloEvent TrigInDetEvent TrigInterfacesLib IRegionSelector xAODEventInfo ZdcConditions ZdcIdentifier InDetIdentifier InDetRawData InDetPrepRawData TileIdentifier TrkSpacePoint TrigSteeringEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
