################################################################################
# Package: RunDependentSimData
################################################################################

# Declare the package name:
atlas_subdir( RunDependentSimData )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/rundmc_printdist )

