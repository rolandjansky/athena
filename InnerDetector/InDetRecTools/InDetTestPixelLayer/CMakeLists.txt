# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTestPixelLayer )

# Component(s) in the package:
atlas_add_library( InDetTestPixelLayerLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetTestPixelLayer
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GeoPrimitives Identifier GaudiKernel InDetIdentifier InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkToolInterfaces InDetConditionsSummaryService
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr IdDictDetDescr InDetReadoutGeometry PixelReadoutGeometryLib TrkGeometry TrkSurfaces TrkMeasurementBase TrkParticleBase TrkTrack TrkTrackSummary TrkExInterfaces )

atlas_add_component( InDetTestPixelLayer
                     src/components/*.cxx
                     LINK_LIBRARIES InDetTestPixelLayerLib )
