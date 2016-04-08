################################################################################
# Package: PmbUtils
################################################################################

# Declare the package name:
atlas_subdir( PmbUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/CoolDozer
                          Tools/PyUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py share/pmb-rtt-domain-callgrind )

