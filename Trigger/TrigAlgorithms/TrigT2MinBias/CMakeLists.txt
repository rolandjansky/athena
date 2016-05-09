################################################################################
# Package: TrigT2MinBias
################################################################################

# Declare the package name:
atlas_subdir( TrigT2MinBias )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigMinBias
                          ForwardDetectors/ZDC/ZdcEvent
                          GaudiKernel
                          TileCalorimeter/TileEvent
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          PRIVATE
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
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigT2MinBiasLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2MinBias
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} xAODTrigMinBias ZdcEvent GaudiKernel TileEvent TrigCaloEvent TrigInDetEvent TrigT2CaloCommonLib TrigInterfacesLib TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES IRegionSelector xAODEventInfo ZdcConditions ZdcIdentifier InDetIdentifier InDetRawData InDetPrepRawData TileIdentifier TrkSpacePoint TrigSteeringEvent )

atlas_add_component( TrigT2MinBias
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} xAODTrigMinBias ZdcEvent GaudiKernel TileEvent TrigT2CaloCommonLib TrigCaloEvent TrigInDetEvent TrigInterfacesLib IRegionSelector xAODEventInfo ZdcConditions ZdcIdentifier InDetIdentifier InDetRawData InDetPrepRawData TileIdentifier TrkSpacePoint TrigSteeringEvent TrigTimeAlgsLib TrigT2MinBiasLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

