################################################################################
# Package: AthenaConfiguration
################################################################################

# Declare the package name:
atlas_subdir( AthenaConfiguration )

# External dependencies:
find_package( six )

# Install files from the package:
atlas_install_python_modules( python/*.py python/iconfTool 
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/confTool.py python/iconfTool/iconfTool python/CARunner.py)

atlas_add_test( ComponentAccumulatorTest
   SCRIPT python -m unittest -v AthenaConfiguration.ComponentAccumulatorTest
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( UnifyPropertiesTest
   SCRIPT python -m unittest -v AthenaConfiguration.UnifyProperties
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( AthConfigFlagsTest
   SCRIPT python -m unittest AthenaConfiguration.AthConfigFlags
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( AllConfigFlagsTest_EVNT_test
                SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/python/testAllConfigFlags_EVNT.py
                PROPERTIES TIMEOUT 300  )

if( NOT GENERATIONBASE )
    atlas_add_test( AllConfigFlagsTest_HITS_test
                    SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/python/testAllConfigFlags_HITS.py
                    PROPERTIES TIMEOUT 300  )
endif()

if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
    atlas_add_test( AllConfigFlagsTest_RDO_test
                    SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/python/testAllConfigFlags_RDO.py
                    PROPERTIES TIMEOUT 300  )

    atlas_add_test( AllConfigFlagsTest
                    SCRIPT python -m AthenaConfiguration.AllConfigFlags
                    POST_EXEC_SCRIPT nopost.sh )
endif()

