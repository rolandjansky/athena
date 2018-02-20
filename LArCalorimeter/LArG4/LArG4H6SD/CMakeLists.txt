################################################################################
# Package: LArG4H6SD
################################################################################

# Declare the package name:
atlas_subdir( LArG4H6SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloG4Sim
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/Identifier
                          Generators/GeneratorObjects
                          LArCalorimeter/LArG4/LArG4Code
                          LArCalorimeter/LArG4/LArG4RunControl
                          LArCalorimeter/LArG4TB/LArG4TBSimEvent
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          LArCalorimeter/LArSimEvent
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/SimHelpers
                          Simulation/HitManagement
                          TestBeam/TBEvent
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Geant4 )
find_package( HepMC )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LArG4H6SD
                     src/*.cxx
                     src/*.cc
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${GEANT4_LIBRARIES} GaudiKernel CaloG4SimLib CaloSimEvent AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests GeoModelUtilities Identifier GeneratorObjects LArG4Code LArG4RunControl LArG4TBSimEvent LArReadoutGeometry LArSimEvent G4AtlasInterfaces G4AtlasToolsLib SimHelpers HitManagement TBEvent PathResolver )

# Install files from the package:
atlas_install_headers( LArG4H6SD )
atlas_install_python_modules( python/*.py )

