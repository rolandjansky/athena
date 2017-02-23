################################################################################
# Package: Protos_i
################################################################################

# Declare the package name:
atlas_subdir( Protos_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Generators/GeneratorFortranCommon )

# The used external(s):
find_package( Pythia6 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Protos_i
   Protos_i/*.inc src/*.F
   PUBLIC_HEADERS Protos_i
   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS}
   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} GeneratorFortranCommonLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.events share/*.dat )
