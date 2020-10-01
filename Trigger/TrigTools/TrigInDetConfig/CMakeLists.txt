################################################################################
# Package: TrigInDetConfig
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( TrigInDetCfg    SCRIPT python -m TrigInDetConfig.TrigInDetConfig    POST_EXEC_SCRIPT nopost.sh )
atlas_add_test( TrigTrackingCutFlags    SCRIPT python -m TrigInDetConfig.TrigTrackingCutFlags    POST_EXEC_SCRIPT nopost.sh )
