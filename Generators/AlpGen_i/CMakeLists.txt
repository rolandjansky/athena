################################################################################
# Package: AlpGen_i
################################################################################

# Declare the package name:
atlas_subdir( AlpGen_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Generators/GeneratorFortranCommon )

# External dependencies:
find_package( Pythia6 )

# tag allowUndefined was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( AlpGen_i
                   src/*.F
                   PUBLIC_HEADERS AlpGen_i
                   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} GeneratorFortranCommonLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

