# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoEvent )

# Component(s) in the package:
atlas_add_library( L1TopoEvent
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoEvent
                   LINK_LIBRARIES TrigConfBase L1TopoCommon )
