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
find_package( ipython )

# Install files from the package:
atlas_install_python_modules( python/*.py python/subdetectors POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/*.py )

# Test: does DCS Calculator work?
atlas_add_test( DCSCRun
   SCRIPT dcsc.py -r348885 -d 'sqlite://$<SEMICOLON>schema=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/AthDataQuality/dcscalc_input_348885.db$<SEMICOLON>dbname=CONDBR2'
   PROPERTIES TIMEOUT 360
   )
