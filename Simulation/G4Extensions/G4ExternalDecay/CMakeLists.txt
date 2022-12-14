################################################################################
# Package: G4ExternalDecay
################################################################################

# Declare the package name:
atlas_subdir( G4ExternalDecay )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( Lhapdf )
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
   list( APPEND extra_includes  )
   list( APPEND extra_libs AtlasHepMCLib )
endif()

# Component(s) in the package:
atlas_add_library( G4ExternalDecay
                   src/*.cxx
                   OBJECT
                   PUBLIC_HEADERS G4ExternalDecay
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${extra_includes}
                   PRIVATE_INCLUDE_DIRS 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} ${LHAPDF_LIBRARIES} ${GEANT4_LIBRARIES} Pythia8_iLib ${extra_libs}
                   PRIVATE_LINK_LIBRARIES AtlasHepMCLib )
