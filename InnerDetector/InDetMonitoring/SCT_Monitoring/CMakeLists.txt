################################################################################
# Package: SCT_Monitoring
################################################################################

# Declare the package name:
atlas_subdir( SCT_Monitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          MagneticField/MagFieldInterfaces
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Commission/CommissionEvent
                          Control/AthenaKernel
                          Control/DataModel
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tools/LWHists
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrackSummary )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaMonitoringLib GaudiKernel InDetReadoutGeometry InDetPrepRawData MagFieldInterfaces ITrackToVertex TrkTrack TrkToolInterfaces TrigDecisionToolLib CommissionEvent AthenaKernel DataModel Identifier EventInfo InDetIdentifier InDetRawData InDetRIO_OnTrack LWHists PathResolver TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkSpacePoint TrkTrackSummary )

# Install files from the package:
atlas_install_headers( SCT_Monitoring )
atlas_install_joboptions( share/*.py )

