################################################################################
# Package: TrigMuonHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigMuon
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigMuonBackExtrapolator
                          Trigger/TrigTools/TrigMuonRoITools
			  Trigger/TrigSteer/DecisionHandling
			  Control/AthViews
                          PRIVATE
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerUtils
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigMuonHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODTrigMuon MuonIdHelpersLib MuonRecHelperToolsLib TrigInDetEvent TrigMuonEvent TrigSteeringEvent TrigInterfacesLib DecisionHandling AthViews xAODMuon xAODTracking GaudiKernel MuonSegment MuonSegmentMakerUtils TrigConfHLTData TrigT1Interfaces TrigT1Result )

atlas_add_test( TrigMuonHypoConfig SCRIPT python -m  	TrigMuonHypo.testTrigMuonHypoConfig
		POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_headers( TrigMuonHypo )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

