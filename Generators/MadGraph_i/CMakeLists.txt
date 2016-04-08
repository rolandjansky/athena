################################################################################
# Package: MadGraph_i
################################################################################

# Declare the package name:
atlas_subdir( MadGraph_i )

# External dependencies:
find_package( Pythia6 )

# tag allowUndefined was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( MadGraph_i
                   src/*.F
                   PUBLIC_HEADERS MadGraph_i
                   PRIVATE_INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${PYTHIA6_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/events.lhe )

