################################################################################
# Package: BCM_RawDataByteStreamCnv
################################################################################

# Declare the package name:
atlas_subdir( BCM_RawDataByteStreamCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          PRIVATE
                          Control/StoreGate )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( BCM_RawDataByteStreamCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel InDetBCM_RawData StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( BCM_RawDataByteStreamCnv )

