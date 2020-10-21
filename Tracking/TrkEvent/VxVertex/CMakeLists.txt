# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VxVertex )

# Component(s) in the package:
atlas_add_library( VxVertex
                   src/*.cxx
                   PUBLIC_HEADERS VxVertex
                   LINK_LIBRARIES AthLinks AthContainers AthenaKernel GeoPrimitives
                   EventPrimitives TrkEventPrimitives  TrkNeutralParameters 
                   TrkTrackLink TrkParameters 
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( VxVertexDict
                      VxVertex/VxVertexDict.h
                      VxVertex/selection.xml
                      LINK_LIBRARIES AthLinks VxVertex )
