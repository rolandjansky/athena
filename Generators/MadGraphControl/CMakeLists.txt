################################################################################
# Package: MadGraphControl
################################################################################

# Declare the package name:
atlas_subdir( MadGraphControl )

# External dependencies:
find_package( MadGraph )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_generic( share/*.dat share/lhapdf-config share/fastjet-config
                       DESTINATION share
                       EXECUTABLE )

