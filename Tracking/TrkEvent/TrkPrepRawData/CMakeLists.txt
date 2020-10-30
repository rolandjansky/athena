# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkPrepRawData )

# Component(s) in the package:
atlas_add_library( TrkPrepRawData
                   src/*.cxx
                   PUBLIC_HEADERS TrkPrepRawData
                   LINK_LIBRARIES AthLinks AthContainers AthenaKernel GeoPrimitives Identifier EventPrimitives EventContainers
                   PRIVATE_LINK_LIBRARIES GaudiKernel )
