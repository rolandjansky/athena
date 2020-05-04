################################################################################
# Package: TrigUpgradeTest
################################################################################

# Declare the package name:
atlas_subdir( TrigUpgradeTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          Control/AthenaBaseComps
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigSteer/TrigCompositeUtils
                          Trigger/TrigEvent/TrigSteeringEvent
                          )

atlas_add_component( TrigUpgradeTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrigSteeringEvent DecisionHandlingLib TrigCompositeUtilsLib
                     )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref share/*.conf )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/exec*.sh test/test*.sh )

# Check python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 --extend-ignore=E701 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )

# Unit tests (they test things from outside TrigUpgradeTest - should be moved or removed):
atlas_add_test( ViewSchedule1  SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=1 POST_EXEC_SCRIPT nopost.sh )
atlas_add_test( ViewSchedule2  SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=2 POST_EXEC_SCRIPT nopost.sh )
atlas_add_test( ViewSchedule64 SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=64 POST_EXEC_SCRIPT nopost.sh )

# Unit tests of the test scripts
atlas_add_test( flake8_test_dir
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/test
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
