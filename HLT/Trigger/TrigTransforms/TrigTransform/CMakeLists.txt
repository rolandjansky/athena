################################################################################
# Package: TrigTransform
################################################################################

# Declare the package name:
atlas_subdir( TrigTransform )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Reconstruction/RecJobTransforms
                          Tools/PyJobTransforms )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( scripts/*.py )

