################################################################################
# Package: SimCoreTests
################################################################################

# Declare the package name:
atlas_subdir( SimCoreTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/SimCoreTests_TestConfiguration.xml )
atlas_install_scripts( test/*.sh test/*.py )
