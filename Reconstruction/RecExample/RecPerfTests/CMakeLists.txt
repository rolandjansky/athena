################################################################################
# Package: RecPerfTests
################################################################################

# Declare the package name:
atlas_subdir( RecPerfTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/RecPerfTests_TestConfiguration.xml )
