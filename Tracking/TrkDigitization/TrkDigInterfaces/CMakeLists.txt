# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDigInterfaces )

# Component(s) in the package:
atlas_add_library( TrkDigInterfaces
                     PUBLIC_HEADERS TrkDigInterfaces
                     LINK_LIBRARIES GaudiKernel TrkDigEvent )

