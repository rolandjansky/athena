################################################################################
# Package: L1TopoByteStream
################################################################################

# Declare the package name:
atlas_subdir( L1TopoByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          Event/xAOD/xAODTrigL1Calo
                          Trigger/TrigT1/L1Topo/L1TopoRDO )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write )

# Component(s) in the package:
atlas_add_component( L1TopoByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test xAODTrigL1Calo L1TopoRDO )

# Install files from the package:
atlas_install_headers( L1TopoByteStream )
atlas_install_joboptions( share/*.py )

