################################################################################
# Package: ALFA_RawDataByteStreamCnv
################################################################################

# Declare the package name:
atlas_subdir( ALFA_RawDataByteStreamCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          ForwardDetectors/ALFA/ALFA_RawEv
                          GaudiKernel
                          PRIVATE
                          Event/xAOD/xAODEventInfo)

# External dependencies:
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_component( ALFA_RawDataByteStreamCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps DataModel StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test ALFA_RawEv GaudiKernel xAODEventInfo)

# Install files from the package:
atlas_install_headers( ALFA_RawDataByteStreamCnv )

