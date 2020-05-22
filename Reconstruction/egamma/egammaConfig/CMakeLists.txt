################################################################################
# Package: egammaConfig
################################################################################

# Declare the package name:
atlas_subdir( egammaConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( egammaConfigFlagsTest
		SCRIPT python -m unittest -v egammaConfig.egammaConfigFlags
		POST_EXEC_SCRIPT nopost.sh )
