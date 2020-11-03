# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetMultipleVertexSeedFinderUtils )

# Component(s) in the package:
atlas_add_library( InDetMultipleVertexSeedFinderUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetMultipleVertexSeedFinderUtils
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel
                   PRIVATE_LINK_LIBRARIES EventPrimitives TrkParameters TrkParticleBase TrkTrack VxVertex TrkExInterfaces )

atlas_add_component( InDetMultipleVertexSeedFinderUtils
                     src/components/*.cxx
                     LINK_LIBRARIES InDetMultipleVertexSeedFinderUtilsLib )
