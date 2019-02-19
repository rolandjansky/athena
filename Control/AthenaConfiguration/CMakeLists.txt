################################################################################
# Package: AthenaConfiguration
################################################################################

# Declare the package name:
atlas_subdir( AthenaConfiguration )

# External dependencies:


# Install files from the package:
atlas_install_python_modules( python/*.py python/iconfTool )
atlas_install_runtime(python/*.pkl )
atlas_install_scripts( share/confTool.py python/iconfTool/iconfTool )

atlas_add_test( ComponentAccumulatorTest
   SCRIPT python -m unittest -v AthenaConfiguration.ComponentAccumulator 
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( UnifyPropertiesTest
   SCRIPT python -m unittest -v AthenaConfiguration.UnifyProperties
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( AthConfigFlagsTest
   SCRIPT python -m unittest AthenaConfiguration.AthConfigFlags
   POST_EXEC_SCRIPT nopost.sh )

if( NOT "${CMAKE_PROJECT_NAME}" STREQUAL "AthSimulation" )
    atlas_add_test( AllConfigFlagsTest
    		    SCRIPT python -m AthenaConfiguration.AllConfigFlags
   		    POST_EXEC_SCRIPT nopost.sh )
endif()




# Check python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python 
   POST_EXEC_SCRIPT nopost.sh )
