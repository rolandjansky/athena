################################################################################
# Package: NTUPtoNTUPCore
################################################################################

# Declare the package name:
atlas_subdir( NTUPtoNTUPCore )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/PyJobTransformsCore )

# Install files from the package:
atlas_install_python_modules( python/*.py )

