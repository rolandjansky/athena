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
find_package( Pythia8 )
find_package( XercesC )
find_package( HEPUtils )
find_package( MCUtils )

set( extra_includes )
set( extra_libs )
if( HEPUTILS_FOUND )
   list( APPEND extra_includes ${HEPUTILS_INCLUDE_DIRS} )
   list( APPEND extra_libs ${HEPUTILS_LIBRARIES} )
endif()
if( MCUTILS_FOUND )
   list( APPEND extra_includes ${MCUTILS_INCLUDE_DIRS} )
   list( APPEND extra_libs ${MCUTILS_LIBRARIES} )
endif()
if( HEPMC_FOUND )
   list( APPEND extra_includes ${HEPMC_INCLUDE_DIRS} )
   list( APPEND extra_libs ${HEPMC_LIBRARIES} )
endif()


# Component(s) in the package:
atlas_add_library( G4ExternalDecay
                   src/*.cxx
                   PUBLIC_HEADERS G4ExternalDecay
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${extra_includes}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA6_LIBRARIES} ${PYTHIA8_LIBRARIES} ${LHAPDF_LIBRARIES} ${GEANT4_LIBRARIES} Pythia_iLib Pythia8_iLib ${extra_libs}
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} )

