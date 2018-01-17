################################################################################
# Package: TrigCaloDataAccessSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigCaloDataAccessSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          LArCalorimeter/LArRecEvent

                          PRIVATE
                     	   Calorimeter/CaloEvent
                           Calorimeter/CaloGeoHelpers
                           Calorimeter/CaloIdentifier
                           Calorimeter/CaloInterface
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
                           LArCalorimeter/LArCabling
                           TileCalorimeter/TileEvent
                           TileCalorimeter/TileSvc/TileByteStream
                           Trigger/TrigEvent/TrigSteeringEvent
                           Trigger/TrigSteer/TrigInterfaces
                           Trigger/TrigTools/TrigTimeAlgs
			   Trigger/TrigAlgorithms/TrigT2CaloCommon
			   #
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamData
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArRecUtils
                          Trigger/TrigT1/TrigT1Interfaces
			  AtlasTest/TestTools
			  Control/AthenaMonitoring )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigCaloDataAccessSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigCaloDataAccessSvc
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel IRegionSelector Identifier EventInfo xAODTrigCalo ZdcEvent GaudiKernel LArRecEvent TileEvent TrigSteeringEvent StoreGateLib SGtests ByteStreamCnvSvcBaseLib ZdcByteStreamLib ZdcRecLib LArByteStreamLib LArRawUtilsLib LArCablingLib TileByteStreamLib TrigInterfacesLib TrigTimeAlgsLib CaloDetDescrLib CaloUtilsLib ByteStreamData_test LArRecUtilsLib TrigT2CaloCommonLib
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities ByteStreamData LArIdentifier LArRecConditions TrigT1Interfaces )

atlas_add_component( TrigCaloDataAccessSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests IRegionSelector Identifier ByteStreamCnvSvcBaseLib EventInfo xAODTrigCalo ZdcByteStreamLib ZdcEvent ZdcRecLib GaudiKernel LArByteStreamLib LArRawUtilsLib LArRecEvent LArCablingLib TileEvent TileByteStreamLib TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib CaloDetDescrLib CaloUtilsLib AthenaPoolUtilities ByteStreamData ByteStreamData_test LArIdentifier LArRecConditions LArRecUtilsLib TrigT1Interfaces TrigT2CaloCommonLib  TrigCaloDataAccessSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( TestService 
		SCRIPT test/test_dataaccess.sh
		PROPERTIES TIMEOUT 300)
