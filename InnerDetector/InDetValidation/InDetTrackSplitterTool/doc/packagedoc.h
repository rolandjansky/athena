/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page InDetTrackSplitterTool_page InDetTrackSplitterTool Package
@author John Alison <johnda@hep.upenn.edu>, Manuel Kayl <Manuel.Kayl@cern.ch>


This tool is used for splitting tracks in different segments and returning fitted tracks that
have the corrisponding hits associated with them.

Currently it is for seen to split tracks in:

Upper and Lower track segment for cosmics
(However there are lingering problems fitting TRT only tracks)

Tracks using alternating hits. (odd and even)

Usages:
1) If you have a single track and want to split in to upper and lower do 

m_trackSplitterTool->splitInUpperLowerTrack(inputtrack);
->splitInOddEvenHitsTrack(inputtrack);

2) If you have a track collection and want to create track collections with upper and lower tracks do

m_trackSplitterTool->splitTracks(inputTracks);

then retrieve the collection from storeGate with the names m_outputUpperTracksName, m_outputLowerTracksName

*/
