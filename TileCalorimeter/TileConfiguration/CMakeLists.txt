################################################################################
# Package: TileConfiguration
################################################################################

# Declare the package name:
atlas_subdir( TileConfiguration )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Check python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )

if( NOT SIMULATIONBASE )

    atlas_add_test( TileConfigFlagsTest
                    SCRIPT python -m TileConfiguration.TileConfigFlags
                    POST_EXEC_SCRIPT nopost.sh )

endif()
