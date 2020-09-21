################################################################################
# Package: CalibNtupleUtils
################################################################################

# Declare the package name:
atlas_subdir( CalibNtupleUtils )

# External dependencies:
find_package( ROOT COMPONENTS MathCore MathMore Graf Hist Core Tree RIO pthread Minuit Minuit2 Matrix Physics HistPainter Rint Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( CalibNtupleUtils
                   src/*.cxx
                   PUBLIC_HEADERS CalibNtupleUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} MuonCalibEventBase
                   PRIVATE_LINK_LIBRARIES MuonCalibIdentifier )

