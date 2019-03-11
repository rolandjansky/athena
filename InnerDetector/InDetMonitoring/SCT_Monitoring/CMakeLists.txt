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
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRawEvent/InDetRawData
                          MagneticField/MagFieldInterfaces
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          PRIVATE
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
                          Trigger/TrigEvent/TrigDecisionInterface )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaMonitoringLib GaudiKernel InDetReadoutGeometry InDetPrepRawData MagFieldInterfaces ITrackToVertex TrkTrack TrkToolInterfaces CommissionEvent AthenaKernel AthContainers Identifier InDetIdentifier InDetRawData InDetRIO_OnTrack LWHists PathResolver TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkSpacePoint TrkTrackSummary )

# Install files from the package:
atlas_install_headers( SCT_Monitoring )
atlas_install_joboptions( share/*.py )

