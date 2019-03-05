################################################################################
# Package: TrigT2CaloCommon
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloCommon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          Event/EventInfo
                          Event/xAOD/xAODTrigCalo
                          ForwardDetectors/ZDC/ZdcCnv/ZdcByteStream
                          ForwardDetectors/ZDC/ZdcEvent
                          ForwardDetectors/ZDC/ZdcRec
                          GaudiKernel
                          LArCalorimeter/LArCnv/LArByteStream
                          LArCalorimeter/LArRawUtils
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArCabling
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileSvc/TileByteStream
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamData
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArRecUtils
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigT2CaloCommonLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloCommon
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel IRegionSelector Identifier EventInfo xAODTrigCalo ZdcEvent GaudiKernel LArRecEvent TileEvent TrigSteeringEvent StoreGateLib SGtests ByteStreamCnvSvcBaseLib ZdcByteStreamLib ZdcRecLib LArByteStreamLib LArRawUtilsLib LArCablingLib TileByteStreamLib TrigInterfacesLib TrigTimeAlgsLib CaloDetDescrLib CaloUtilsLib ByteStreamData_test LArRecUtilsLib
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities ByteStreamData LArIdentifier LArRecConditions TrigT1Interfaces )

atlas_add_component( TrigT2CaloCommon
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests IRegionSelector Identifier ByteStreamCnvSvcBaseLib EventInfo xAODTrigCalo ZdcByteStreamLib ZdcEvent ZdcRecLib GaudiKernel LArByteStreamLib LArRawUtilsLib LArRecEvent LArCablingLib TileEvent TileByteStreamLib TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib CaloDetDescrLib CaloUtilsLib AthenaPoolUtilities ByteStreamData ByteStreamData_test LArIdentifier LArRecConditions LArRecUtilsLib TrigT1Interfaces TrigT2CaloCommonLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( TestService 
		SCRIPT test/test_dataaccess.sh
		POST_EXEC_SCRIPT nopost.sh
		PROPERTIES TIMEOUT 1200)

atlas_add_test( TestServiceNewJO 
		SCRIPT test/test_dataaccessNewJO.sh
		POST_EXEC_SCRIPT nopost.sh
		PROPERTIES TIMEOUT 1200)



