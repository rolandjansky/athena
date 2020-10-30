# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkLinks )

# Component(s) in the package:
atlas_add_library( TrkLinks
                   src/*.cxx
                   PUBLIC_HEADERS TrkLinks
                   LINK_LIBRARIES AthLinks xAODTracking TrkNeutralParameters TrkParameters TrkTrackLink )
