
################################################################################
# Package: EvgenJobTransforms
################################################################################

# Declare the package name:
atlas_subdir( EvgenJobTransforms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/PyJobTransforms )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py )
