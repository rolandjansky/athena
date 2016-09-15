################################################################################
# Package: TrigT1ResultByteStream
################################################################################

# Declare the package name:
atlas_subdir( TrigT1ResultByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/SGTools
                          Control/StoreGate
                          Event/ByteStreamCnvSvc
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/L1Topo/L1TopoRDO
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( tdaq-common COMPONENTS CTPfragment eformat )

# tag $(ttr_allow_undefined) was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TrigT1ResultByteStreamLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1ResultByteStream
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps SGTools ByteStreamData GaudiKernel TrigConfL1Data L1TopoRDO TrigT1Interfaces TrigT1Result StoreGateLib SGtests ByteStreamCnvSvcLib ByteStreamCnvSvcBaseLib ByteStreamData_test )

atlas_add_component( TrigT1ResultByteStream
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps SGTools StoreGateLib SGtests ByteStreamCnvSvcLib ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel TrigConfL1Data L1TopoRDO TrigT1Interfaces TrigT1Result TrigT1ResultByteStreamLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

