################################################################################
# Package: SimExoticsTests
################################################################################

# Declare the package name:
atlas_subdir( SimExoticsTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/SimExoticsTests_TestConfiguration.xml )

