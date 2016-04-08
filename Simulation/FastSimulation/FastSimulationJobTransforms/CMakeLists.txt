################################################################################
# Package: FastSimulationJobTransforms
################################################################################

# Declare the package name:
atlas_subdir( FastSimulationJobTransforms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/PyJobTransformsCore )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.db share/*.root )

