################################################################################
# Package: MuonHolesValidationRTT
################################################################################

# Declare the package name:
atlas_subdir( MuonHolesValidationRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
#atlas_install_runtime( test/MuonHolesValidationRTT_TestConfiguration.xml )

