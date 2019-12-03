################################################################################
# Package: SimPerformanceTests
################################################################################

# Declare the package name:
atlas_subdir( SimPerformanceTests )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/SimPerformanceTests_TestConfiguration.xml )

