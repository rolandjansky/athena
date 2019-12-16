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
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODBase
                          Trigger/TrigConfiguration/TrigConfHLTData
                          AtlasTest/TestTools
                          Control/StoreGate
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs
                          Trigger/TrigMonitoring/TrigCostMonitorMT
                          Control/AthenaMonitoring )

atlas_add_library( DecisionHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS DecisionHandling
                   LINK_LIBRARIES xAODTrigger GaudiKernel TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps CxxUtils TrigConfHLTData TrigTimeAlgsLib AthenaMonitoringLib )

# Component(s) in the package:
atlas_add_component( DecisionHandling
                     src/components/*.cxx
                     LINK_LIBRARIES DecisionHandlingLib )

atlas_install_python_modules( python/*.py )

# Unit tests:
atlas_add_test( TrigCompositeUtils_test
                SOURCES test/TrigCompositeUtils_test.cxx
                LINK_LIBRARIES TestTools xAODTrigger DecisionHandlingLib AthContainers )

atlas_add_test( TrigTraversal_test
                SOURCES test/TrigTraversal_test.cxx
                LINK_LIBRARIES  TestTools xAODTrigger xAODEgamma xAODMuon xAODBase DecisionHandlingLib
                AthContainers )

atlas_add_test( Combinators_test
                SOURCES test/Combinators_test.cxx
                LINK_LIBRARIES TestTools xAODTrigger DecisionHandlingLib AthContainers )

atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python/*.py
   POST_EXEC_SCRIPT nopost.sh )
