# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

find_package( Boost )

# Declare the package name:
atlas_subdir( TrkVKalVrtCore )

atlas_add_library( TrkVKalVrtCore
                   src/*.cxx
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PUBLIC_HEADERS TrkVKalVrtCore )

