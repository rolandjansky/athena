################################################################################
# Package: DecisionHandling
################################################################################

# Declare the package name:
atlas_subdir( DecisionHandling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          Control/AthContainers
                          Control/AthLinks 
                          PRIVATE
                          Control/AthViews
                          Control/StoreGate
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Trigger/TrigConfiguration/TrigConfHLTData
                          AtlasTest/TestTools
                          Control/StoreGate
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs
                          Trigger/TrigMonitoring/TrigCostMonitorMT )

atlas_add_library( DecisionHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS DecisionHandling
                   LINK_LIBRARIES xAODTrigger GaudiKernel TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps CxxUtils TrigConfHLTData TrigTimeAlgsLib )

# Component(s) in the package:
atlas_add_component( DecisionHandling
                     src/components/*.cxx
                     LINK_LIBRARIES DecisionHandlingLib )

# Unit tests:
atlas_add_test( TrigCompositeUtils_test
                SOURCES test/TrigCompositeUtils_test.cxx
                LINK_LIBRARIES TestTools xAODTrigger DecisionHandlingLib AthContainers SGtests )

atlas_add_test( Combinators_test
                SOURCES test/Combinators_test.cxx
                LINK_LIBRARIES TestTools xAODTrigger DecisionHandlingLib AthContainers )
