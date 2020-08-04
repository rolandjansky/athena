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
  atlas_add_test( MagFieldServicesConfig    SCRIPT python -m MagFieldServices.MagFieldServicesConfig
                  PROPERTIES TIMEOUT 300
                  POST_EXEC_SCRIPT nopost.sh )
endif()
