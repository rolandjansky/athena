# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkAlignInterfaces )

atlas_add_library( TrkAlignInterfaces
                   TrkAlignInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkAlignInterfaces
                   LINK_LIBRARIES GaudiKernel AthContainers TrkAlignEvent TrkTrack )

