################################################################################
# Package: AthenaCommon
################################################################################

# Declare the package name:
atlas_subdir( AthenaCommon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_headers( AthenaCommon )
atlas_install_python_modules( python/*.py python/Utils )
atlas_install_joboptions( share/Bootstrap.py share/Preparation.py share/Execution.py share/Atlas.UnixStandardJob.py test/*.py share/Atlas_Gen.UnixStandardJob.py share/MemTraceInclude.py share/runbatch.py )
atlas_install_scripts( share/athena.py share/athena-app.py share/chappy.py share/find_cfg_dups.py share/test_cfg_pickling.py )

# Aliases:
atlas_add_alias( athena-app "athena-app.py" )
atlas_add_alias( athena "athena.py" )

atlas_add_test( AthAppMgrUnitTests SCRIPT test/test_AthAppMgrUnitTests.sh )
atlas_add_test( ConfigurableUnitTests SCRIPT test/test_ConfigurableUnitTests.sh )
atlas_add_test( JobOptionsUnitTests SCRIPT test/test_JobOptionsUnitTests.sh )
atlas_add_test( JobPropertiesUnitTests SCRIPT test/test_JobPropertiesUnitTests.sh )
atlas_add_test( KeyStoreUnitTests SCRIPT test/test_KeyStoreUnitTests.sh )
