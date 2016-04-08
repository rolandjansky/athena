################################################################################
# Package: PyJobTransformsMisc
################################################################################

# Declare the package name:
atlas_subdir( PyJobTransformsMisc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/PyJobTransformsCore )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_scripts( share/merge_trf.py )

