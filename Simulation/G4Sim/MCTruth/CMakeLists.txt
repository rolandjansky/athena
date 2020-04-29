################################################################################
# Package: MCTruth
################################################################################

# Declare the package name:
atlas_subdir( MCTruth )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Generators/AtlasHepMC
                          Generators/GeneratorObjects
                          PRIVATE
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/G4Sim/SimHelpers )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( MCTruth
                   src/*.cxx
                   PUBLIC_HEADERS MCTruth
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaKernel GeneratorObjects
                   PRIVATE_LINK_LIBRARIES ISF_Event SimHelpers )
