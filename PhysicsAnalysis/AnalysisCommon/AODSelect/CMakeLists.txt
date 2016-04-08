################################################################################
# Package: AODSelect
################################################################################

# Declare the package name:
atlas_subdir( AODSelect )

# Install files from the package:
atlas_install_headers( AODSelect )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/PrintAODSelect.py )

