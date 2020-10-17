# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatInterfaces )

# Component(s) in the package:
atlas_add_library( iPatInterfaces
                   PUBLIC_HEADERS iPatInterfaces
                   LINK_LIBRARIES AtlasHepMCLib AthenaKernel GeoPrimitives GaudiKernel InDetPrepRawData iPatTrack TrkParameters )
