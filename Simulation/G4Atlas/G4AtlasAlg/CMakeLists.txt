################################################################################
# Package: G4AtlasAlg
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/FADS/FadsKinematics
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventInfo
                          Generators/GeneratorObjects
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/MCTruthBase
                          Simulation/G4Sim/SimHelpers )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepMC )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( G4AtlasAlgLib
                   src/*.cxx
                   PUBLIC_HEADERS G4AtlasAlg
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel G4AtlasInterfaces FadsKinematics G4AtlasToolsLib StoreGateLib SGtests MCTruthBaseLib
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} SGTools EventInfo GeneratorObjects MCTruth SimHelpers )

atlas_add_component( G4AtlasAlg
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel G4AtlasInterfaces G4AtlasToolsLib FadsKinematics SGTools StoreGateLib SGtests EventInfo GeneratorObjects MCTruth MCTruthBaseLib SimHelpers G4AtlasAlgLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

