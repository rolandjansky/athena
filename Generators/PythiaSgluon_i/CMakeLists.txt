################################################################################
# Package: PythiaSgluon_i
################################################################################

# Declare the package name:
atlas_subdir( PythiaSgluon_i )

# External dependencies:
find_package( Pythia6 )

# tag $(as_needed_tag) was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( PythiaSgluon_i
                   src/*.F
                   PUBLIC_HEADERS PythiaSgluon_i
                   PRIVATE_INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${PYTHIA6_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/sgluons.dat )

