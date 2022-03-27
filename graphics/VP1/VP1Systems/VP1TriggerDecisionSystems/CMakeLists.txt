# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1TriggerDecisionSystems )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1TriggerDecisionSystems 
                   VP1TriggerDecisionSystems/*.h src/*.c*
                   PUBLIC_HEADERS VP1TriggerDecisionSystems
                   LINK_LIBRARIES VP1Base TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES TrigSteeringEvent )

