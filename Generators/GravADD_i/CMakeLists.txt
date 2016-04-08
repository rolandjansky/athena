################################################################################
# Package: GravADD_i
################################################################################

# Declare the package name:
atlas_subdir( GravADD_i )

# External dependencies:
find_package( GSL )
find_package( Lhapdf )
find_package( Pythia6 )

# tag allowUndefined was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( GravADD_i
                   src/*.cxx
                   src/*.F
                   PUBLIC_HEADERS GravADD_i
                   INCLUDE_DIRS ${GSL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GSL_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )

