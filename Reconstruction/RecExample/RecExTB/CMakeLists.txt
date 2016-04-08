################################################################################
# Package: RecExTB
################################################################################

# Declare the package name:
atlas_subdir( RecExTB )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/*.xml )

