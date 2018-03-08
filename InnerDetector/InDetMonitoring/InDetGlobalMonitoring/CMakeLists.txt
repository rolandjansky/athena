################################################################################
# Package: InDetGlobalMonitoring
################################################################################

# Declare the package name:
atlas_subdir( InDetGlobalMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   PRIVATE
   Commission/CommissionEvent
   Control/AthenaBaseComps
   Control/AthenaMonitoring
   DetectorDescription/AtlasDetDescr
   DetectorDescription/IdDictDetDescr
   DetectorDescription/Identifier
   Event/EventInfo
   Event/EventPrimitives
   Event/xAOD/xAODJet
   Event/xAOD/xAODTracking
   Generators/GeneratorObjects
   InnerDetector/InDetConditions/InDetBeamSpotService
   InnerDetector/InDetConditions/InDetConditionsSummaryService
   InnerDetector/InDetConditions/PixelConditionsServices
   InnerDetector/InDetConditions/SCT_ConditionsServices
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelGeoModel
   InnerDetector/InDetRawEvent/InDetBCM_RawData
   InnerDetector/InDetRawEvent/InDetRawData
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   Tools/LWHists
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkParticleBase
   Tracking/TrkEvent/TrkSpacePoint
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkEvent/TrkTruthData
   Tracking/TrkEvent/VxVertex
   Tracking/TrkExtrapolation/TrkExInterfaces
   Tracking/TrkTools/TrkToolInterfaces
   Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist )

# Component(s) in the package:
atlas_add_component( InDetGlobalMonitoring
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel
   CommissionEvent AthenaBaseComps AthenaMonitoringLib AtlasDetDescr
   IdDictDetDescr Identifier EventInfo EventPrimitives xAODJet xAODTracking
   GeneratorObjects InDetIdentifier InDetReadoutGeometry InDetBCM_RawData
   InDetRawData InDetPrepRawData InDetRIO_OnTrack LWHists TrkSurfaces
   TrkEventPrimitives TrkMeasurementBase TrkSpacePoint TrkParameters TrkParticleBase TrkTrack
   TrkTrackSummary TrkTruthData VxVertex TrkExInterfaces TrkToolInterfaces
   TrkVertexFitterInterfaces PixelGeoModelLib InDetTrackSelectionToolLib )
