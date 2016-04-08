################################################################################
# Package: RecExAnaTest
################################################################################

# Declare the package name:
atlas_subdir( RecExAnaTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/RecExAnaTest_TestConfiguration.xml share/run_memleak_reat.sh )

