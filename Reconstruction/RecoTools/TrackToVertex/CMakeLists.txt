# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackToVertex )

# Component(s) in the package:
atlas_add_component( TrackToVertex
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps BeamSpotConditionsData GaudiKernel GeoPrimitives ITrackToVertex Particle TrkDetDescrUtils TrkExInterfaces TrkParameters TrkSurfaces TrkTrack VxVertex )
