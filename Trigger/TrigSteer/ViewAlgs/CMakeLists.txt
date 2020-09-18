# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ViewAlgs )

# Component(s) in the package:
atlas_add_library( ViewAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS ViewAlgs
                   LINK_LIBRARIES AthContainers AthLinks GaudiKernel DecisionHandlingLib MuonCombinedEvent xAODJet xAODMuon TrigCompositeUtilsLib xAODTrigger )

atlas_add_component( ViewAlgs
                     src/components/*.cxx
                     LINK_LIBRARIES AthViews AthenaBaseComps CxxUtils DecisionHandlingLib MuonCombinedEvent StoreGateLib TrigSteeringEvent ViewAlgsLib xAODJet xAODMuon ViewAlgsLib )
