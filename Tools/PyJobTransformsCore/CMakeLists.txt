################################################################################
# Package: PyJobTransformsCore
################################################################################

# Declare the package name:
atlas_subdir( PyJobTransformsCore )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/checklog.py share/find_*.py share/trf_ls POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/slimmetadata )
atlas_install_generic( share/*.db
                       DESTINATION share
                       EXECUTABLE )

