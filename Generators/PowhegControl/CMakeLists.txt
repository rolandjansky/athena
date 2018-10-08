################################################################################
# Package: PowhegControl
################################################################################

# Declare the package name:
atlas_subdir( PowhegControl )

# Install files from the package:
atlas_install_python_modules( python/*.py python/algorithms python/decorators python/parameters python/processes python/utility )
atlas_install_joboptions( share/*.py )

