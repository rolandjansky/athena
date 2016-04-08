################################################################################
# Package: McAtNlo_i
################################################################################

# Declare the package name:
atlas_subdir( McAtNlo_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Generators/GeneratorFortranCommon )

# External dependencies:
find_package( Herwig )
find_package( Pythia6 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( McAtNlo_i
                   src/*.F
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${HERWIG_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${HERWIG_LIBRARIES} GeneratorFortranCommonLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/inparmMcAtNlo.dat share/tt.events )

