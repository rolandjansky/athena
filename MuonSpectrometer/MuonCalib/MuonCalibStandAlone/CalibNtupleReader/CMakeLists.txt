################################################################################
# Package: CalibNtupleReader
################################################################################

# Declare the package name:
atlas_subdir( CalibNtupleReader )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( CalibNtupleReader
                   src/*.cxx
                   PUBLIC_HEADERS CalibNtupleReader
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} MuonCalibEventBase
                   PRIVATE_LINK_LIBRARIES GeoPrimitives )

