################################################################################
# Package: InDetGlobalMonitoring
################################################################################

# Declare the package name:
atlas_subdir( InDetGlobalMonitoring )

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
   GeneratorObjects InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry InDetBCM_RawData
   InDetRawData InDetPrepRawData InDetRIO_OnTrack InDetByteStreamErrors LWHists TrkSurfaces
   TrkEventPrimitives TrkMeasurementBase TrkParameters TrkParticleBase TrkTrack
   TrkTrackSummary TrkTruthData VxVertex TrkExInterfaces TrkToolInterfaces
   TrkVertexFitterInterfaces PixelGeoModelLib InDetTrackSelectionToolLib
   BeamSpotConditionsData InDetConditionsSummaryService SCT_ConditionsToolsLib )
