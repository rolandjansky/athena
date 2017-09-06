/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiSPSeededTrackFinder_page SiSPSeededTrackFinder
@author Markus.Elsing@cern.ch, Igor.Gavrilenko@cern.ch
@section SiSPSeededTrackFinder_SiSPSeededTrackFinderIntro Introduction

This algorithm is devoted to track finding using Pixels and SCT clusters starting from space point seeds
information. Output of this algorithm is Trk::Track collection. 

This algorithm uses three standard tools.

1. ISiSpacePointsSeedMaker - space point seeds maker tool.
2. ISiZvertexMake          - primary verices search tool.
3. ISiTrackMaker           - local track-finding in Pixels and SCT tool.


*/
