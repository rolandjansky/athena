################################################################################
# Package: AthenaConfiguration
################################################################################

# Declare the package name:
atlas_subdir( AthenaConfiguration )

# External dependencies:


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime(python/*.pkl )

atlas_add_test( ComponentAccumulatorTest SCRIPT test/test_ComponentAccumulator.sh ) 