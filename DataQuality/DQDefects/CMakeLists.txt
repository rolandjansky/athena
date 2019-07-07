################################################################################
# Package: DQDefects
################################################################################

# Declare the package name:
atlas_subdir( DQDefects )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          DataQuality/DQUtils )

# External dependencies:
find_package( Oracle )
find_package( pyyaml )

# Install files from the package:
atlas_install_python_modules( python/*.py python/tests python/data )
atlas_install_scripts( share/*.py share/*.sh )

# Add tests
atlas_add_test( DQDefects SCRIPT test/DQDefects_test.sh
                EXTRA_PATTERNS "^Ran .* tests in"
                PROPERTIES TIMEOUT 300 )
		
# Code quality check
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,E11,E402,E71,E72,E9,W1,W6,F ${CMAKE_CURRENT_SOURCE_DIR}/python/
   POST_EXEC_SCRIPT nopost.sh )
