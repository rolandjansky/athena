################################################################################
# Package: LUCID_G4_SD
################################################################################

# Declare the package name:
atlas_subdir( LUCID_G4_SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          ForwardDetectors/LUCID/LUCID_GeoModel
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/HitManagement
                          Tools/AtlasDoxygen )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LUCID_G4_SD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CxxUtils StoreGateLib SGtests LUCID_GeoModelLib LUCID_SimEvent G4AtlasToolsLib HitManagement )

# Install files from the package:
atlas_install_python_modules( python/*.py )

