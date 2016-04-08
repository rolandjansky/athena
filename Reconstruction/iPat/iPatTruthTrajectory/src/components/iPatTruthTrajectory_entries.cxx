/***************************************************************************
 iPatTruthTrajectory
 ATLAS Collaboration
 ***************************************************************************/

// $Id: iPatTruthTrajectory_entries.cxx,v 1.4 2005-09-06 10:34:32 pop Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "iPatTruthTrajectory/TruthParameters.h"

DECLARE_TOOL_FACTORY( TruthParameters )

DECLARE_FACTORY_ENTRIES( iPatTruthTrajectory ) 
{
    DECLARE_TOOL( TruthParameters )
}

