################################################################################
# Package: PixelMonitoring
################################################################################

# Declare the package name:
atlas_subdir( PixelMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( PixelMonitoring
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib AtlasDetDescr
   GaudiKernel InDetRawData InDetPrepRawData TrkTrack 
   AthenaPoolUtilities GeoPrimitives xAODEventInfo EventPrimitives InDetIdentifier
   InDetReadoutGeometry InDetRIO_OnTrack LWHists TrkParameters TrkSpacePoint
   TrkTrackSummary TrkToolInterfaces PixelCablingLib PixelGeoModelLib PathResolver 
   InDetTrackSelectionToolLib InDetConditionsSummaryService PixelConditionsToolsLib )

atlas_install_python_modules( python/*.py )
