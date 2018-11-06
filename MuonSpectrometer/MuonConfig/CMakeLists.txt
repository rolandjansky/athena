################################################################################
# Package: MuonConfiguration
################################################################################

# Declare the package name:
atlas_subdir( MuonConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


atlas_add_test( MuonCablingConfigTest
   SCRIPT python -m MuonConfig.MuonCablingConfig
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( MuonReconstructionConfigTest
   SCRIPT python -m MuonConfig.MuonReconstructionConfig
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( MuonSegmentFindingConfigTest
   SCRIPT python -m MuonConfig.MuonSegmentFindingConfig
   POST_EXEC_SCRIPT nopost.sh )
