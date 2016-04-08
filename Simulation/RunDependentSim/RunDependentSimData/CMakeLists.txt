################################################################################
# Package: RunDependentSimData
################################################################################

# Declare the package name:
atlas_subdir( RunDependentSimData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/MinimalRunTime )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/rundmc_printdist )

