################################################################################
# Package: RecExTrigTest
################################################################################

# Declare the package name:
atlas_subdir( RecExTrigTest )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/RecExTrigTest_TestConfiguration.xml share/run_memleak_rett.sh )

