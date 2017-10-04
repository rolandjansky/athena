################################################################################
# Package: InDetVertexMonitoring
################################################################################

# Declare the package name:
atlas_subdir( InDetVertexMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          GaudiKernel
                          Tools/LWHists
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetVertexMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib GaudiKernel LWHists AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities xAODEventInfo xAODTracking )


