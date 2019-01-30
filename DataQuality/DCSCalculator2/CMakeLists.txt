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

# Install files from the package:
atlas_install_python_modules( python/*.py python/subdetectors )
atlas_install_scripts( share/*.py )

