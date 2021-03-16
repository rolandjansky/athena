# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AtlasGeoModel )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

if( NOT GENERATIONBASE )
  atlas_add_test( EVNT_InputGeo_test
                  SCRIPT test/AtlasGeometryConfig_EVNT_test.py
                  PROPERTIES TIMEOUT 300 )
endif()

if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
  atlas_add_test( AtlasGeoModelConfig    SCRIPT python -m AtlasGeoModel.GeoModelConfig POST_EXEC_SCRIPT nopost.sh )
  atlas_add_test( HITS_InputGeo_test
                  SCRIPT test/AtlasGeometryConfig_HITS_test.py
                  PROPERTIES TIMEOUT 600 )
  atlas_add_test( AOD_InputGeo_test
                  SCRIPT test/AtlasGeometryConfig_AOD_test.py
                  PROPERTIES TIMEOUT 900 )
endif()
