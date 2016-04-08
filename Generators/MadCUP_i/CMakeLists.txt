################################################################################
# Package: MadCUP_i
################################################################################

# Declare the package name:
atlas_subdir( MadCUP_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Generators/GeneratorFortranCommon )

# External dependencies:
find_package( Herwig )
find_package( Pythia6 )

# Component(s) in the package:
atlas_add_library( MadCUP_i
                   src/*.F
                   PUBLIC_HEADERS MadCUP_i
                   PRIVATE_INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${HERWIG_INCLUDE_DIRS}
                   LINK_LIBRARIES GeneratorFortranCommonLib
                   PRIVATE_LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${HERWIG_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/event_input.dat )

