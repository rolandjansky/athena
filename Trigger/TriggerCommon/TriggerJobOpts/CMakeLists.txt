################################################################################
# Package: TriggerJobOpts
################################################################################

# Declare the package name:
atlas_subdir( TriggerJobOpts )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TriggerCommon/TrigEDMConfig
                          Trigger/TriggerCommon/TrigTier0 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( TriggerConfigFlagsTest
   SCRIPT python -m unittest TriggerJobOpts.TriggerConfigFlags
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TriggerMenuFlagsTest
   SCRIPT python -m unittest TriggerJobOpts.MenuConfigFlags
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TriggerConfigTest
   SCRIPT python -m TriggerJobOpts.TriggerConfig
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )
