# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVxEdmCnv )

# Component(s) in the package:
atlas_add_library( TrkVxEdmCnvLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkVxEdmCnv
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel TrkVertexFitterInterfaces
                   PRIVATE_LINK_LIBRARIES TrkLinks TrkTrackLink VxVertex )

atlas_add_component( TrkVxEdmCnv
                     src/components/*.cxx
                     LINK_LIBRARIES TrkVxEdmCnvLib )
