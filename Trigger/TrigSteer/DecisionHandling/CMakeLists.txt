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
                          Trigger/TrigSteer/TrigCompositeUtils
                          PRIVATE
                          Control/AthViews
                          Control/StoreGate
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODBase
                          AtlasTest/TestTools
                          Control/StoreGate
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigTools/TrigTimeAlgs
                          Trigger/TrigMonitoring/TrigCostMonitorMT
                          Trigger/TrigSteer/TrigCompositeUtils
                          Control/AthenaMonitoringKernel )

atlas_add_library( DecisionHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS DecisionHandling
                   LINK_LIBRARIES xAODTrigger GaudiKernel TrigSteeringEvent TrigCompositeUtilsLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps CxxUtils TrigTimeAlgsLib AthenaMonitoringKernelLib TrigCostMonitorMTLib AthViews )

# Component(s) in the package:
atlas_add_component( DecisionHandling
                     src/components/*.cxx
                     LINK_LIBRARIES DecisionHandlingLib TrigCompositeUtilsLib TrigTimeAlgsLib AthenaMonitoringKernelLib TrigCostMonitorMTLib )

atlas_install_python_modules( python/*.py )

atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python/*.py
   POST_EXEC_SCRIPT nopost.sh )
