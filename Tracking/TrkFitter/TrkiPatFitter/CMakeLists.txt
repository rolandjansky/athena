# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkiPatFitter )

# Component(s) in the package:
atlas_add_component( TrkiPatFitter
                     src/iPatFitter.cxx
                     src/iPatGlobalFitter.cxx
                     src/MaterialAllocator.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GeoPrimitives GaudiKernel TrkParameters TrkTrack TrkFitterInterfaces TrkiPatFitterUtils EventPrimitives TrkDetDescrInterfaces TrkDetElementBase TrkGeometry TrkSurfaces Identifier TrkMaterialOnTrack TrkMeasurementBase TrkRIO_OnTrack MuonCompetingRIOsOnTrack TrkExInterfaces TrkExUtils TrkToolInterfaces TrkTrackSummary )
