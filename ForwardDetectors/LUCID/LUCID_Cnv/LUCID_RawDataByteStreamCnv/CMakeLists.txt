################################################################################
# Package: LUCID_RawDataByteStreamCnv
################################################################################

# Declare the package name:
atlas_subdir( LUCID_RawDataByteStreamCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          GaudiKernel )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( LUCID_RawDataByteStreamCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test LUCID_RawEvent GaudiKernel )

# Install files from the package:
atlas_install_headers( LUCID_RawDataByteStreamCnv )
atlas_install_joboptions( share/*.py )

