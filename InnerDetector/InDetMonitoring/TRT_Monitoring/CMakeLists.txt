
################################################################################
# Package: TRT_Monitoring
################################################################################

# Declare the package name:
atlas_subdir( TRT_Monitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData
                          LumiBlock/LumiBlockComps
                          PRIVATE
                          Control/StoreGate
                          Commission/CommissionEvent
                          Control/DataModel
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTrigger
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          InnerDetector/InDetRecTools/TRT_TrackHoleSearch
                          Tools/LWHists
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TRT_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaMonitoringLib GaudiKernel InDetRawData LumiBlockCompsLib CommissionEvent DataModel AtlasDetDescr Identifier xAODEventInfo xAODTrigger EventInfo EventPrimitives TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry InDetRIO_OnTrack LWHists TrkTrack TrkTrackSummary TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TRT_Monitoring )
atlas_install_joboptions( share/*.py )

