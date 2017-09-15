################################################################################
# Package: TrigMonitoringEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigMonitoringEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/AthenaKernel
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigMonitoringEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigMonitoringEvent
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} DataModel SGTools TrigSteeringEvent TrigSerializeCnvSvcLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel )

atlas_add_sercnv_library( TrigMonitoringEventSerCnv
                          FILES TrigMonitoringEvent/TrigMonEvent.h TrigMonitoringEvent/TrigMonEventCollection.h TrigMonitoringEvent/TrigMonConfig.h TrigMonitoringEvent/TrigMonConfigCollection.h
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} DataModel SGTools TrigSteeringEvent AthenaKernel TrigSerializeCnvSvcLib TrigNavigationLib TrigMonitoringEvent )

atlas_add_dictionary( TrigMonitoringEventDict
                      TrigMonitoringEvent/TrigMonitoringEventDict.h
                      TrigMonitoringEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} DataModel SGTools TrigSteeringEvent AthenaKernel TrigSerializeCnvSvcLib TrigNavigationLib TrigMonitoringEvent )

