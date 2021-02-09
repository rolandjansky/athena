################################################################################
# Package: SCT_RawDataByteStreamCnv
################################################################################

# Declare the package name:
atlas_subdir( SCT_RawDataByteStreamCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/SCT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( SCT_RawDataByteStreamCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel InDetRawData AthenaBaseComps StoreGateLib SGtests Identifier EventInfo xAODEventInfo InDetIdentifier InDetReadoutGeometry SCT_CablingLib TrigSteeringEvent)


# Install files from the package:
atlas_install_headers( SCT_RawDataByteStreamCnv )
atlas_install_joboptions( share/*.py )
