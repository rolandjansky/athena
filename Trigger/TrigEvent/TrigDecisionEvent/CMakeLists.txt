# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigDecisionEvent )

# Component(s) in the package:
atlas_add_library( TrigDecisionEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigDecisionEvent
                   LINK_LIBRARIES AthLinks AthenaKernel TrigSteeringEvent )

atlas_add_dictionary( TrigDecisionEventDict
                      TrigDecisionEvent/TrigDecisionEventDict.h
                      TrigDecisionEvent/selection.xml
                      LINK_LIBRARIES TrigDecisionEvent
                      DATA_LINKS HLT::HLTResult )

