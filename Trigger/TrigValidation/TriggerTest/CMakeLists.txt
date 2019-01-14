################################################################################
# Package: TriggerTest
################################################################################

# Declare the package name:
atlas_subdir( TriggerTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/__init__.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( python/RttScriptRunner_TriggerTest.py share/*.cxx python/CheckLog_TriggerTest.py python/LogTail_TriggerTest.py python/CheckForTrigCostD3PD_TriggerTest.py python/CheckForHLTChainAllZero_TriggerTest.py python/CheckForHLTTEAllZero_TriggerTest.py python/CheckForL1AllZero_TriggerTest.py python/CreateIgprofReport_TriggerTest.py python/TruncateLog_TriggerTest.py python/ValGrindPostScript_TriggerTest.py python/ValgrindAnalysisScript_TriggerTest.py Testing/*.conf )
atlas_install_scripts( Testing/*.sh Testing/*.pl test/exec*.sh test/test*sh )

