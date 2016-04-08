################################################################################
# Package: JetD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( JetD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigCaloEvent
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/Navigation
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventKernel
                          Event/FourMomUtils
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/D3PDMaker/TriggerD3PDMaker
                          PhysicsAnalysis/JetTagging/JetTagEvent
                          PhysicsAnalysis/JetTagging/JetTagInfo
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Particle
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( JetD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} TrigCaloEvent CaloEvent CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel Navigation SGTools StoreGateLib SGtests Identifier EventKernel FourMomUtils GaudiKernel AnalysisTriggerEvent D3PDMakerUtils TriggerD3PDMakerLib JetTagEvent JetTagInfo JetEvent Particle TileConditionsLib TileEvent TileIdentifier VxVertex )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

