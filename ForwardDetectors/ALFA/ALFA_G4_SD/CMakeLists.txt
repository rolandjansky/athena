################################################################################
# Package: ALFA_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( ALFA_G4_SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          ForwardDetectors/ALFA/ALFA_SimEv
                          Simulation/G4Atlas/G4AtlasTools
                          Tools/AtlasDoxygen )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ALFA_G4_SD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests ALFA_SimEv G4AtlasToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

