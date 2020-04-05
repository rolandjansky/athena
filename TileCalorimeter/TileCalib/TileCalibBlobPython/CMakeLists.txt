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

atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --ignore=F401,F821,ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )
