################################################################################
# Package: AthenaSealSvc
################################################################################

# Declare the package name:
atlas_subdir( AthenaSealSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaServices )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

