################################################################################
# Package: TrigP1Test
################################################################################

# Declare the package name:
atlas_subdir( TrigP1Test )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigValidation/TrigValTools )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref )
atlas_install_runtime( Testing/*.trans Testing/*.conf )
atlas_install_scripts( bin/*.py test/test*.sh test/test*.py)

# Unit tests:
atlas_add_test( flake8_test_dir
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/test
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                POST_EXEC_SCRIPT nopost.sh )
