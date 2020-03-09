################################################################################
# Package: SimCoreTests
################################################################################

# Declare the package name:
atlas_subdir( SimCoreTests )

# Install files from the package:
atlas_install_runtime( test/SimCoreTests_TestConfiguration.xml )
atlas_install_scripts( test/*.sh test/*.py )
