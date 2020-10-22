# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexBilloirTools )

# Component(s) in the package:
atlas_add_component( TrkVertexBilloirTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkVertexFitterInterfaces EventPrimitives TrkDetDescrUtils 
		             TrkSurfaces TrkLinks TrkParticleBase TrkTrack VxVertex TrkExInterfaces CxxUtils)
