################################################################################
# Package: PFODQA
################################################################################

# Declare the package name:
atlas_subdir( PFODQA )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaMonitoring
			  Control/StoreGate
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODPFlow
			  Event/xAOD/xAODTracking
                          Reconstruction/PFlow/PFlowValidation/PFOHistUtils
                          Tracking/TrkValidation/TrkValHistUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( PFODQA
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaMonitoringLib xAODCaloEvent xAODPFlow PFOHistUtils TrkValHistUtils )

