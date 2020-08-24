################################################################################
# Package: FastSimulationJobTransforms
################################################################################

# Declare the package name:
atlas_subdir( FastSimulationJobTransforms )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.db share/*.root )

