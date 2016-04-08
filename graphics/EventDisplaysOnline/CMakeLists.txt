################################################################################
# Package: EventDisplaysOnline
################################################################################

# Declare the package name:
atlas_subdir( EventDisplaysOnline )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.sh )

