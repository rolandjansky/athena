################################################################################
# Package: ISF_Config
################################################################################

# Declare the package name:
atlas_subdir( ISF_Config )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

