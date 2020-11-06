# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetInclusiveSecVtx )

# Component(s) in the package:
atlas_add_component( InDetInclusiveSecVtx
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces TrkTrack TrkParticleBase TrkVertexFitterInterfaces TrkVxEdmCnvLib )
