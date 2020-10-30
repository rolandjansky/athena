# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVKalVrtCore )

atlas_add_library( TrkVKalVrtCore
                   src/*.cxx
                   PUBLIC_HEADERS TrkVKalVrtCore )

