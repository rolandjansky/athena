################################################################################
# Package: AthDataQuality
################################################################################

# Declare the package name:
atlas_subdir( AthDataQuality )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE ART )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( test/*.sh )
