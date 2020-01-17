################################################################################
# Package: InDetConfiguration
################################################################################

# Declare the package name:
atlas_subdir( InDetConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_data( share/*.ref )
