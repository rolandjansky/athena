################################################################################
# Package: PowhegControl
################################################################################

# Declare the package name:
atlas_subdir( PowhegControl )

# Install files from the package:
atlas_install_python_modules( python/*.py python/decorators python/processes python/strategies python/utility )
atlas_install_joboptions( share/*.py )

