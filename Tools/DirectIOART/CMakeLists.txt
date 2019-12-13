################################################################################
# Package: DirectIOART
################################################################################

# Declare the package name:
atlas_subdir( DirectIOART  )

# Install files from the package:
atlas_install_scripts( test/*.sh test/*.py )

# Install joboptions
atlas_install_joboptions( share/*.py )
