################################################################################
# Package: TrigMonitoringEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrigMonitoringEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigEvent/TrigMonitoringEvent
                          PRIVATE
                          Control/AthenaKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigMonitoringEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigMonitoringEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMonitoringEvent AthenaKernel )

atlas_add_dictionary( TrigMonitoringEventTPCnvDict
                      TrigMonitoringEventTPCnv/TrigMonitoringEventTPCnvDict.h
                      TrigMonitoringEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMonitoringEvent AthenaKernel TrigMonitoringEventTPCnv )

