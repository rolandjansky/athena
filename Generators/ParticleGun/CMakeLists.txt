################################################################################
# Package: ParticleGun
################################################################################

# Declare the package name:
atlas_subdir( ParticleGun )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Install files from the package:
atlas_install_joboptions( share/common/*.py
                          share/example/*.py )

