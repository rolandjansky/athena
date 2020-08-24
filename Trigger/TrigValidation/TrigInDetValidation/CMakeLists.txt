################################################################################
# Package: TrigInDetValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetValidation )

# External dependencies:
find_package( requests )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/TrigInDetValidation_*.py share/Cosmic.py )
atlas_install_runtime( test/TrigInDetValidation_TestConfiguration.xml share/TrigInDetValidation_*.py TIDAbuild )

atlas_install_scripts( scripts/TIDA*.py test/test*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=ATL902 )

# Unit test for python test scripts:
atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
