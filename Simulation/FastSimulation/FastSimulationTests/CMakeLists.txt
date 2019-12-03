################################################################################
# Package: FastSimulationTests
################################################################################

# Declare the package name:
atlas_subdir( FastSimulationTests )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_runtime( test/FastSimulationTests_TestConfiguration.xml share/*.C share/*.h )

