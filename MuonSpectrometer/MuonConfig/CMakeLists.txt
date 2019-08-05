################################################################################
# Package: MuonConfiguration
################################################################################

# Declare the package name:
atlas_subdir( MuonConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref )

if( NOT SIMULATIONBASE )
   # Configure unit tests
   # Cache alignment will change depending on whether FRONTIER_SERVER is defined.
   file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_MuonDataDecodeTest )
   atlas_add_test( MuonDataDecodeTest
                   PROPERTIES TIMEOUT 1000
                   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_MuonDataDecodeTest
                   EXTRA_PATTERNS "-s ^MdtRawDataProvi.*|^RpcRawDataProvi.*|^TgcRawDataProvi.*|^CscRawDataProvi.*|^MdtRdoTo.*|^RpcRdoTo.*|^TgcRdoTo.*|^CscRdoTo.*|^MdtROD_Decod.*|^RpcROD_Decod.*|^TgcROD_Decod.*|^CscROD_Decod.*|^MuonCache.*"
                   SCRIPT test/testMuonDataDecode.sh )

   # Adding an identical test for the ByteStream identifiable caches (and future RDO caches)
   file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_MuonDataDecodeTest_Cache )
   atlas_add_test( MuonDataDecodeTest_Cache
                   PROPERTIES TIMEOUT 1000
                   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_MuonDataDecodeTest_Cache
                   EXTRA_PATTERNS "-s ^MdtRawDataProvi.*|^RpcRawDataProvi.*|^TgcRawDataProvi.*|^CscRawDataProvi.*|^MdtRdoTo.*|^RpcRdoTo.*|^TgcRdoTo.*|^CscRdoTo.*|^MdtROD_Decod.*|^RpcROD_Decod.*|^TgcROD_Decod.*|^CscROD_Decod.*|^MuonCache.*"
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
                   
   atlas_add_test( flake8
                   SCRIPT flake8 --select=F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                  POST_EXEC_SCRIPT nopost.sh )
endif()
