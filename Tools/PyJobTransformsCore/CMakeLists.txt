################################################################################
# Package: PyJobTransformsCore
################################################################################

# Declare the package name:
atlas_subdir( PyJobTransformsCore )

# External dependencies:
find_package( PythonLibs )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/slimmetadata share/checklog.py share/find_*.py share/trf_ls python/envutil.py )
atlas_install_generic( share/*.db
                       DESTINATION share
                       EXECUTABLE )

