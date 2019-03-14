################################################################################
# Package: TileCalibBlobPython
################################################################################

# Declare the package name:
atlas_subdir( TileCalibBlobPython )

# Necessary external(s):
find_package( cx_Oracle )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.py )

