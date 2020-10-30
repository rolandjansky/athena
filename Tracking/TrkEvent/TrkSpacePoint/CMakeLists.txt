# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkSpacePoint )

# Component(s) in the package:
atlas_add_library( TrkSpacePoint
                   src/*.cxx
                   PUBLIC_HEADERS TrkSpacePoint
                   LINK_LIBRARIES AthContainers AthenaKernel Identifier GaudiKernel TrkMeasurementBase EventContainers
                   PRIVATE_LINK_LIBRARIES TrkDetElementBase TrkSurfaces TrkEventPrimitives TrkPrepRawData )

