#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonSegmentMomentum/MuonSegmentMomentum.h"
#include "MuonSegmentMomentum/MuonSegmentMomentumFromField.h"

DECLARE_TOOL_FACTORY( MuonSegmentMomentum )
DECLARE_TOOL_FACTORY( MuonSegmentMomentumFromField )

DECLARE_FACTORY_ENTRIES( MuonSegmentMomentum ) 
{
  DECLARE_TOOL( MuonSegmentMomentum )
  DECLARE_TOOL( MuonSegmentMomentumFromField )
}
