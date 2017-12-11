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

# - commented out for the meoment, seem like CI is not adding just compiled packages to the pythonpath
#atlas_add_test( ROBDataProviderSvcMT		 
#		SCRIPT test/test_ROBDataProviderSvcMT.sh		
#		TIMEOUT 1200 )
#

# Install files from the package:
atlas_install_joboptions( share/*.py )

