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
			  )


# Component(s) in the package:
atlas_add_library( TrigOutputHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigOutputHandling
                   LINK_LIBRARIES GaudiKernel AthViews AthenaBaseComps TrigSteeringEvent 
                   xAODTrigCalo xAODTrigEgamma xAODTrigger xAODTracking )                   

atlas_add_component( TrigOutputHandling
                     src/components/*.cxx                      
                     LINK_LIBRARIES TrigOutputHandlingLib )
