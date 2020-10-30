# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkCaloClusterROI )

# Component(s) in the package:
atlas_add_library( TrkCaloClusterROI
                   src/*.cxx
                   PUBLIC_HEADERS TrkCaloClusterROI
                   LINK_LIBRARIES AthContainers AthenaKernel GeoPrimitives EventPrimitives
                   PRIVATE_LINK_LIBRARIES TrkSurfaces TrkEventPrimitives )
