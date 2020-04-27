################################################################################
# Package: TrigInDetMonitoringTools
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetMonitoringTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Control/AthenaMonitoringKernel
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrackingCnv
                          PRIVATE
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                           )

atlas_add_library( TrigInDetTrackMonitoringToolLib
    src/*.cxx
    PUBLIC_HEADERS TrigInDetMonitoringTools
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps  GaudiKernel AthenaMonitoringKernelLib xAODTracking xAODTrackingCnvLib 
)

atlas_add_component(
    TrigInDetTrackMonitoringTool
    src/components/*.cxx
    LINK_LIBRARIES TrigInDetTrackMonitoringToolLib 
)


# Install files from the package:
atlas_install_headers( TrigInDetMonitoringTools )
atlas_install_python_modules( python/*.py )
