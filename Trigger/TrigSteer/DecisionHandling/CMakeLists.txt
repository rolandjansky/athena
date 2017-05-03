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
                          #Control/AthenaBaseComps
                         # Control/CxxUtils
                          Trigger/TrigConfiguration/TrigConfHLTData
                          #Trigger/TrigEvent/TrigSteeringEvent
                          #Trigger/TrigSteer/L1Decoder
			  AtlasTest/TestTools
			  Control/StoreGate
			  )

# Component(s) in the package:
atlas_add_library( DecisionHandling
                   src/*.cxx
                   PUBLIC_HEADERS DecisionHandling
                   LINK_LIBRARIES xAODTrigger GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps CxxUtils TrigConfHLTData TrigSteeringEvent )



atlas_add_test( TrigCompositeUtils_test
                        SOURCES test/TrigCompositeUtils_test.cxx
                        LINK_LIBRARIES  TestTools xAODTrigger DecisionHandling
                        AthContainers )
