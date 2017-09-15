/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiTrigSPSeededTrackFinder_page SiTrigSPSeededTrackFinder
@author Markus.Elsing@cern.ch, Igor.Gavrilenko@cern.ch
@section SiTrigSPSeededTrackFinder_SiTrigSPSeededTrackFinderIntro Introduction

This algorithms is a trigger seeded version of SiSPSeededTrackFinder.
It is devoted to track finding using Pixels and SCT
clusters starting from space point seeds information. Output of this
algorithm is Trk::Track collection.

This algorithm uses three standard offline tools:
<ul>
<li>ISiSpacePointsSeedMaker - space point seeds maker tool.  </li>
<li>ISiZvertexMake          - primary verices search tool. </li>
<li>ISiTrackMaker           - local track-finding in Pixels and SCT tool.</li>
</ul>

It can operate in RoI driven or full-scan mode (m_doFullScan). In the
latter RegionSelector is used to obtain list of SCT and Pixel detector
elements.



*/
