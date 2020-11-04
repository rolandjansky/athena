# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetPriVxFinderTool )

# Component(s) in the package:
atlas_add_component( InDetPriVxFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthenaBaseComps BeamSpotConditionsData
                     EventPrimitives GaudiKernel GeoPrimitives InDetRecToolInterfaces InDetTrackSelectionToolLib
                     StoreGateLib TrkEventPrimitives TrkLinks TrkParameters TrkParticleBase TrkSurfaces TrkTrack
                     TrkTrackLink TrkTrackSummary TrkVertexFitterInterfaces TrkVertexFittersLib
                     VxMultiVertex VxVertex xAODTracking )
