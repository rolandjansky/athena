################################################################################
# Package: ExoGraviton_i
################################################################################

# Declare the package name:
atlas_subdir( ExoGraviton_i )

# External dependencies:
find_package( Pythia6 )

# tag allowUndefined was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( ExoGraviton_i
                   src/*.cxx
                   src/*.F
                   PUBLIC_HEADERS ExoGraviton_i
                   PRIVATE_INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${PYTHIA6_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )

