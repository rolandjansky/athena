################################################################################
# Package: MuonCnvExample
################################################################################

# Declare the package name:
atlas_subdir( MuonCnvExample )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.data )

