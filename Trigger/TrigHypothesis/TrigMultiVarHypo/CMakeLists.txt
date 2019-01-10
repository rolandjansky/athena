# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: TrigMultiVarHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigMultiVarHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigRinger
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          LumiBlock/LumiBlockComps
                          Tools/PathResolver
                          GaudiKernel
                          Control/AthViews )

# Component(s) in the package:
atlas_add_library( TrigMultiVarHypoLib
                   src/*.cxx
                   src/tools/*.cxx
                   src/preproc/*.cxx
                   PUBLIC_HEADERS TrigMultiVarHypo
                   LINK_LIBRARIES xAODTrigCalo xAODTrigRinger TrigCaloEvent
		   TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib
		   LumiBlockCompsLib PathResolver DecisionHandlingLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel AthViews )

atlas_add_component( TrigMultiVarHypo
                     src/components/*.cxx
                     LINK_LIBRARIES xAODTrigCalo xAODTrigRinger TrigCaloEvent
		     TrigSteeringEvent TrigInterfacesLib TrigTimeAlgsLib
		     GaudiKernel TrigMultiVarHypoLib PathResolver
		     LumiBlockCompsLib DecisionHandlingLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )

