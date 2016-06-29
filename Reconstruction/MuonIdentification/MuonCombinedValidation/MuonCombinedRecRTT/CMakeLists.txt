################################################################################
# Package: MuonCombinedRecRTT
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedRecRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_joboptions( share/*.py )
#atlas_install_runtime( test/MuonCombinedRecRTT_TestConfiguration.xml share/*.C share/*.h share/*.py ExtraFiles/*.html )

