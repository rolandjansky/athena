# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( TrkSegment )

# Component(s) in the package.
atlas_add_library( TrkSegment
   TrkSegment/*.h src/*.cxx
   PUBLIC_HEADERS TrkSegment
   LINK_LIBRARIES AthenaKernel AthContainers AthLinks Identifier TrkMeasurementBase
   PRIVATE_LINK_LIBRARIES GaudiKernel TrkSurfaces TrkEventPrimitives )

atlas_add_dictionary( TrkSegmentDict
   TrkSegment/TrkSegmentDict.h
   TrkSegment/selection.xml
   LINK_LIBRARIES AthContainers AthLinks TrkSegment )
