################################################################################
# Package: CaloG4Sim
################################################################################

# Declare the package name:
atlas_subdir( CaloG4Sim )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( CaloG4SimLib
                   src/*.cc
                   src/*.cxx
                   PUBLIC_HEADERS CaloG4Sim
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps GaudiKernel G4AtlasInterfaces G4AtlasToolsLib
                   PRIVATE_LINK_LIBRARIES CxxUtils MCTruth )

atlas_add_component( CaloG4Sim
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps GaudiKernel G4AtlasInterfaces G4AtlasToolsLib CxxUtils CaloG4SimLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

