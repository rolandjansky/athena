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
atlas_install_joboptions( share/Preparation.py share/Execution.py share/Atlas.UnixStandardJob.py test/*.py share/Atlas_Gen.UnixStandardJob.py share/MemTraceInclude.py share/runbatch.py )
atlas_install_scripts( share/athena.py share/athena_preload.sh share/chappy.py share/find_cfg_dups.py share/test_cfg_pickling.py )

# Aliases:
atlas_add_alias( athena "athena.py" )

atlas_add_test( AthAppMgrUnitTests SCRIPT test/test_AthAppMgrUnitTests.sh
                EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in|built on" )
atlas_add_test( ConfigurableUnitTests SCRIPT test/test_ConfigurableUnitTests.sh
                EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in" )
atlas_add_test( JobOptionsUnitTests SCRIPT test/test_JobOptionsUnitTests.sh 
                EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in" )
atlas_add_test( JobPropertiesUnitTests SCRIPT test/test_JobPropertiesUnitTests.sh
                EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in" )
atlas_add_test( KeyStoreUnitTests SCRIPT test/test_KeyStoreUnitTests.sh
                EXTRA_PATTERNS "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in|^outFileName: " )
atlas_add_test( CFElementsTest SCRIPT python -m unittest -v AthenaCommon.CFElements
		POST_EXEC_SCRIPT nopost.sh ) 

# Check python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=F,E101,E112,E113,E7,E9,W6 --ignore=E701,E741 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )
