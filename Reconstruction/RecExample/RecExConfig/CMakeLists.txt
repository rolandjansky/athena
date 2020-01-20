################################################################################
# Package: RecExConfig
################################################################################

# Declare the package name:
atlas_subdir( RecExConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py share/tests/*.py )
atlas_install_scripts( share/RecExConfig_links.sh )

atlas_add_test( testObjKeyStore SCRIPT python -m RecExConfig.testObjKeyStore
    POST_EXEC_SCRIPT nopost.sh ) 

