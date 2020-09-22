################################################################################
# Package: MMRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MMRawDataMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Graf Core Tree MathCore Hist RIO pthread Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( MMRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
 		     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib EventInfo MuonIdHelpersLib MuonPrepRawData MuonDQAUtilsLib TrkSegment Identifier xAODEventInfo xAODTrigger GaudiKernel MuonCalibIdentifier MuonReadoutGeometry MuonRIO_OnTrack MuonSegment AnalysisTriggerEvent LWHists TrkEventPrimitives GeoModelUtilities MuonAnalysisInterfacesLib) 

#
# Install files from the package:
atlas_install_headers( MMRawDataMonitoring )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
