# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1TriggerDecisionSystems )

# Component(s) in the package:
atlas_add_library( VP1TriggerDecisionSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1TriggerDecisionSystems
                   LINK_LIBRARIES VP1Base TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES TrigSteeringEvent )

