################################################################################
# Package: DecisionHandling
################################################################################

# Declare the package name:
atlas_subdir( DecisionHandling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigger
			  #                          GaudiKernel
			  Control/AthContainers
			  Control/AthLinks 
                          PRIVATE
                          #Control/AthViews
                         # Control/StoreGate
                          Control/AthenaBaseComps
                         # Control/CxxUtils
                          Trigger/TrigConfiguration/TrigConfHLTData
                          #Trigger/TrigEvent/TrigSteeringEvent
                          #Trigger/TrigSteer/L1Decoder
			  AtlasTest/TestTools
			  Control/StoreGate
			  )

find_package( ROOT )
find_package( Boost )

atlas_add_library( DecisionHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS DecisionHandling
                   LINK_LIBRARIES xAODTrigger GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps CxxUtils TrigConfHLTData )

# Component(s) in the package:
atlas_add_component( DecisionHandling
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES}
		     ${ROOT_LIBRARIES} GaudiKernel AthViews  AthenaBaseComps CxxUtils xAODTrigger DecisionHandlingLib
                     )


atlas_add_test( TrigCompositeUtils_test
                        SOURCES test/TrigCompositeUtils_test.cxx
                        LINK_LIBRARIES  TestTools xAODTrigger DecisionHandlingLib
                        AthContainers )

atlas_add_test( Combinators_test
                        SOURCES test/Combinators_test.cxx
                        LINK_LIBRARIES  TestTools xAODTrigger DecisionHandlingLib
                        AthContainers )
