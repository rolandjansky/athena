################################################################################
# Package: PixelRTT
################################################################################

# Declare the package name:
atlas_subdir( PixelRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          InnerDetector/InDetCalibAlgs/PixelCalibAlgs
                          PRIVATE
                          InnerDetector/InDetConditions/PixelConditionsData
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTCintexLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( PixelValidation
                   src/*.cxx
                   PUBLIC_HEADERS PixelRTT
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} PixelOfflineCalib
                   PRIVATE_LINK_LIBRARIES PixelConditionsData PathResolver )

atlas_add_executable( doPixelValidation
                      Application/doPixelValidation.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} PixelOfflineCalib PixelConditionsData PathResolver PixelValidation )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.* )

