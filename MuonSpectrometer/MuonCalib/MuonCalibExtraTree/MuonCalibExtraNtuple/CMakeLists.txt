################################################################################
# Package: MuonCalibExtraNtuple
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibExtraNtuple )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( MuonCalibExtraNtuple
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibExtraNtuple
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CalibNtupleReader
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} MuonCalibEventBase MuonCalibExtraTreeEvent MuonCalibIdentifier )

