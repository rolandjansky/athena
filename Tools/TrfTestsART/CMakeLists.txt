################################################################################
# Package: TrfTestsART
################################################################################

# Declare the package name:
atlas_subdir( TrfTestsART )

# Install files from the package:
atlas_install_runtime( test/*.sh )
atlas_install_scripts( test/*.sh test/*.py )
