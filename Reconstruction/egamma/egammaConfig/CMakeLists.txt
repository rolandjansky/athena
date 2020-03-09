################################################################################
# Package: egammaConfig
################################################################################

# Declare the package name:
atlas_subdir( egammaConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test( egammaConfigFlagsTest
		SCRIPT python -m unittest -v egammaConfig.egammaConfigFlags
		POST_EXEC_SCRIPT nopost.sh )

# Check python syntax on Config files
atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python/*.py
                POST_EXEC_SCRIPT nopost.sh )
