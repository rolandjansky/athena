# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO Hist )

# Component(s) in the package:
atlas_add_library( L1TopoInterfaces
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoInterfaces
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
                   LINK_LIBRARIES L1TopoCommon L1TopoEvent TrigConfBase )
