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
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

atlas_add_library( SCT_RawDataByteStreamCnvLib
                   SCT_RawDataByteStreamCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS SCT_RawDataByteStreamCnv
                   LINK_LIBRARIES GaudiKernel InDetRawData ByteStreamCnvSvcBaseLib ByteStreamData InDetByteStreamErrors )

# Component(s) in the package:
atlas_add_component( SCT_RawDataByteStreamCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} SCT_RawDataByteStreamCnvLib ByteStreamData_test AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier xAODEventInfo InDetIdentifier InDetReadoutGeometry SCT_ReadoutGeometry SCT_CablingLib TrigSteeringEvent SCT_ConditionsData SCT_ConditionsToolsLib )

# Run tests:
atlas_add_test( TestSCTDecode
                SCRIPT athena.py --threads=5 SCT_RawDataByteStreamCnv/testSCTDecode.py
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=5 )
atlas_add_test( TestSCTEncodeSerial
                SCRIPT athena.py SCT_RawDataByteStreamCnv/testSCTEncode.py
                PROPERTIES TIMEOUT 300 )
atlas_add_test( TestSCTEncode
                SCRIPT athena.py --threads=1 SCT_RawDataByteStreamCnv/testSCTEncode.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=1 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
