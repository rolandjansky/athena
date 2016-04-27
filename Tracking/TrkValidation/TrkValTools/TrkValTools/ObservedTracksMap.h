/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 CLASS_DEF for ObservedTracksMap used in TrkObserverTool
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

#ifndef OBSERVERMAP_H
#define OBSERVERMAP_H 1

//<<<<<< INCLUDES                                                       >>>>>>
#include "SGTools/CLASS_DEF.h"
#include <map>

//<<<<<< PUBLIC DEFINES  >>>>>>

// Id, parentId, score, rejection location, unique Id, unique parentId - numPixelHoles, numSCTHoles, numSplitSharedPixel, numSplitSharedSCT, numSharedOrSplit, numSharedOrSplitPixels, numShared
typedef std::multimap< const Trk::Track*, std::tuple< const Trk::Track*, double, int, Trk::Track*, Trk::Track*, int, int, int, int, int, int, int> > ObservedTracksMap;


CLASS_DEF( ObservedTracksMap , 717976956 , 1 )



#endif // OBSERVERMAP
