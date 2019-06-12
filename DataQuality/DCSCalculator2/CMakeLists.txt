################################################################################
# Package: DCSCalculator2
################################################################################

# Declare the package name:
atlas_subdir( DCSCalculator2 )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/CoolRunQuery
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          DataQuality/DQDefects
                          DataQuality/DQUtils )

# External dependencies:
find_package( sqlalchemy )

# Install files from the package:
atlas_install_python_modules( python/*.py python/subdetectors )
atlas_install_scripts( share/*.py )

# Code quality check
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,E11,E402,E71,E72,E9,W1,W6,F ${CMAKE_CURRENT_SOURCE_DIR}/python/
   POST_EXEC_SCRIPT nopost.sh )
