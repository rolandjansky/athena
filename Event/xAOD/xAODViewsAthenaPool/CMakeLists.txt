# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODViewsAthenaPool )

# Component(s) in the package.
atlas_add_poolcnv_library( xAODViewsAthenaPoolPoolCnv
   FILES xAODViews/TrackParticleViewVector.h xAODViews/VertexViewVector.h
   TYPES_WITH_NAMESPACE xAOD::TrackParticleViewVector xAOD::VertexViewVector
   CNV_PFX xAOD 
   LINK_LIBRARIES AthenaPoolCnvSvcLib xAODViewsLib )
