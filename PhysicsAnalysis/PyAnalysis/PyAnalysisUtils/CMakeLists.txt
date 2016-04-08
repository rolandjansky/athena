################################################################################
# Package: PyAnalysisUtils
################################################################################

# Declare the package name:
atlas_subdir( PyAnalysisUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_headers( PyAnalysisUtils )
atlas_install_python_modules( python/*.py )

