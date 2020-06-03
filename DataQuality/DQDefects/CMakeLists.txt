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
find_package( pytools )

# tag NEEDS_PYCOOL was not recognized in automatic conversion in cmt2cmake

# tag NEEDS_COOL_FACTORY was not recognized in automatic conversion in cmt2cmake

# Install files from the package:
atlas_install_python_modules( python/*.py python/tests python/data )
atlas_install_scripts( share/*.py share/*.sh )

# Add tests
atlas_add_test( DQDefects SCRIPT test/DQDefects_test.sh
                EXTRA_PATTERNS "^Ran .* tests in"
                PROPERTIES TIMEOUT 300 )
