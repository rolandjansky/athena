# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrInterfaces )

# Component(s) in the package:
atlas_add_library( TrkDetDescrInterfaces
                   PUBLIC_HEADERS TrkDetDescrInterfaces
                   LINK_LIBRARIES AthenaKernel CxxUtils GeoPrimitives GaudiKernel TrkDetDescrUtils TrkGeometry TrkSurfaces TrkVolumes TrkEventPrimitives TrkMaterialOnTrack TrkParameters )
