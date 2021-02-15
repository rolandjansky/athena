################################################################################
# Package: RecJobTransformTests
################################################################################

# Declare the package name:
atlas_subdir( RecJobTransformTests )

# Install files from the package:
atlas_install_runtime( test/RecJobTransformTests_TestConfiguration.xml share/compressRTTLogFiles_rjtt.sh share/run_memleak_rjtt.sh )

#Make test scripts available
atlas_install_scripts( test/test*.sh )
