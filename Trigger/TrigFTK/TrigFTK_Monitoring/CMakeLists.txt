################################################################################
# Package: TrigFTK_Monitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigFTK_Monitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          Trigger/TrigAnalysis/TrigInDetAnalysis
                          Trigger/TrigAnalysis/TrigInDetAnalysisUtils
                          Trigger/TrigFTK/FTK_DataProviderInterfaces
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Event/EventInfo
                          GaudiKernel
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_component( TrigFTK_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTracking TrigInDetAnalysis TrigInDetAnalysisUtils FTK_DataProviderInterfaces TrigInterfacesLib EventInfo GaudiKernel TrigNavigationLib TrigSteeringEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigFTK_Monitoring )
atlas_install_python_modules( python/*.py )

