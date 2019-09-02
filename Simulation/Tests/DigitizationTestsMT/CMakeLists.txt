################################################################################
# Package: DigitizationTestsMT
################################################################################

# Declare the package name:
atlas_subdir( DigitizationTestsMT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

atlas_install_scripts( test/*.sh )

