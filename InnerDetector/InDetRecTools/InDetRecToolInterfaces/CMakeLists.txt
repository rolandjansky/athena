# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetRecToolInterfaces )

set( extra_libs )
if( NOT XAOD_ANALYSIS )
  set( extra_libs GeoPrimitives IRegionSelector Identifier InDetPrepRawData MagFieldElements SiSPSeededTrackFinderData SiSpacePointsSeed TrkEventPrimitives TrkMeasurementBase TrkParameters TrkParticleBase TrkSegment TrkSpacePoint TrkTrack VxVertex )
endif()


# Component(s) in the package:
atlas_add_library( InDetRecToolInterfaces
                   InDetRecToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetRecToolInterfaces
                   LINK_LIBRARIES AsgTools AthLinks GaudiKernel xAODCaloEvent xAODTracking ${extra_libs} )
