# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkJetVxFitter )

# External dependencies:
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_library( TrkJetVxFitterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkJetVxFitter
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel MagFieldConditions StoreGateLib TrkParameters TrkTrackLink
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} MagFieldElements TrkNeutralParameters TrkTrack VxJetVertex VxVertex TrkVertexFitterInterfaces )

atlas_add_component( TrkJetVxFitter
                     src/components/*.cxx
                     LINK_LIBRARIES TrkJetVxFitterLib )
