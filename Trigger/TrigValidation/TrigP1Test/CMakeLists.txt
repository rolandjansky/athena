################################################################################
# Package: TrigP1Test
################################################################################

# Declare the package name:
atlas_subdir( TrigP1Test )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy
                          Tools/RunTimeTester
                          Trigger/TrigValidation/TrigValTools )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref )

atlas_install_runtime( test/TrigP1Test_TestConfiguration.xml Testing/*.trans Testing/*.conf python/RttScriptRunner.py python/RootComp.py python/RegTest.py python/CheckLog.py python/CheckStatusCode.py python/LogTail.py python/DeleteRaw.py python/TruncateLog.py python/releaseFromSMK.py)
atlas_install_scripts( bin/*py share/*.sh share/testMonHistOH.py share/part_lhl2ef_fix.py share/setMagFieldCurrents.py share/part_get_runnumber.py share/trigp1test_athenaHLT.py share/trigp1test_prescaleForROSsim.py Testing/*.sh Testing/splitlog.py test/exec*.sh test/test*.sh test/test*.py)

# Unit tests:
atlas_add_test( flake8_test_dir
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/test
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TrigValSteeringUT
                SCRIPT trigvalsteering-unit-tester.py ${CMAKE_CURRENT_SOURCE_DIR}/test
                POST_EXEC_SCRIPT nopost.sh )
