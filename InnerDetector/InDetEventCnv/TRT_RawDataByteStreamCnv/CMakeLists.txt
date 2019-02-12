################################################################################
# Package: TRT_RawDataByteStreamCnv
################################################################################

# Declare the package name:
atlas_subdir( TRT_RawDataByteStreamCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          Event/ByteStreamData
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamCnvSvcBase
                          DetectorDescription/IRegionSelector
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/TRT_Cabling
                          Tools/PathResolver
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( TRT_RawDataByteStreamCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${COOL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps Identifier ByteStreamData ByteStreamData_test InDetRawData StoreGateLib SGtests AthenaPoolUtilities ByteStreamCnvSvcBaseLib GaudiKernel TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry PathResolver IRegionSelector TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( TRT_RawDataByteStreamCnv )
atlas_install_runtime( share/*.dat )

