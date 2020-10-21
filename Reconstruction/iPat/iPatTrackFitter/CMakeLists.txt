# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatTrackFitter )

# Component(s) in the package:
atlas_add_component( iPatTrackFitter
                     src/TrackFitter.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel iPatInterfaces GeoPrimitives EventPrimitives TrkSurfaces TrkVolumes TrkDetDescrInterfaces TrkEventPrimitives TrkMeasurementBase VxVertex TrkExInterfaces TrkExUtils TrkiPatFitterUtils TrkTrack )
