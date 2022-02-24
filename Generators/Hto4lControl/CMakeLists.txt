################################################################################
# Package: Hto4lControl
################################################################################

# Declare the package name:
atlas_subdir( Hto4lControl )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )
find_package( Hto4l )
# find_package( Powheg )
find_package( Pythia8 )

# Install files from the package:
atlas_install_python_modules( python/*.py python/decorators )
atlas_install_joboptions( share/*.py )

