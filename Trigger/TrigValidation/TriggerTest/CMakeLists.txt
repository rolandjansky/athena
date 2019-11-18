################################################################################
# Package: TriggerTest
################################################################################

# Declare the package name:
atlas_subdir( TriggerTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.cxx Testing/*.conf )
atlas_install_scripts( test/exec*.sh test/test*.sh test/test*.py )

# Unit tests:
atlas_add_test( flake8_test_dir
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/test
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
