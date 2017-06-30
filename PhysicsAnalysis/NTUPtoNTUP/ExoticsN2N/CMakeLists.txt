################################################################################
# Package: ExoticsN2N
################################################################################

# Declare the package name:
atlas_subdir( ExoticsN2N )

# External dependencies:
find_package( ROOT COMPONENTS Hist Physics Tree Core MathCore RIO pthread MathMore Minuit Minuit2 Matrix HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_executable( ExoticsMultiJetSkim
                      src/ExoticsMultiJetSkim.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

