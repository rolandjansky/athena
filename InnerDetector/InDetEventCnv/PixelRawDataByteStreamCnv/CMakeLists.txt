################################################################################
# Package: PixelRawDataByteStreamCnv
################################################################################

# Declare the package name:
atlas_subdir( PixelRawDataByteStreamCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamData
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelCabling
			  Event/xAOD/xAODEventInfo)

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( PixelRawDataByteStreamCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamData ByteStreamData_test GaudiKernel InDetRawData AthenaBaseComps StoreGateLib SGtests ByteStreamCnvSvcBaseLib InDetIdentifier InDetReadoutGeometry )

# Install files from the package:
atlas_install_headers( PixelRawDataByteStreamCnv )

