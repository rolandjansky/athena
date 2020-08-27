################################################################################
# Package: ISF_Config
################################################################################

# Declare the package name:
atlas_subdir( ISF_Config )

if( NOT SIMULATIONBASE )
   atlas_add_test(test_Sim_tf_configuration
                  SCRIPT test/test_Sim_tf_configuration.py
                  PROPERTIES TIMEOUT 300
                  PROPERTIES RESOURCE_LOCK SimTf)
endif()
atlas_add_test(test_FullG4_Sim_tf_configuration
               SCRIPT test/test_FullG4_Sim_tf_configuration.py
               PROPERTIES TIMEOUT 300
               PROPERTIES RESOURCE_LOCK SimTf)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

