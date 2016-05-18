################################################################################
# Package: MuonRecConfigTests
################################################################################

# Declare the package name:
atlas_subdir( MuonRecConfigTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
#atlas_install_runtime( test/MuonRecConfigTests_TestConfiguration.xml share/*.py )

