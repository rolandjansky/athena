################################################################################
# Package: CompHep_i
################################################################################

# Declare the package name:
atlas_subdir( CompHep_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Generators/GeneratorFortranCommon )

# External dependencies:
find_package( Lhapdf )
find_package( Pythia6 )

# tag allowUndefined was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( CompHep_i
                   src/*.F
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} GeneratorFortranCommonLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/Mixed.PEV share/inparmCompHep.dat )

