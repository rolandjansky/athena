# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkV0Vertex )

# Component(s) in the package:
atlas_add_library( TrkV0Vertex
                   src/*.cxx
                   PUBLIC_HEADERS TrkV0Vertex
                   LINK_LIBRARIES AthLinks AthContainers AthenaKernel EventKernel VxVertex )

atlas_add_dictionary( TrkV0VertexDict
                      TrkV0Vertex/V0VertexDict.h
                      TrkV0Vertex/selection.xml
                      LINK_LIBRARIES AthLinks TrkV0Vertex VxVertex
                      ELEMENT_LINKS V0Container )

