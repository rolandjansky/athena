################################################################################
# Package: MuonTrackSummaryHelperTool
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackSummaryHelperTool )

# Component(s) in the package:
atlas_add_component( MuonTrackSummaryHelperTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonPrepRawData TrkGeometry TrkEventPrimitives TrkTrackSummary TrkToolInterfaces StoreGateLib SGtests Identifier MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces TrkDetElementBase TrkSurfaces TrkCompetingRIOsOnTrack TrkMeasurementBase TrkParameters TrkPseudoMeasurementOnTrack TrkTrack TrkExInterfaces )
