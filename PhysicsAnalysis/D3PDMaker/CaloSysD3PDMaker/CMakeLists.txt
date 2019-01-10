################################################################################
# Package: CaloSysD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( CaloSysD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloConditions
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloDmDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloSimEvent
                          Calorimeter/CaloUtils
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/Navigation
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArSimEvent
                          LArCalorimeter/LArCabling
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/PrimaryDPDMaker
                          Reconstruction/Jet/JetEvent
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          TileCalorimeter/TileSimEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CaloSysD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloDetDescrLib CaloDmDetDescr CaloEvent CaloGeoHelpers CaloIdentifier CaloSimEvent CaloUtilsLib AthContainers AthenaBaseComps AthenaKernel Navigation StoreGateLib SGtests Identifier xAODCaloEvent GaudiKernel LArIdentifier LArRawEvent LArRecEvent LArSimEvent LArCablingLib D3PDMakerUtils PrimaryDPDMakerLib JetEvent TileConditionsLib TileEvent TileIdentifier TileSimEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

