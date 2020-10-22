# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkV0Fitter )

# Component(s) in the package:
atlas_add_library( TrkV0FitterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkV0Fitter
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel MagFieldConditions StoreGateLib TrkParameters TrkVertexFitterInterfaces
                   PRIVATE_LINK_LIBRARIES TrkDetDescrUtils TrkSurfaces TrkLinks TrkParticleBase TrkTrack VxVertex TrkExInterfaces )

atlas_add_component( TrkV0Fitter
                     src/components/*.cxx
                     LINK_LIBRARIES TrkV0FitterLib )
