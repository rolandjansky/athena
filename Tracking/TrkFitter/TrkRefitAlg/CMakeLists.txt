# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkRefitAlg )

# Component(s) in the package:
atlas_add_component( TrkRefitAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkEventPrimitives TrkEventUtils TrkTrack TrkFitterUtils InDetReadoutGeometry StoreGateLib TrkSurfaces TrkMeasurementBase TrkParameters TrkTrackSummary TrkVertexOnTrack VxVertex TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces BeamSpotConditionsData )
