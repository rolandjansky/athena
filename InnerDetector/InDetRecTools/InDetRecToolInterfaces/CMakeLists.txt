# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetRecToolInterfaces )

# Component(s) in the package:
atlas_add_library( InDetRecToolInterfaces
                   InDetRecToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetRecToolInterfaces
                   LINK_LIBRARIES AsgTools AthLinks GaudiKernel GeoPrimitives IRegionSelector Identifier InDetPrepRawData MagFieldElements SiSPSeededTrackFinderData SiSpacePointsSeed TrkEventPrimitives TrkMeasurementBase TrkParameters TrkParticleBase TrkSegment TrkSpacePoint TrkTrack VxVertex xAODCaloEvent xAODTracking )
