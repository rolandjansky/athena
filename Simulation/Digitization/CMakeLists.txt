################################################################################
# Package: Digitization
################################################################################

# Declare the package name:
atlas_subdir( Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/jobOpts/*.py )
atlas_install_runtime( data/*.ascii test/Digitization_TestConfiguration.xml )

atlas_add_test(test_MC16a_Digi_tf_configuration
               SCRIPT test/test_MC16a_Digi_tf_configuration.py
               PROPERTIES TIMEOUT 300
               PROPERTIES RESOURCE_LOCK DigiTf)
