/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkRIO_OnTrack_page The RIO_OnTrack class

@section TrkRIO_OnTrack_introductionTrkRIO_OnTrack Introduction

Other conceptual names for this class is HitOnTrack or ClusterOnTrack. It contains the modified location of the RIOs according to a track. There are no container of RIO_OnTrack objects since the Track is essentially the container. The RIO_OnTrack contains the RIO from which it comes from. For most use cases there will be a single RIO associated with a RIO_OnTrack, while several RIO_OnTrack for each RIO (since there can be different tracks that make use of the same RIOs). However some users (like xKalman) might join clusters together so for this case a vector of RIOs is provided. 

For the navigation, a multimap is recorded between RIO and RIO_OnTrack. In the TrkTrack package there is a map between RIO_OnTrack and Track. In summary the navigation is as followed:
- from Track to RIO_OnTrack: Track class
- from RIO_OnTrack to RIO: RIO_OnTrack class
- from RIO to RIO_OnTrack: multimap
- from RIO_OnTrack to Track: map
- from RIO to Track: use the multimap then the map
- from Track to RIO: Track class to get to RIO_OnTrack, then RIO_OnTrack class to get to RIO

All of these items are under active discussion.

Please let me know of any errors, or if anything is unclear.
@author Veronique.Boisvert@cern.ch
@author Edward.Moyse@cern.ch
@author Andreas.Salzburger@cern.ch

@namespace Trk
A namespace to enclose the Tracking classes.
*/
