################################################################################
# Package: AtlasGeoModel
################################################################################

# Declare the package name:
atlas_subdir( AtlasGeoModel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py )

atlas_add_test( EVNT_InputGeo_test
                SCRIPT test/AtlasGeometryConfig_EVNT_test.py
                PROPERTIES TIMEOUT 300 )

if( NOT SIMULATIONBASE )
  atlas_add_test( AtlasGeoModelConfig    SCRIPT python -m AtlasGeoModel.GeoModelConfig POST_EXEC_SCRIPT nopost.sh )
  atlas_add_test( HITS_InputGeo_test
                  SCRIPT test/AtlasGeometryConfig_HITS_test.py
                  PROPERTIES TIMEOUT 300 )
  atlas_add_test( AOD_InputGeo_test
                  SCRIPT test/AtlasGeometryConfig_AOD_test.py
                  PROPERTIES TIMEOUT 300 )
endif()
