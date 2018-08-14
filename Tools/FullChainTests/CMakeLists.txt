################################################################################
# Package: FullChainTests
################################################################################

# Declare the package name:
atlas_subdir( FullChainTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( test/FullChainTests_TestConfiguration.xml test/*.cfg )
atlas_install_scripts( scripts/*.py scripts/*.sh )

