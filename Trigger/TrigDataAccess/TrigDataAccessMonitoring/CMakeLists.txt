################################################################################
# Package: TrigDataAccessMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigDataAccessMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigDataAccessMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigDataAccessMonitoring
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} DataModel SGTools ByteStreamData GaudiKernel ByteStreamCnvSvcBaseLib ByteStreamData_test StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_component( TrigDataAccessMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} DataModel SGTools ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel TestTools StoreGateLib SGtests TrigDataAccessMonitoringLib )

atlas_add_test( Methods_test
                SOURCES
                test/Methods_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} DataModel SGTools ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel TestTools StoreGateLib SGtests TrigDataAccessMonitoringLib
                EXTRA_PATTERNS ".*" )

# Install files from the package:
atlas_install_joboptions( share/*.py )

