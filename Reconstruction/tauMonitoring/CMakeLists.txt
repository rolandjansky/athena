################################################################################
# Package: tauMonitoring
################################################################################

# Declare the package name:
atlas_subdir( tauMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Event/xAOD/xAODTau
                          GaudiKernel
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Trigger/TrigAnalysis/TrigDecisionTool )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( tauMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib xAODTau GaudiKernel xAODEventInfo xAODTracking TrigDecisionToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
