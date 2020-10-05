# $Id: CMakeLists.txt 756928 2016-06-22 13:33:12Z krasznaa $
################################################################################
# Package: PixelRawDataByteStreamCnv
################################################################################

# Declare the package name:
atlas_subdir( PixelRawDataByteStreamCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/ByteStreamData
   GaudiKernel
   InnerDetector/InDetRawEvent/InDetRawData
   PRIVATE
   Control/AthenaBaseComps
   Control/StoreGate
   Control/AthenaKernel
   Control/AthContainers
   DetectorDescription/IRegionSelector
   Event/ByteStreamCnvSvcBase
   InnerDetector/InDetConditions/PixelConditionsTools
   InnerDetector/InDetConditions/InDetByteStreamErrors
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelReadoutGeometry
   InnerDetector/InDetDetDescr/PixelCabling
   InnerDetector/InDetConditions/PixelConditionsData
	 Event/xAOD/xAODEventInfo
   Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_library( PixelRawDataByteStreamCnvLib
   PixelRawDataByteStreamCnv/*.h
   INTERFACE
   PUBLIC_HEADERS PixelRawDataByteStreamCnv
   LINK_LIBRARIES GaudiKernel ByteStreamData InDetRawData )

atlas_add_component( PixelRawDataByteStreamCnv
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamData
   GaudiKernel InDetRawData AthenaBaseComps AthContainers  StoreGateLib
   ByteStreamCnvSvcBaseLib InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry IRegionSelector
   xAODEventInfo TrigSteeringEvent InDetByteStreamErrors PixelConditionsData PixelRawDataByteStreamCnvLib PixelCablingLib PixelConditionsToolsLib )

