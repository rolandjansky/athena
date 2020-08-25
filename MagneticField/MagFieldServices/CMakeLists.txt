# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MagFieldServices )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MagFieldServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaPoolUtilities CxxUtils EventInfo GaudiKernel MagFieldConditions MagFieldElements MagFieldInterfaces PathResolver StoreGateLib )

# Install files from the package:
atlas_install_headers( MagFieldServices )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

if( NOT SIMULATIONBASE )
  atlas_add_test( MagFieldServicesConfig
                  SCRIPT python -m MagFieldServices.MagFieldServicesConfig
                  PROPERTIES TIMEOUT 300
                  POST_EXEC_SCRIPT nopost.sh )

  atlas_add_test( MagFieldCondAlg
                  SCRIPT athena --threads=1 ${CMAKE_CURRENT_SOURCE_DIR}/share/test_magFieldCondAlg.py
                  PROPERTIES TIMEOUT 300
                  LOG_SELECT_PATTERN "MagFieldCondReader" )
endif()
