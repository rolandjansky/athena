################################################################################
# Package: TrkConfig
################################################################################

# Declare the package name:
atlas_subdir( TrkConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py)
atlas_install_data( share/*.ref )


atlas_add_test( TrackCollectionReadConfig_test
                SCRIPT python -m TrkConfig.TrackCollectionReadConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---" )

atlas_add_test( AtlasTrackingGeometrySvcCfgTest    
                SCRIPT python -m TrkConfig.AtlasTrackingGeometrySvcConfig
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( SolenoidalIntersectorConfig_test
                SCRIPT python -m TrkConfig.SolenoidalIntersectorConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^private tools" )
