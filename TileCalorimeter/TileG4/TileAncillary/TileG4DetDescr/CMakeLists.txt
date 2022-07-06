# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileG4DetDescr )

# Component(s) in the package:
atlas_add_library( TileG4DetDescr
                   src/*.cxx
                   PUBLIC_HEADERS TileG4DetDescr
                   LINK_LIBRARIES CxxUtils )
