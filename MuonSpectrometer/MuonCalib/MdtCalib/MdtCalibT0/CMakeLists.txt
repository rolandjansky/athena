################################################################################
# Package: MdtCalibT0
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibT0 )

# External dependencies:
find_package( ROOT COMPONENTS Graf Minuit Core Tree MathCore Hist RIO pthread MathMore Minuit2 Matrix Physics HistPainter Rint RooFitCore RooFit Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( MdtCalibT0
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibT0
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} MdtCalibData MuonCalibStandAloneBaseLib
                   PRIVATE_LINK_LIBRARIES MuonCalibEventBase MuonCalibIdentifier )

atlas_add_dictionary( MdtCalibT0Dict
                      MdtCalibT0/ReflexHeaders.h
                      MdtCalibT0/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} MdtCalibData MuonCalibStandAloneBaseLib MuonCalibEventBase MuonCalibIdentifier MdtCalibT0 )

