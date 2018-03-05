################################################################################
# Package: TrigInDetValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/TrigInDetValidation_RTT_*.py )
atlas_install_runtime( test/TrigInDetValidation_TestConfiguration.xml share/TrigInDetValidation_RTT_*.py TIDAbuild )

atlas_install_scripts( scripts/TIDA*.py )
