# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArConfiguration )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

if( NOT "${CMAKE_PROJECT_NAME}" STREQUAL "AthSimulation" )
  atlas_add_test( LArElecCalibDBConfig_test
                  SCRIPT python -m LArConfiguration.LArElecCalibDBConfig
                  LOG_SELECT_PATTERN "ComponentAccumulator|^---" )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
