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
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetConditions/SCT_ConditionsServices
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
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Event/ByteStreamCnvSvcBase
                          InnerDetector/InDetConditions/InDetBeamSpotService
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
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps IRegionSelector Identifier ByteStreamData ByteStreamData_test GaudiKernel InDetIdentifier InDetReadoutGeometry SCT_CablingLib InDetRawData InDetPrepRawData SiClusterizationToolLib TrkPrepRawData TrigInDetEvent TrigSteeringEvent AtlasDetDescr ByteStreamCnvSvcBaseLib TrkSpacePoint TrigTimeAlgsLib PixelCablingLib )

# Install files from the package:
atlas_install_headers( TrigOnlineSpacePointTool )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

