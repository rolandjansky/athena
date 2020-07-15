################################################################################
# Package: MCTruthAlgs
################################################################################

# Declare the package name:
atlas_subdir( MCTruthAlgs )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_library( MCTruthAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS MCTruthAlgs
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${HEPPDT_LIBRARIES} AthenaBaseComps GaudiKernel TrackRecordLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} )

atlas_add_component( MCTruthAlgs
                     src/components/*.cxx
                     LINK_LIBRARIES MCTruthAlgsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

