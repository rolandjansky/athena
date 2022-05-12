# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTestBLayer )

# Component(s) in the package:
atlas_add_library( InDetTestBLayerLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetTestBLayer
                   LINK_LIBRARIES AthenaBaseComps GeoPrimitives Identifier EventPrimitives GaudiKernel InDetRecToolInterfaces
                   PixelReadoutGeometryLib TrkEventPrimitives TrkParameters TrkToolInterfaces TrkExInterfaces InDetConditionsSummaryService
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr IdDictDetDescr InDetIdentifier InDetReadoutGeometry
                   Particle TrkGeometry TrkSurfaces TrkMeasurementBase TrkTrack )

atlas_add_component( InDetTestBLayer
                     src/components/*.cxx
                     LINK_LIBRARIES InDetTestBLayerLib )
