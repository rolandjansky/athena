################################################################################
# Package: TrigInDetConfig
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test( TrigInDetCfg    SCRIPT python -m TrigInDetConfig.TrigInDetConfig    POST_EXEC_SCRIPT nopost.sh )
