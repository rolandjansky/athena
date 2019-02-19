################################################################################
# Package: TrigBjetHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigBjetHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
			  Control/AthenaBaseComps
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Reconstruction/Particle
			  Reconstruction/Jet/JetCalibTools
                          Tracking/TrkEvent/VxSecVertex
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTrackJetFinderTool
			  Control/AthViews
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          PRIVATE
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          Event/EventInfo
                          Event/EventPrimitives
                          Event/FourMomUtils
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTrigger
                          PhysicsAnalysis/JetTagging/JetTagAlgs/BTagging
                          Reconstruction/Jet/JetEvent
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent 
 			  Trigger/TrigSteer/DecisionHandling )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigBjetHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODBase xAODTracking GaudiKernel Particle VxSecVertex TrigInDetEvent TrigInterfacesLib AthContainers GeoPrimitives EventInfo EventPrimitives FourMomUtils xAODBTagging xAODCore xAODJet xAODMuon BTaggingLib JetEvent VxVertex TrigCaloEvent TrigMuonEvent TrigNavigationLib TrigParticle TrigSteeringEvent DecisionHandlingLib AthViews )
atlas_add_test( TrigBjetHypoTool SCRIPT python -m  	TrigBjetHypo.TrigBjetHypoTool	
			POST_EXEC_SCRIPT nopost.sh )


# Install files from the package:
atlas_install_headers( TrigBjetHypo )
atlas_install_python_modules( python/*.py )

