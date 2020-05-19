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
atlas_install_python_modules( python/*.py python/tests python/data
   POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select E11,E402,W1 )

atlas_install_scripts( share/*.py share/*.sh )

# Add tests
atlas_add_test( DQDefects SCRIPT test/DQDefects_test.sh
                LOG_IGNORE_PATTERN "^Ran .* tests in"
                PROPERTIES TIMEOUT 300 )
