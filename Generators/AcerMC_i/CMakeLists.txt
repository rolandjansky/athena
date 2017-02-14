################################################################################
# Package: AcerMC_i
################################################################################

# Declare the package name:
atlas_subdir( AcerMC_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Generators/GeneratorFortranCommon )

# External dependencies:
find_package( Herwig )
find_package( Pythia6 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( AcerMC_i
   AcerMC_i/*.h AcerMC_i/*.icc AcerMC_i/*.inc src/*.cxx src/*.F
   PUBLIC_HEADERS AcerMC_i
   PRIVATE_INCLUDE_DIRS ${HERWIG_INCLUDE_DIRS} ${PYTHIA6_INCLUDE_DIRS}
   LINK_LIBRARIES GeneratorFortranCommonLib
   PRIVATE_LINK_LIBRARIES ${HERWIG_LIBRARIES} ${PYTHIA6_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.events share/*.dat )
