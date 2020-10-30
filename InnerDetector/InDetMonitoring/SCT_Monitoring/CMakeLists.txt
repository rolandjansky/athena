################################################################################
# Package: SCT_Monitoring
################################################################################

# Declare the package name:
atlas_subdir( SCT_Monitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     PUBLIC_HEADERS SCT_Monitoring
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib GaudiKernel InDetReadoutGeometry SCT_ReadoutGeometry InDetPrepRawData ITrackToVertex TrkTrack TrkToolInterfaces CommissionEvent AthenaKernel AthContainers Identifier InDetIdentifier InDetRawData InDetRIO_OnTrack LWHists PathResolver TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkSpacePoint TrkTrackSummary MagFieldElements MagFieldConditions InDetConditionsSummaryService SCT_ConditionsToolsLib )

atlas_add_dictionary( SCT_MonitoringDict
                      SCT_Monitoring/SCT_MonitoringDict.h
                      SCT_Monitoring/selection.xml )

# Run tests:
# This is a standalone test with ESD input.
# This is not essential and can be abandoned if it won't run.
atlas_add_test( SCTLorentzMonAlg_test
                SCRIPT python -m SCT_Monitoring.SCTLorentzMonAlg
                PROPERTIES TIMEOUT 600 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
