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

atlas_add_test( egammaReconstructionConfigTest
		SCRIPT python -m egammaConfig.egammaReconstructionConfig
		POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( egammaUpstreamConfigTest
		SCRIPT python -m egammaConfig.egammaUpstreamConfig
		POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( egammaxAODThinningConfigTest
		SCRIPT python -m egammaConfig.egammaxAODThinningConfig
		POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( egammaSteeringConfigTest
		SCRIPT python -m egammaConfig.egammaSteeringConfig
		POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( egammaLRTReconstructionConfigTest
		SCRIPT python -m egammaConfig.egammaLRTReconstructionConfig
		POST_EXEC_SCRIPT nopost.sh )