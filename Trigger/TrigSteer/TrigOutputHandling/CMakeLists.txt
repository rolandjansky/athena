################################################################################
# Package: TrigOutputHandling
################################################################################

# Declare the package name:
atlas_subdir( TrigOutputHandling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigger
			  #                          GaudiKernel
			  Control/AthContainers
			  Control/AthLinks 
                          PRIVATE
                          Control/AthenaBaseComps
			  Trigger/TrigEvent/TrigSteeringEvent
			  Event/xAOD/xAODTrigCalo
			  Event/xAOD/xAODTrigEgamma
			  Event/xAOD/xAODTrigger
			  Event/xAOD/xAODTracking
			  AtlasTest/TestTools
			  Control/StoreGate
			  )

find_package( ROOT )
find_package( Boost )
find_package( Eigen )


# Component(s) in the package:
atlas_add_component( TrigOutputHandling
                     src/components/*.cxx src/*.cxx
                     PUBLIC_HEADERS TrigOutputHandling
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIR}
                     LINK_LIBRARIES ${Boost_LIBRARIES}
		     ${ROOT_LIBRARIES} GaudiKernel AthViews  AthenaBaseComps TrigSteeringEvent 		     
        	     xAODTrigger xAODTracking xAODTrigCalo xAODTrigEgamma
                     )
