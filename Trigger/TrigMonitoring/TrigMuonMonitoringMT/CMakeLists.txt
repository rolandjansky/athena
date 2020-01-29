
atlas_subdir( TrigMuonMonitoringMT )

atlas_depends_on_subdirs(  PUBLIC
			   Control/AthenaBaseComps
			   Control/AthenaMonitoring
			   GaudiKernel
			   Trigger/TrigAnalysis/TrigDecisionTool
			   PRIVATE
			   Event/xAOD/xAODMuon
			   Event/xAOD/xAODTrigMuon
				 Event/FourMomUtils
				 Trigger/TrigSteer/DecisionHandling
			   )


atlas_add_component( TrigMuonMonitoringMT
		     src/*.cxx
		     src/components/*.cxx
		     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringLib GaudiKernel xAODMuon xAODTrigMuon TrigDecisionToolLib FourMomUtils DecisionHandlingLib
		     )


atlas_install_python_modules( python/*.py )
