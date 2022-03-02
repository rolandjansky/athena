# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaConfiguration )

# External dependencies.
find_package( future )

# Install files from the package:
atlas_install_python_modules( python/*.py python/iconfTool
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_install_scripts( share/confTool.py python/iconfTool/iconfTool share/CARunner.py
                       POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests in the package:
atlas_add_test( AthConfigFlagsTest
   SCRIPT test/testAthConfigFlags.py
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( MainServicesConfig
   SCRIPT python -m AthenaConfiguration.MainServicesConfig
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( testAccumulatorCache
   SCRIPT test/testAccumulatorCache.py
   POST_EXEC_SCRIPT nopost.sh )

if( NOT XAOD_ANALYSIS )
   atlas_add_test( ComponentAccumulatorTest
      SCRIPT test/testComponentAccumulator.py
      POST_EXEC_SCRIPT nopost.sh )

   atlas_add_test( AllConfigFlagsTest_EVNT_test
      SCRIPT test/testAllConfigFlags_EVNT.py
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 300 )
endif()

if( NOT GENERATIONBASE AND NOT XAOD_ANALYSIS )
    atlas_add_test( DetectorConfigFlags_test
                    SCRIPT test/testDetectorFlags.py
                    POST_EXEC_SCRIPT nopost.sh
                    PROPERTIES TIMEOUT 300 )

    atlas_add_test( AllConfigFlagsTest_HITS_test
                    SCRIPT test/testAllConfigFlags_HITS.py
                    POST_EXEC_SCRIPT nopost.sh
                    PROPERTIES TIMEOUT 300  )
endif()

if( NOT SIMULATIONBASE AND NOT GENERATIONBASE AND NOT XAOD_ANALYSIS )
    atlas_add_test( AllConfigFlagsTest_RDO_test
                    SCRIPT test/testAllConfigFlags_RDO.py
                    POST_EXEC_SCRIPT nopost.sh
                    PROPERTIES TIMEOUT 300  )

    atlas_add_test( AllConfigFlagsTest
                    SCRIPT python -m AthenaConfiguration.AllConfigFlags
                    POST_EXEC_SCRIPT nopost.sh
                    PROPERTIES TIMEOUT 300  )

    atlas_add_test( ComponentAccumulatorTestReco
                    SCRIPT test/testComponentAccumulatorReco.py
                    POST_EXEC_SCRIPT nopost.sh )

    atlas_add_test( OldFlags2NewFlagsTest
                    SCRIPT python -m AthenaConfiguration.OldFlags2NewFlags
                    POST_EXEC_SCRIPT nopost.sh )

    atlas_add_test( AtlasSemantics
                    SCRIPT test/testAtlasSemantics.py
                    POST_EXEC_SCRIPT nopost.sh )
endif()
