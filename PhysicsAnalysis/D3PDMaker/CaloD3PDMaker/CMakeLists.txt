################################################################################
# Package: CaloD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( CaloD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloConditions
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          DetectorDescription/Identifier
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          LArCalorimeter/LArRecEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/PrimaryDPDMaker
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CaloD3PDMaker
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib AthContainers AthenaBaseComps AthenaKernel SGTools Identifier xAODCaloEvent GaudiKernel LArRecEvent D3PDMakerUtils PrimaryDPDMakerLib TileEvent TileIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py )
