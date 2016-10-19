################################################################################
# Package: TrigLongLivedParticlesHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigLongLivedParticlesHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/CxxUtils
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          Trigger/TrigEvent/TrigParticle 
			  External
			  InnerDetector/InDetDetDescr
			  InnerDetector/InDetRecEvent	
			  DetectorDescription
			  Tracking/TrkEvent
			  Event/xAOD)

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TrigLongLivedParticlesHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib CaloEvent CxxUtils xAODJet xAODTracking xAODTrigger GaudiKernel TrigParticle InDetIdentifier InDetPrepRawData IRegionSelector TrkSpacePoint xAODEventInfo)

# Install files from the package:
atlas_install_headers( TrigLongLivedParticlesHypo )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

