################################################################################
# Package: InDetConfig
################################################################################

# Declare the package name:
atlas_subdir( InDetConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( InDetClusterization_test
    SCRIPT python -m InDetConfig.ClusterizationConfig
    PROPERTIES TIMEOUT 600)
