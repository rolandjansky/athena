################################################################################
# Package: AthenaConfiguration
################################################################################

# Declare the package name:
atlas_subdir( AthenaConfiguration )

# External dependencies:


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime(python/*.pkl )
atlas_install_scripts( share/confTool.py )

atlas_add_test( ComponentAccumulatorTest
   SCRIPT python -m AthenaConfiguration.ComponentAccumulator )

# Check python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python 
   POST_EXEC_SCRIPT nopost.sh )
