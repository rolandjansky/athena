################################################################################
# Package: AtlasGeoModel
################################################################################

# Declare the package name:
atlas_subdir( AtlasGeoModel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

if( NOT SIMULATIONBASE )
  atlas_add_test( AtlasGeoModelConfig    SCRIPT python -m AtlasGeoModel.GeoModelConfig POST_EXEC_SCRIPT nopost.sh )
endif()
