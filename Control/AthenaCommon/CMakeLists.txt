################################################################################
# Package: AthenaCommon
################################################################################

# Declare the package name:
atlas_subdir( AthenaCommon )

# External dependencies:
find_package( six )

# Install files from the package:
atlas_install_python_modules( python/*.py python/Utils
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/Preparation.py share/Execution.py share/Atlas.UnixStandardJob.py test/*.py 
                          share/zeroJO.py share/Atlas_Gen.UnixStandardJob.py share/MemTraceInclude.py share/runbatch.py)
atlas_install_scripts( share/athena.py share/athena_preload.sh share/chappy.py share/test_cfg_pickling.py share/ThinCAWrapper.sh)
atlas_install_runtime(share/*.pkl)

# Aliases:
atlas_add_alias( athena "athena.py" )

atlas_add_test( AthAppMgrUnitTests SCRIPT test/test_AthAppMgrUnitTests.sh
                PROPERTIES TIMEOUT 300
                LOG_IGNORE_PATTERN "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in|built on" )
atlas_add_test( ConfigurableUnitTests SCRIPT test/test_ConfigurableUnitTests.sh
                PROPERTIES TIMEOUT 300
                LOG_IGNORE_PATTERN "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in" )
atlas_add_test( JobOptionsUnitTests SCRIPT test/test_JobOptionsUnitTests.sh 
                LOG_IGNORE_PATTERN "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in" )
atlas_add_test( JobPropertiesUnitTests SCRIPT test/test_JobPropertiesUnitTests.sh
                LOG_IGNORE_PATTERN "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in" )
atlas_add_test( KeyStoreUnitTests SCRIPT test/test_KeyStoreUnitTests.sh
                LOG_IGNORE_PATTERN "Warning in <TFile::Init>: no StreamerInfo found|^Ran .* tests in|^outFileName: " )
atlas_add_test( CFElementsTest SCRIPT python -m unittest -v AthenaCommon.CFElements
		POST_EXEC_SCRIPT nopost.sh ) 

atlas_add_test( GenerateBootstrapTest
   SCRIPT test/test_gen_bootstrap.sh
   PROPERTIES TIMEOUT 300
   #ignore location of bootstrap file and whether it was already downloaded
   LOG_IGNORE_PATTERN ".*copy bootstrap.*pkl|.*\/share\/bootstrap.*pkl|File.*exists in the current directory|^Willing to acquire"
   )
