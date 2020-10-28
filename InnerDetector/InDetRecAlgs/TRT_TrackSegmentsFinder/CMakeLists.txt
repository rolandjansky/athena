# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_TrackSegmentsFinder )

# Component(s) in the package:
atlas_add_component( TRT_TrackSegmentsFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces StoreGateLib TRT_ReadoutGeometry TrkCaloClusterROI TrkSegment MagFieldElements MagFieldConditions )
