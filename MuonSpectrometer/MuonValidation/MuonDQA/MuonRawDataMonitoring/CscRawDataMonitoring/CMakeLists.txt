################################################################################
# Package: CscRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( CscRawDataMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( CscRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib GaudiKernel MuonPrepRawData MuonSegment TrkSegment TrkTrack TrigDecisionToolLib StoreGateLib xAODEventInfo MuonIdHelpersLib MuonRDO CscClusterizationLib MuonRIO_OnTrack MuonRecHelperToolsLib TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives MuonCSC_CnvToolsLib )

# Install files from the package:
atlas_install_headers( CscRawDataMonitoring )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

