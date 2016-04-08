################################################################################
# Package: ParticleBuilderOptions
################################################################################

# Declare the package name:
atlas_subdir( ParticleBuilderOptions )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/AODFlags.py )
atlas_install_joboptions( share/*.py )

