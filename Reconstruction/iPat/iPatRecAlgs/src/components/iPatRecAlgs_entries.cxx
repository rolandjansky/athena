/***************************************************************************
 iPatRecAlgs
 -----------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: iPatRecAlgs_entries.cxx,v 1.12 2008-12-16 15:52:48 pop Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "iPatRecAlgs/iPatRec.h"
#include "iPatRecAlgs/iPatShortTracks.h"
#include "iPatRecAlgs/iPatStatistics.h"
#include "iPatRecAlgs/iPatTrackTruthAssociator.h"
#include "iPatRecAlgs/IntersectorTest.h"

DECLARE_ALGORITHM_FACTORY( iPatRec )
DECLARE_ALGORITHM_FACTORY( iPatShortTracks )
DECLARE_ALGORITHM_FACTORY( iPatStatistics )
DECLARE_ALGORITHM_FACTORY( iPatTrackTruthAssociator )
DECLARE_ALGORITHM_FACTORY( IntersectorTest )

DECLARE_FACTORY_ENTRIES( iPatRecAlgs ) 
{
    DECLARE_ALGORITHM( iPatRec )
    DECLARE_ALGORITHM( iPatShortTracks )
    DECLARE_ALGORITHM( iPatStatistics )
    DECLARE_ALGORITHM( iPatTrackTruthAssociator )
    DECLARE_ALGORITHM( IntersectorTest )	
}

