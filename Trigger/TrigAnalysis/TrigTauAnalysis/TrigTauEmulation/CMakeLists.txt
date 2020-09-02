# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTauEmulation )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_library( TrigTauEmulationLib
   TrigTauEmulation/*.h Root/*.cxx
   PUBLIC_HEADERS TrigTauEmulation
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODTau xAODTracking
   xAODTrigger PATCoreLib PATInterfaces TrigDecisionToolLib
   PRIVATE_LINK_LIBRARIES AthContainers xAODBase xAODCore )

atlas_add_component( TrigTauEmulation
  src/*.h src/*.cxx src/components/*.cxx
  LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigTauEmulationLib xAODJet )

atlas_add_dictionary( TrigTauEmulationDict
  TrigTauEmulation/TrigTauEmulationDict.h
  TrigTauEmulation/selection.xml
  LINK_LIBRARIES TrigTauEmulationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
