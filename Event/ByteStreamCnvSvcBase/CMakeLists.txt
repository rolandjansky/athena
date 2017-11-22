################################################################################
# Package: ByteStreamCnvSvcBase
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamCnvSvcBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/StoreGate
                          Event/ByteStreamData
                          GaudiKernel
                          PRIVATE
                          Control/SGTools
			  AtlasTest/TestTools )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat eformat_write )

# Component(s) in the package:
atlas_add_library( ByteStreamCnvSvcBaseLib
                   src/*.cxx
                   PUBLIC_HEADERS ByteStreamCnvSvcBase
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamData GaudiKernel StoreGateLib SGtests ByteStreamData_test
                   PRIVATE_LINK_LIBRARIES SGTools )

atlas_add_component( ByteStreamCnvSvcBase
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests ByteStreamData ByteStreamData_test GaudiKernel SGTools ByteStreamCnvSvcBaseLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

