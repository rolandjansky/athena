################################################################################
# Package: SCT_Monitoring
################################################################################

# Declare the package name:
atlas_subdir( SCT_Monitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Commission/CommissionEvent
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRawEvent/InDetRawData
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          PRIVATE
			  LumiBlock/LumiBlockData
                          Control/AthenaKernel
                          Control/AthContainers
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tools/LWHists
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Trigger/TrigEvent/TrigDecisionInterface
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     PUBLIC_HEADERS SCT_Monitoring
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib GaudiKernel InDetReadoutGeometry SCT_ReadoutGeometry InDetPrepRawData ITrackToVertex TrkTrack TrkToolInterfaces CommissionEvent AthenaKernel AthContainers Identifier InDetIdentifier InDetRawData InDetRIO_OnTrack LWHists PathResolver TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkSpacePoint TrkTrackSummary MagFieldElements MagFieldConditions )

atlas_add_dictionary( SCT_MonitoringDict
                      SCT_Monitoring/SCT_MonitoringDict.h
                      SCT_Monitoring/selection.xml )

# Run tests:
atlas_add_test( SCTLorentzMonAlg_test
                SCRIPT python -m SCT_Monitoring.SCTLorentzMonAlg
                PROPERTIES TIMEOUT 600 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
