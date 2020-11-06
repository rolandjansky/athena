# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackCaloClusterRecInterfaces )

# Declare an interface library for the package.
atlas_add_library( TrackCaloClusterRecInterfacesLib
   TrackCaloClusterRecInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TrackCaloClusterRecInterfaces
   LINK_LIBRARIES GaudiKernel xAODBase xAODCaloEvent TrkCaloExtension
   TrkEventPrimitives )
