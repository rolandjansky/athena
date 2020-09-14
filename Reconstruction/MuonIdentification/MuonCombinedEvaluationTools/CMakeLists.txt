################################################################################
# Package: MuonCombinedEvaluationTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedEvaluationTools )

# Component(s) in the package:
atlas_add_component( MuonCombinedEvaluationTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks AthenaBaseComps xAODEventInfo xAODMuon xAODTracking xAODTruth GaudiKernel MuonCalibEventBase MuonCalibITools MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonCombinedToolInterfaces TrkDetDescrInterfaces TrkGeometry TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkTrack TrkFitterInterfaces )

