################################################################################
# Package: LArConfiguration
################################################################################

# Declare the package name:
atlas_subdir( LArConfiguration )

# Install files from the package:
atlas_install_python_modules( python/*.py )

if( NOT "${CMAKE_PROJECT_NAME}" STREQUAL "AthSimulation" )
  atlas_add_test( LArElecCalibDBConfig_test
                  SCRIPT python -m LArConfiguration.LArElecCalibDBConfig
                  EXTRA_PATTERNS "-s ComponentAccumulator|^---" )
endif()

