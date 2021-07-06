# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetMultipleVertexSeedFinder )

# Component(s) in the package:
atlas_add_component( InDetMultipleVertexSeedFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces EventPrimitives xAODTracking InDetMultipleVertexSeedFinderUtilsLib TrkParticleBase TrkTrack VxVertex TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces BeamSpotConditionsData )
