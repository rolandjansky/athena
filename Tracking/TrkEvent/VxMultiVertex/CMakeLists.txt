# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VxMultiVertex )

# Component(s) in the package:
atlas_add_library( VxMultiVertex
                   src/*.cxx
                   PUBLIC_HEADERS VxMultiVertex
                   LINK_LIBRARIES AthLinks AthContainers AthenaKernel GeoPrimitives TrkParameters TrkParticleBase TrkTrack VxVertex xAODTracking )

atlas_add_dictionary( VxMultiVertexDict
                      VxMultiVertex/VxMultiVertexDict.h
                      VxMultiVertex/selection.xml
                      LINK_LIBRARIES AthLinks TrkParticleBase VxMultiVertex VxVertex
                      ELEMENT_LINKS MVFVxContainer )

