################################################################################
# Package: CTB_G4Sim
################################################################################

# Declare the package name:
atlas_subdir( CTB_G4Sim )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( data/*.xml data/*.dtd G4macros/*.mac )

