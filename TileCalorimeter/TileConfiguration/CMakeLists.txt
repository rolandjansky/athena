################################################################################
# Package: TileConfiguration
################################################################################

# Declare the package name:
atlas_subdir( TileConfiguration )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests:
if( NOT SIMULATIONBASE )

    atlas_add_test( TileConfigFlagsTest
                    SCRIPT python -m TileConfiguration.TileConfigFlags
                    POST_EXEC_SCRIPT nopost.sh )

endif()
