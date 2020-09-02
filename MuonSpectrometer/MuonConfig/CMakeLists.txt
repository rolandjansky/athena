################################################################################
# Package: MuonConfiguration
################################################################################

# Declare the package name:
atlas_subdir( MuonConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py test/MdtRdoToPrepDataTool_test.py test/MdtIntersectGeometry_test.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_data( share/*.ref )

if( NOT SIMULATIONBASE )
   # Mdt tests
   atlas_add_test( MdtRdoToPrepDataTool_test
     SCRIPT python -m MuonConfig.MdtRdoToPrepDataTool_test
     PROPERTIES TIMEOUT 600
     LOG_SELECT_PATTERN "dead tube" )

   atlas_add_test( MdtIntersectGeometry_test
     SCRIPT python -m MuonConfig.MdtIntersectGeometry_test
     PROPERTIES TIMEOUT 600
     LOG_SELECT_PATTERN "dead tube" )

   # Configure unit tests
   # Cache alignment will change depending on whether FRONTIER_SERVER is defined.
   file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_MuonDataDecodeTest )
   atlas_add_test( MuonDataDecodeTest
                   PROPERTIES TIMEOUT 1000
                   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_MuonDataDecodeTest
                   LOG_SELECT_PATTERN "^MdtRawDataProvi.*|^RpcRawDataProvi.*|^TgcRawDataProvi.*|^CscRawDataProvi.*|^MdtRdoTo.*|^RpcRdoTo.*|^TgcRdoTo.*|^CscRdoTo.*|^MdtROD_Decod.*|^RpcROD_Decod.*|^TgcROD_Decod.*|^CscROD_Decod.*|^MuonCache.*"
                   SCRIPT test/testMuonDataDecode.sh )

   # Adding an identical test for the ByteStream identifiable caches (and future RDO caches)
   file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_MuonDataDecodeTest_Cache )
   atlas_add_test( MuonDataDecodeTest_Cache
                   PROPERTIES TIMEOUT 1000
                   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_MuonDataDecodeTest_Cache
                   LOG_SELECT_PATTERN "^MdtRawDataProvi.*|^RpcRawDataProvi.*|^TgcRawDataProvi.*|^CscRawDataProvi.*|^MdtRdoTo.*|^RpcRdoTo.*|^TgcRdoTo.*|^CscRdoTo.*|^MdtROD_Decod.*|^RpcROD_Decod.*|^TgcROD_Decod.*|^CscROD_Decod.*|^MuonCache.*"
                   SCRIPT test/testMuonDataDecode_Cache.sh )

   atlas_add_test( MuonCablingConfigTest
                   SCRIPT python -m MuonConfig.MuonCablingConfig
                   POST_EXEC_SCRIPT nopost.sh )

   atlas_add_test( MuonReconstructionConfigTest
                   SCRIPT python -m MuonConfig.MuonReconstructionConfig --run
                   PROPERTIES TIMEOUT 1200
                   POST_EXEC_SCRIPT nopost.sh )

   atlas_add_test( MuonSegmentFindingConfigTest
                   SCRIPT python -m MuonConfig.MuonSegmentFindingConfig --run
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )
endif()
