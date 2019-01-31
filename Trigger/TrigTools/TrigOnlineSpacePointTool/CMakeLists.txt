################################################################################
# Package: TrigOnlineSpacePointTool
################################################################################

# Declare the package name:
atlas_subdir( TrigOnlineSpacePointTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          Event/ByteStreamData
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
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
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Event/ByteStreamCnvSvcBase
                          InnerDetector/InDetEventCnv/SCT_RawDataByteStreamCnv
                          Tracking/TrkEvent/TrkSpacePoint
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigOnlineSpacePointTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps IRegionSelector InDetByteStreamErrors ByteStreamData ByteStreamData_test GaudiKernel InDetByteStreamErrors InDetIdentifier InDetReadoutGeometry SCT_CablingLib InDetRawData InDetPrepRawData SiClusterizationToolLib TrkPrepRawData TrigInDetEvent TrigSteeringEvent AtlasDetDescr ByteStreamCnvSvcBaseLib SCT_ConditionsData TrkSpacePoint TrigTimeAlgsLib PixelCablingLib )


# Install files from the package:
atlas_install_headers( TrigOnlineSpacePointTool )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

