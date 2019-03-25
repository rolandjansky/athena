# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: TrigMultiVarHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigMultiVarHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigRinger
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Trigger/TrigSteer/DecisionHandling
                          LumiBlock/LumiBlockComps
                          Tools/PathResolver
                          PRIVATE
                          GaudiKernel 
                          Control/AthViews
                          Control/AthContainers
                          Control/AthenaMonitoring
                          )

# Component(s) in the package:
atlas_add_library( TrigMultiVarHypoLib
                   src/*.cxx
                   src/tools/*.cxx
                   src/tools/procedures/*.cxx
                   src/tools/common/*.cxx
                   PUBLIC_HEADERS TrigMultiVarHypo
                   LINK_LIBRARIES AsgTools xAODTrigCalo xAODTrigRinger TrigCaloEvent 
                   TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib 
                   DecisionHandlingLib LumiBlockCompsLib PathResolver 
                   AthenaMonitoringLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel AthViews )


atlas_add_component(  TrigMultiVarHypo
                      src/components/*.cxx
                      LINK_LIBRARIES AsgTools xAODTrigCalo xAODTrigRinger TrigCaloEvent 
                      TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib GaudiKernel 
                      TrigMultiVarHypoLib DecisionHandlingLib LumiBlockCompsLib 
                      PathResolver AthenaMonitoringLib) 

# Install files from the package:
atlas_install_python_modules( python/*.py )

