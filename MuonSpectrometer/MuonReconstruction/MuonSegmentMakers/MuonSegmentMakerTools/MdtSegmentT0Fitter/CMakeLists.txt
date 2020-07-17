################################################################################
# Package: MdtSegmentT0Fitter
################################################################################

# Declare the package name:
atlas_subdir( MdtSegmentT0Fitter )

# External dependencies:
find_package( ROOT COMPONENTS Minuit Core Tree MathCore Hist RIO pthread MathMore Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( MdtSegmentT0Fitter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibSvcLib TrkDriftCircleMath MdtCalibData MuonCalibToolsLib MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonSegmentMakerInterfacesLib )

# Install files from the package:
atlas_install_headers( MdtSegmentT0Fitter )
