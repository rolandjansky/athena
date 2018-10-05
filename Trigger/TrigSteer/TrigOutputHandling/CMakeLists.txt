################################################################################
# Package: TrigOutputHandling
################################################################################

# Declare the package name:
atlas_subdir( TrigOutputHandling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
			  Control/AthContainers
			  Control/AthLinks 
                          Control/AthenaBaseComps
                          Control/AthViews
			  Trigger/TrigEvent/TrigSteeringEvent
			  Event/xAOD/xAODTrigCalo
			  Event/xAOD/xAODTrigEgamma
			  Event/xAOD/xAODTrigger
			  Event/xAOD/xAODTracking
			  Trigger/TrigDataAccess/TrigSerializeResult
			  Event/xAOD/xAODTrigMuon
			  Event/xAOD/xAODMuon
			  Trigger/TrigSteer/DecisionHandling
			  Control/AthenaMonitoring
			  )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigOutputHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigOutputHandling
		   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}  GaudiKernel AthViews AthenaBaseComps TrigSteeringEvent TrigSerializeResultLib
                   xAODTrigCalo xAODTrigEgamma xAODTrigger xAODTracking xAODTrigMuon xAODMuon DecisionHandlingLib AthenaMonitoringLib )                 

atlas_add_component( TrigOutputHandling
                     src/components/*.cxx                      
                     LINK_LIBRARIES TrigOutputHandlingLib )
