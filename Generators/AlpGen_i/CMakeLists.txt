################################################################################
# Package: AlpGen_i
################################################################################

# Declare the package name:
atlas_subdir( AlpGen_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Generators/GeneratorFortranCommon )

# External dependencies:
find_package( Pythia6 )
find_package( AlpGen COMPONENTS alpgen atoher )
find_package( Tauola )

# Component(s) in the package:
atlas_add_library( AlpGen_i
   AlpGen_i/*.inc src/*.F
   PUBLIC_HEADERS AlpGen_i
   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${ALPGEN_INCLUDE_DIRS}
   ${TAUOLA_INCLUDE_DIRS}
   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${ALPGEN_LIBRARIES}
   ${TAUOLA_LIBRARIES} GeneratorFortranCommonLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
