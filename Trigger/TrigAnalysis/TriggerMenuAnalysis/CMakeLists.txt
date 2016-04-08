################################################################################
# Package: TriggerMenuAnalysis
################################################################################

# Declare the package name:
atlas_subdir( TriggerMenuAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Control/StoreGate
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          TileCalorimeter/TileEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigNtupleBase
                          Trigger/TrigAnalysis/TriggerMenuNtuple
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Event/EventInfo
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigConfiguration/TrigConfigSvc
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TriggerMenuAnalysisLib
                   src/*.cxx
                   PUBLIC_HEADERS TriggerMenuAnalysis
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel DataModel SGTools GaudiKernel AnalysisTriggerEvent TileEvent TrigNtupleBase TriggerMenuNtuple TrigConfHLTData TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent StoreGateLib SGtests TrigDecisionToolLib TrigNavigationLib TrigConfigSvcLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} EventInfo MuonTrigCoinData JetEvent muonEvent Particle egammaEvent tauEvent VxVertex TrigConfL1Data TrigMissingEtEvent TrigT1Interfaces )

atlas_add_component( TriggerMenuAnalysis
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel DataModel SGTools StoreGateLib SGtests GaudiKernel AnalysisTriggerEvent TileEvent TrigDecisionToolLib TrigNtupleBase TriggerMenuNtuple TrigConfHLTData TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigNavigationLib TrigParticle TrigSteeringEvent EventInfo MuonTrigCoinData JetEvent muonEvent Particle egammaEvent tauEvent VxVertex TrigConfL1Data TrigConfigSvcLib TrigMissingEtEvent TrigT1Interfaces TriggerMenuAnalysisLib )

atlas_add_dictionary( TriggerMenuAnalysisDict
                      TriggerMenuAnalysis/TriggerMenuAnalysisDict.h
                      TriggerMenuAnalysis/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel DataModel SGTools StoreGateLib SGtests GaudiKernel AnalysisTriggerEvent TileEvent TrigDecisionToolLib TrigNtupleBase TriggerMenuNtuple TrigConfHLTData TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigNavigationLib TrigParticle TrigSteeringEvent EventInfo MuonTrigCoinData JetEvent muonEvent Particle egammaEvent tauEvent VxVertex TrigConfL1Data TrigConfigSvcLib TrigMissingEtEvent TrigT1Interfaces TriggerMenuAnalysisLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

