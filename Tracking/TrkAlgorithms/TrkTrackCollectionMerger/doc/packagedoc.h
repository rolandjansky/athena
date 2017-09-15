/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkTrackCollectionMerger_page TrkTrackCollectionMerger
@author Thomas.Koffas@cern.ch

@section TrkTrackCollectionMerger_TrkTrackCollectionMergerIntro Introduction

This is an algorithm that merges all individual track collections,
SiSeededTracks, back-tracks, standalone TRT tracks etc. It runs at
the end of the tracking and before the post processing. It ouputs
a global track collection.

@section TrkTrackCollectionMerger_TrkTrackCollectionMergerDescription Description

This algorithm performs the following tasks:

1.  Loops over the input vector of all available track collections.
    For each track collection, the track are saved in a common track
    container that will be stored for later usage. The hit association
    tool also gets updated simultaneuosly.

2.  Using the new common track collection, that track summary tool is then
    updated.

3.  The new collection is stored.

@section TrkTrackCollectionMerger_TrkTrackCollectionMergerTools Used Tools

This algorithm uses the ITrackSummaryTool, IPRD_AssociationTool tools.

@section TrkTrackCollectionMerger_TrkTrackCollectionMergerContents Algorithm Contents

The TrkTrackCollectionMerger contains the following classes/files:

- TrkTrackCollectionMerger.cxx...Main class calling all the necessary tools and producing the ouput track collection.




*/
