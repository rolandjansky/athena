################################################################################
# Package: Matchig_i
################################################################################

# Declare the package name:
atlas_subdir( Matchig_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Generators/GeneratorFortranCommon )

# External dependencies:
find_package( Pythia6 )

# Component(s) in the package:
atlas_add_library( Matchig_i
                   src/*.F
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} GeneratorFortranCommonLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

