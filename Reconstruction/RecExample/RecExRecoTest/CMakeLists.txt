################################################################################
# Package: RecExRecoTest
################################################################################

# Declare the package name:
atlas_subdir( RecExRecoTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/RecExRecoTest_TestConfiguration.xml python/PostProcessRecoRTTTests.py python/PostProcessIgprofWeb.py share/slurpRecoRTTTests.pl share/dumpPoolFileInchunks.sh share/diffPoolFilesRERT.sh share/run_memleak_rert.sh )

