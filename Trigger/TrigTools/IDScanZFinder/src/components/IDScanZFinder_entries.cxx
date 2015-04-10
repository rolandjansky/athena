////////////////////////////////////////////////////////////////////////////////
// IDScanZFinder
// -------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "IDScanZFinder/IDScanZFinder.h"
#include "IDScanZFinder/TrigZFinder.h"

DECLARE_TOOL_FACTORY( IDScanZFinder )

DECLARE_FACTORY_ENTRIES( IDScanZFinder )
{
  DECLARE_TOOL( IDScanZFinder )
}

DECLARE_TOOL_FACTORY( TrigZFinder )

DECLARE_FACTORY_ENTRIES( TrigZFinder )
{
  DECLARE_TOOL( TrigZFinder )
}
