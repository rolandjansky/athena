################################################################################
# Package: ReleaseTests
################################################################################

# Declare the package name:
atlas_subdir( ReleaseTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/ReleaseTests_TestConfiguration.xml )

