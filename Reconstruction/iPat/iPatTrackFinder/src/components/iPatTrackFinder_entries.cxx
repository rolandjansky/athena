/***************************************************************************
 iPatTrackFinder
 ATLAS Collaboration
 ***************************************************************************/

// $Id: iPatTrackFinder_entries.cxx,v 1.6 2007-02-12 18:09:40 pop Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "iPatTrackFinder/AssignedHits.h"
#include "iPatTrackFinder/CandidateBuilder.h"
#include "iPatTrackFinder/CombinationMaker.h"
#include "iPatTrackFinder/FinderTolerances.h"
#include "iPatTrackFinder/PointManager.h"
#include "iPatTrackFinder/TrackFinder.h"
#include "iPatTrackFinder/TrackManager.h"

DECLARE_TOOL_FACTORY( AssignedHits )
DECLARE_TOOL_FACTORY( CandidateBuilder )
DECLARE_TOOL_FACTORY( CombinationMaker )
DECLARE_TOOL_FACTORY( FinderTolerances )
DECLARE_TOOL_FACTORY( PointManager )
DECLARE_TOOL_FACTORY( TrackFinder )
DECLARE_TOOL_FACTORY( TrackManager )

DECLARE_FACTORY_ENTRIES( iPatTrackFinder ) 
{
    DECLARE_TOOL( AssignedHits )
    DECLARE_TOOL( CandidateBuilder )	
    DECLARE_TOOL( CombinationMaker )	
    DECLARE_TOOL( FinderTolerances )
    DECLARE_TOOL( PointManager )
    DECLARE_TOOL( TrackFinder )
    DECLARE_TOOL( TrackManager )	
}

