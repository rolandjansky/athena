################################################################################
# Package: RecExRecoTest
################################################################################

# Declare the package name:
atlas_subdir( RecExRecoTest )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/RecExRecoTest_TestConfiguration.xml python/PostProcessRecoRTTTests.py python/PostProcessIgprofWeb.py share/slurpRecoRTTTests.pl share/dumpPoolFileInchunks.sh share/diffPoolFilesRERT.sh share/run_memleak_rert.sh test/*.sh )

atlas_install_scripts( test/MetaDataTest.py )
atlas_add_alias( metadata-test "MetaDataTest.py" )
