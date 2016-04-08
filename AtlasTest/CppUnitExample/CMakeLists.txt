################################################################################
# Package: CppUnitExample
################################################################################

# Declare the package name:
atlas_subdir( CppUnitExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasTest/TestTools
                          TestPolicy )

# Install files from the package:
atlas_install_headers( CppUnitExample )

