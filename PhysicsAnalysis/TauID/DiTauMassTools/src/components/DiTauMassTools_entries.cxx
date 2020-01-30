#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DiTauMassTools/MissingMassTool.h"
#include "DiTauMassTools/MissingMassToolV2.h"

DECLARE_TOOL_FACTORY( MissingMassTool )
DECLARE_NAMESPACE_TOOL_FACTORY( DiTauMassTools, MissingMassToolV2 )

DECLARE_FACTORY_ENTRIES( DiTauMassTools ) 
{
  DECLARE_TOOL( MissingMassTool );
  DECLARE_NAMESPACE_TOOL( DiTauMassTools, MissingMassToolV2 );
}
