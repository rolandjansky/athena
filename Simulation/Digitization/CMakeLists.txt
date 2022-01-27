################################################################################
# Package: Digitization
################################################################################

# Declare the package name:
atlas_subdir( Digitization )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py share/jobOpts/*.py )
atlas_install_scripts( test/*.py )
atlas_install_runtime( data/*.ascii )

atlas_add_test(test_MC16a_Digi_tf_configuration
               SCRIPT test/test_MC16a_Digi_tf_configuration.py
               POST_EXEC_SCRIPT nopost.sh
               PROPERTIES TIMEOUT 600
               PROPERTIES RESOURCE_LOCK DigiTf)
