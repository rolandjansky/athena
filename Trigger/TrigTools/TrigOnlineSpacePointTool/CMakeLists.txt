################################################################################
# Package: TrigOnlineSpacePointTool
################################################################################

# Declare the package name:
atlas_subdir( TrigOnlineSpacePointTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          Event/ByteStreamData
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelCabling
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          InnerDetector/InDetEventCnv/PixelRawDataByteStreamCnv
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/SiClusterizationTool
                          Tracking/TrkEvent/TrkPrepRawData
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigSiSpacePointTool
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Event/ByteStreamCnvSvcBase
                          InnerDetector/InDetEventCnv/SCT_RawDataByteStreamCnv
                          Tracking/TrkEvent/TrkSpacePoint
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_component( TrigOnlineSpacePointTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps IRegionSelector InDetByteStreamErrors ByteStreamData ByteStreamData_test GaudiKernel InDetByteStreamErrors InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry SCT_CablingLib InDetRawData InDetPrepRawData SiClusterizationToolLib TrkPrepRawData TrigInDetEvent TrigSteeringEvent AtlasDetDescr ByteStreamCnvSvcBaseLib SCT_ConditionsData TrkSpacePoint TrigTimeAlgsLib PixelCablingLib )

