# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigDecisionInterface )

# Define an interface library for the header(s):
atlas_add_library( TrigDecisionInterface
   TrigDecisionInterface/*.h
   INTERFACE
   PUBLIC_HEADERS TrigDecisionInterface
   LINK_LIBRARIES AsgTools )
