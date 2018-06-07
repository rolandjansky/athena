################################################################################
# Package: G4ExternalDecay
################################################################################

# Declare the package name:
atlas_subdir( G4ExternalDecay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Generators/Pythia_i )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepMC )
find_package( Lhapdf )
find_package( Pythia6 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( G4ExternalDecay
                   src/*.cxx
                   PUBLIC_HEADERS G4ExternalDecay
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${GEANT4_LIBRARIES} Pythia_iLib
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} )

