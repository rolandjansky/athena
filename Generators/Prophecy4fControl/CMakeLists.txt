################################################################################
# Package: Prophecy4fControl
################################################################################

# Declare the package name:
atlas_subdir( Prophecy4fControl )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

find_package( Prophecy4f )
# find_package( Powheg )
find_package( Pythia8 )

# Component(s) in the package:
atlas_add_executable( mergeProphecy4f
  src/mergeProphecy4f.cxx
  src/Prophecy4fMerger.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES}
  )

# Install files from the package:
atlas_install_python_modules( python/*.py python/decorators POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

