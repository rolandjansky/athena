# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatUtility )

# Component(s) in the package:
atlas_add_library( iPatUtility
                   src/VertexRegion.cxx
                   PUBLIC_HEADERS iPatUtility
                   LINK_LIBRARIES GeoPrimitives )
