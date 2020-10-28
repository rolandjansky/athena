# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiSpacePoint )

# Component(s) in the package:
atlas_add_library( SiSpacePoint
                   src/*.cxx
                   PUBLIC_HEADERS SiSpacePoint
                   LINK_LIBRARIES Identifier TrkDetElementBase TrkSpacePoint
                   PRIVATE_LINK_LIBRARIES GeoPrimitives EventPrimitives TrkSurfaces TrkEventPrimitives TrkPrepRawData )
