################################################################################
# Package: G4AtlasAlg
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventInfo
                          Generators/GeneratorObjects
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Sim/MCTruthBase
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/Barcode/BarcodeInterfaces)

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( HepMC )
find_package( XercesC )
find_package( Eigen )

# G4AtlasAlgLib library

atlas_add_library( G4AtlasAlgLib
                     src/*.cxx
                     PUBLIC_HEADERS G4AtlasAlg
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
 ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel G4AtlasInterfaces SGTools StoreGateLib SGtests EventInfo GeneratorObjects MCTruthBaseLib )

# Component(s) in the package:
atlas_add_component( G4AtlasAlg
                     src/components/*.cxx
                     PUBLIC_HEADERS G4AtlasAlg
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel G4AtlasInterfaces G4AtlasAlgLib SGTools StoreGateLib SGtests EventInfo GeneratorObjects MCTruthBaseLib )

#Test G4AtlasAlg
#atlas_add_test( G4AtlasAlgConfigNew_Test
#                SCRIPT test/G4AtlasAlgConfigNew_Test.py
#                PROPERTIES TIMEOUT 300 )


# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

