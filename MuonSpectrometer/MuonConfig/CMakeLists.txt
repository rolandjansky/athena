################################################################################
# Package: MuonConfiguration
################################################################################

# Declare the package name:
atlas_subdir( MuonConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref )

# Configure unit tests
# Cache alignment will change depending on whether FRONTIER_SERVER is defined.
atlas_add_test( MuonDataDecodeTest
                PROPERTIES TIMEOUT 1000
                EXTRA_PATTERNS "GeoModelSvc.MuonDetectorTool.*SZ=|Cache alignment|Range of input|recorded new|map from|DEBUG Reconciled configuration"
                SCRIPT test/testMuonDataDecode.sh )

# Adding an identical test for the ByteStream identifiable caches (and future RDO caches)
atlas_add_test( MuonDataDecodeTest_Cache
                PROPERTIES TIMEOUT 1000
                EXTRA_PATTERNS "GeoModelSvc.MuonDetectorTool.*SZ=|Cache alignment|Range of input|recorded new|map from"
                SCRIPT test/testMuonDataDecode_Cache.sh )

atlas_add_test( MuonCablingConfigTest
   SCRIPT python -m MuonConfig.MuonCablingConfig
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( MuonReconstructionConfigTest
   SCRIPT python -m MuonConfig.MuonReconstructionConfig
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( MuonSegmentFindingConfigTest
   SCRIPT python -m MuonConfig.MuonSegmentFindingConfig
   POST_EXEC_SCRIPT nopost.sh )
