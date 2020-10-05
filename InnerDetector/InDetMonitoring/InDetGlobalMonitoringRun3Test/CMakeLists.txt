################################################################################
# Package: InDetGlobalMonitoringRun3Test
################################################################################

# Declare the package name:
atlas_subdir( InDetGlobalMonitoringRun3Test )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( InDetGlobalMonitoringRun3Test
   InDetGlobalMonitoringRun3Test/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES InDetConditionsSummaryService
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib AtlasDetDescr
   GaudiKernel InDetRawData InDetPrepRawData TrkTrack InDetByteStreamErrors 
   AthenaPoolUtilities GeoPrimitives xAODEventInfo EventPrimitives InDetIdentifier
   InDetReadoutGeometry InDetRIO_OnTrack LWHists TrkParameters TrkSpacePoint VxVertex TrkVertexFitterInterfaces xAODTracking
   TrkTrackSummary TrkToolInterfaces PixelCablingLib PixelGeoModelLib PathResolver BeamSpotConditionsData 
   InDetTrackSelectionToolLib )

atlas_install_python_modules( python/*.py )
