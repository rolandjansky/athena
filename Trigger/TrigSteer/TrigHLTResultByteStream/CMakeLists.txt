################################################################################
# Package: TrigHLTResultByteStream
################################################################################

# Declare the package name:
atlas_subdir( TrigHLTResultByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write )

# Component(s) in the package:
atlas_add_library( TrigHLTResultByteStreamLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigHLTResultByteStream
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps SGTools ByteStreamData GaudiKernel TrigSteeringEvent StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData_test )

atlas_add_component( TrigHLTResultByteStream
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps SGTools StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel TrigSteeringEvent TrigHLTResultByteStreamLib )

