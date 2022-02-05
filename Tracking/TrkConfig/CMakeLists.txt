# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_data( share/*.ref )


atlas_add_test( TrackCollectionReadConfig_test
                SCRIPT python -m TrkConfig.TrackCollectionReadConfig
                LOG_SELECT_PATTERN "^---" )

atlas_add_test( AtlasTrackingGeometrySvcCfgTest    
                SCRIPT python -m TrkConfig.AtlasTrackingGeometrySvcConfig
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( SolenoidalIntersectorConfig_test
                SCRIPT python -m TrkConfig.SolenoidalIntersectorConfig
                LOG_SELECT_PATTERN "^private tools" )

atlas_add_test( AtlasExtrapolatorCfgTest    
                SCRIPT python -m TrkConfig.AtlasExtrapolatorConfig
                POST_EXEC_SCRIPT nopost.sh )
