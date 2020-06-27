################################################################################
# Package: H6G4Sim
################################################################################

# Declare the package name:
atlas_subdir( H6G4Sim )

# Install files from the package:
atlas_install_python_modules( python/h6prod_getxy.py python/__init__.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F403,F405,F821 )
atlas_install_runtime( G4macros/*.mac )
atlas_install_scripts( share/cbtG4_trf.py )
