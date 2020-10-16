# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VxJetVertex )

# Component(s) in the package:
atlas_add_library( VxJetVertex
   VxJetVertex/*.h src/*.cxx
   PUBLIC_HEADERS VxJetVertex
   LINK_LIBRARIES EventPrimitives GaudiKernel TrkEventPrimitives TrkParticleBase
   TrkTrack TrkTrackLink VxVertex xAODTracking
   PRIVATE_LINK_LIBRARIES AthLinks )

atlas_add_dictionary( VxJetVertexDict
   VxJetVertex/VxJetVertexDict.h
   VxJetVertex/selection.xml
   LINK_LIBRARIES TrkParticleBase VxJetVertex )
