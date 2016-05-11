################################################################################
# Package: G4AtlasTests
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloSimEvent
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoAdaptors
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventInfo
                          ForwardDetectors/ALFA/ALFA_SimEv
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          ForwardDetectors/ZDC/ZDC_SimEvent
                          Generators/GeneratorObjects
                          InnerDetector/InDetSimEvent
                          LArCalorimeter/LArSimEvent
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Simulation/G4Atlas/G4AtlasInterfaces
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Sim/MCTruth
                          Simulation/G4Sim/TrackRecord
                          Simulation/HitManagement
                          TestPolicy
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileIdentifier
                          TileCalorimeter/TileSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( G4AtlasTests
                     src/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel CaloDetDescrLib CaloIdentifier CaloSimEvent AthenaKernel StoreGateLib SGtests GeoAdaptors GeoPrimitives Identifier EventInfo ALFA_SimEv LUCID_SimEvent ZDC_SimEvent GeneratorObjects InDetSimEvent LArSimEvent MuonReadoutGeometry MuonIdHelpersLib MuonSimEvent McParticleEvent G4AtlasToolsLib MCTruth HitManagement TileDetDescr TileIdentifier TileSimEvent )

# Install files from the package:
atlas_install_headers( G4AtlasTests )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/sim_*.py )

