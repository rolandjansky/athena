# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiSpacePointsSeed )

# Component(s) in the package:
atlas_add_library( SiSpacePointsSeed
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePointsSeed
                   LINK_LIBRARIES TrkSpacePoint
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

