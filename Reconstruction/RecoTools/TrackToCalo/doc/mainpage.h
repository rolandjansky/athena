/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage TrackToCalo Package

This package contains tools and algorithms to extrapolate tracks and truth particles into the calorimeter.

@author Reconstruction Software Group

@section TrackToCaloClassList List of classes

The following classes are defined in this package:

@htmlinclude annotated.html

   - ExtrapolateToCaloTool : Extrapolates Trk::Track, Trk::TrackParameters to the calorimeter samplings using the Trk::IExtrapolator (extended and revised version of ExtrapolTrackToCaloTool)
   - ExtrapolTrackToCaloTool : Extrapolates Trk::Track to the calorimeter samplings
   - TrackToCaloAlg : Example algorithm which uses ExtrapolTrackToCaloTool to create a ImpactInCaloCollection
   - ImpactInCaloAlg : Example algorithm which uses ExtrapolateToCaloTool to create a ImpactInCaloCollection
   - CBNT_TrackToCalo : Writes contents of ImpactInCaloCollection into CBNT
   - CBNTAA_TrackToCalo : Writes contents of ImpactInCaloCollection into CBNTAA
   - ImpactInCalo : data class containing impact positions and directions at the various calorimter samplings
   - ImpactInCaloCollection : collection of ImpactInCalo

@section ExtrasTrackToCalo Extra Pages

 - @ref UsedTrackToCalo
 - @ref requirementsTrackToCalo
*/

/**
@page UsedTrackToCalo Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirementsTrackToCalo Requirements
@include requirements
*/

