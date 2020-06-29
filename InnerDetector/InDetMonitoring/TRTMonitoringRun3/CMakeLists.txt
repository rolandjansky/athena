
################################################################################
# Package: TRTMonitoringRun3
################################################################################

# Declare the package name:
atlas_subdir( TRTMonitoringRun3 )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Control/StoreGate
                          GaudiKernel
                          Control/SGTools
                          InnerDetector/InDetRawEvent/InDetRawData
                          Control/AthenaBaseComps
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/TRT_ConditionsServices
			  InnerDetector/InDetConditions/InDetByteStreamErrors
                          Event/xAOD/xAODEventInfo
                          Commission/CommissionEvent
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          LumiBlock/LumiBlockComps
                          Control/AthContainers
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODTrigger
                          Tools/PathResolver
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Trigger/TrigEvent/TrigDecisionInterface
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          InnerDetector/InDetRecTools/TRT_TrackHoleSearch
                          Tools/LWHists
                          Tracking/TrkEvent/TrkTrackSummary
                          Control/AthenaCommon
                          Control/AthenaConfiguration
                          Control/AthenaKernel
                          Control/SGMon/SGAudCore
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          AtlasTest/TestTools)


# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Tree Hist RIO pthread MathCore Core Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11)

atlas_add_library( TRTMonitoringRun3Lib
                   src/*.cxx
                   PUBLIC_HEADERS TRTMonitoringRun3
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps xAODEventInfo InDetPrepRawData GaudiKernel TRT_ConditionsServicesLib InDetReadoutGeometry TRT_ReadoutGeometry InDetRawData TrkTrack TrkToolInterfaces AthenaMonitoringLib CommissionEvent TrkSpacePoint
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel SGTools SGAudCore AthenaPoolUtilities LumiBlockCompsLib EventInfo LWHists TrkTrackSummary InDetRIO_OnTrack InDetIdentifier TrkRIO_OnTrack TrkParameters TrkMeasurementBase TrkEventUtils TrkSurfaces PathResolver EventPrimitives Identifier AtlasDetDescr AthContainers TRT_DriftFunctionToolLib )
                   
atlas_add_component( TRTMonitoringRun3
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} TRTMonitoringRun3Lib AthenaMonitoringLib SGAudCore SGTools AthenaPoolUtilities GaudiKernel EventInfo InDetRawData InDetPrepRawData LumiBlockCompsLib CommissionEvent AthContainers AtlasDetDescr Identifier xAODEventInfo EventPrimitives TRT_ConditionsServicesLib InDetReadoutGeometry TRT_ReadoutGeometry InDetRIO_OnTrack LWHists TrkTrack TrkTrackSummary TrkToolInterfaces AthenaKernel InDetIdentifier MagFieldInterfaces PathResolver TrkSurfaces TrkEventUtils TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkSpacePoint TRT_DriftFunctionToolLib )
                   
# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

