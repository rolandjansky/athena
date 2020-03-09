################################################################################
# Package: TrigEDMConfig
################################################################################

# Declare the package name:
atlas_subdir( TrigEDMConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test( UnitTest SCRIPT python/testEDM.py )
atlas_add_test( UnitTestRun3 SCRIPT python/testEDMRun3.py )

# Check python code:
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 --extend-ignore=E701,E741 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )
