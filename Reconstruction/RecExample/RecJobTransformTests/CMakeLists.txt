################################################################################
# Package: RecJobTransformTests
################################################################################

# Declare the package name:
atlas_subdir( RecJobTransformTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/RecJobTransformTests_TestConfiguration.xml share/compressRTTLogFiles_rjtt.sh share/run_memleak_rjtt.sh )

