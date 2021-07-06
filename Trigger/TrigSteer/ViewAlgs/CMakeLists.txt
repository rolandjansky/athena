# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ViewAlgs )

# Component(s) in the package:
atlas_add_library( ViewAlgsLib
                   src/lib/*.cxx
                   PUBLIC_HEADERS ViewAlgs
                   LINK_LIBRARIES AthenaBaseComps EventContainers
                   PRIVATE_LINK_LIBRARIES AthViews )

atlas_add_component( ViewAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthLinks AthViews AthenaBaseComps DecisionHandlingLib GaudiKernel MuonCombinedEvent TrigCompositeUtilsLib TrigSteeringEvent xAODJet xAODMuon )
