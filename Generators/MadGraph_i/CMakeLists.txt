################################################################################
# Package: MadGraph_i
################################################################################

# Declare the package name:
atlas_subdir( MadGraph_i )

# External dependencies:
find_package( Pythia6 COMPONENTS pythia6 pythia6_dummy )

# Component(s) in the package:
atlas_add_library( MadGraph_i
   MadGraph_i/*.inc src/*.F
   PUBLIC_HEADERS MadGraph_i
   PRIVATE_INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${PYTHIA6_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/events.lhe )
