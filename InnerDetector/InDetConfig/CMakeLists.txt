################################################################################
# Package: InDetConfig
################################################################################

# Declare the package name:
atlas_subdir( InDetConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py ${ATLAS_FLAKE8} )
#atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
#atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( TrackingCutsFlags_test
    SCRIPT python -m InDetConfig.TrackingCutsFlags
    POST_EXEC_SCRIPT nopost.sh)
 
