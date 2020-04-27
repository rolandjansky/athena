################################################################################
# Package: InDetGlobalMonitoringRun3Test
################################################################################

# Declare the package name:
atlas_subdir( InDetGlobalMonitoringRun3Test )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaMonitoring
   DetectorDescription/AtlasDetDescr
   GaudiKernel
   InnerDetector/InDetDetDescr/PixelGeoModel
   InnerDetector/InDetRawEvent/InDetRawData
   InnerDetector/InDetRecEvent/InDetPrepRawData
   Tracking/TrkEvent/TrkTrack
   PRIVATE
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/GeoPrimitives
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTracking
   Event/EventPrimitives
   InnerDetector/InDetConditions/InDetConditionsSummaryService
   InnerDetector/InDetConditions/InDetByteStreamErrors
   InnerDetector/InDetConditions/PixelConditionsTools
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelCabling
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   InnerDetector/InDetConditions/BeamSpotConditionsData
   Tools/LWHists
   Tools/PathResolver
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkSpacePoint
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkEvent/VxVertex
   Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
   Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( InDetGlobalMonitoringRun3Test
   InDetGlobalMonitoringRun3Test/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib AtlasDetDescr
   GaudiKernel InDetRawData InDetPrepRawData TrkTrack InDetByteStreamErrors 
   AthenaPoolUtilities GeoPrimitives xAODEventInfo EventPrimitives InDetIdentifier
   InDetReadoutGeometry InDetRIO_OnTrack LWHists TrkParameters TrkSpacePoint VxVertex TrkVertexFitterInterfaces xAODTracking
   TrkTrackSummary TrkToolInterfaces PixelCablingLib PixelGeoModelLib PathResolver BeamSpotConditionsData 
   InDetTrackSelectionToolLib )

atlas_install_python_modules( python/*.py )
