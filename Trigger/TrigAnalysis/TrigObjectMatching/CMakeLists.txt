################################################################################
# Package: TrigObjectMatching
################################################################################

# Declare the package name:
atlas_subdir( TrigObjectMatching )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/DataModel
                          GaudiKernel
                          Reconstruction/Jet/JetEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Event/EventKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Reconstruction/MuonIdentification/muonEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigObjectMatchingLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigObjectMatching
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils DataModel GaudiKernel JetEvent egammaEvent tauEvent TrigCaloEvent TrigMuonEvent TrigParticle TrigSteeringEvent TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventKernel AnalysisTriggerEvent muonEvent )

atlas_add_component( TrigObjectMatching
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils DataModel GaudiKernel JetEvent egammaEvent tauEvent TrigDecisionToolLib TrigCaloEvent TrigMuonEvent TrigParticle TrigSteeringEvent EventKernel AnalysisTriggerEvent muonEvent TrigObjectMatchingLib )

atlas_add_dictionary( TrigObjectMatchingDict
                      TrigObjectMatching/TrigObjectMatchingDict.h
                      TrigObjectMatching/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils DataModel GaudiKernel JetEvent egammaEvent tauEvent TrigDecisionToolLib TrigCaloEvent TrigMuonEvent TrigParticle TrigSteeringEvent EventKernel AnalysisTriggerEvent muonEvent TrigObjectMatchingLib )

