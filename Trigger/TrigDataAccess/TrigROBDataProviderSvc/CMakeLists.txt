################################################################################
# Package: TrigROBDataProviderSvc
################################################################################

# Declare the package name:
atlas_subdir( TrigROBDataProviderSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamData
                          GaudiKernel
                          Trigger/TrigEvent/TrigSteeringEvent	
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Trigger/TrigDataAccess/TrigDataAccessMonitoring
                          Trigger/TrigMonitoring/TrigMonitorBase )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigROBDataProviderSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigROBDataProviderSvc
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamData GaudiKernel ByteStreamData_test StoreGateLib SGtests ByteStreamCnvSvcBaseLib TrigDataAccessMonitoringLib TrigMonitorBaseLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils )

atlas_add_component( TrigROBDataProviderSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ByteStreamData ByteStreamData_test GaudiKernel CxxUtils StoreGateLib SGtests ByteStreamCnvSvcBaseLib TrigDataAccessMonitoringLib TrigMonitorBaseLib TrigROBDataProviderSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( python/scripts/*.py )

