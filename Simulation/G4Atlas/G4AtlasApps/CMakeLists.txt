################################################################################
# Package: G4AtlasApps
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasApps )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_test(test_AtlasG4_tf_configuration
               SCRIPT test/test_AtlasG4_tf_configuration.py)

atlas_add_test(test_AtlasG4_cosmics_configuration
               SCRIPT test/test_AtlasG4_cosmics_configuration.py)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/G4AtlasApps_TestConfiguration.xml )

