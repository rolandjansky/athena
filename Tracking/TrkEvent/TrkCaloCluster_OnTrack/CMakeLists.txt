# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkCaloCluster_OnTrack )

# Component(s) in the package:
atlas_add_library( TrkCaloCluster_OnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkCaloCluster_OnTrack
                   LINK_LIBRARIES GeoPrimitives EventPrimitives TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkSurfaces TrkMaterialOnTrack )

