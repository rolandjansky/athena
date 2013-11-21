#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonSegmentCleaner/MuonSegmentAmbiCleaner.h"
#include "MuonSegmentCleaner/MuonPhiHitSelector.h"

DECLARE_TOOL_FACTORY( MuonSegmentAmbiCleaner )
DECLARE_TOOL_FACTORY( MuonPhiHitSelector )

DECLARE_FACTORY_ENTRIES( MuonSegmentCleaner ) 
{
  DECLARE_TOOL( MuonSegmentAmbiCleaner )
  DECLARE_TOOL( MuonPhiHitSelector )
}
