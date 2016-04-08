#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../CaloRingsBuilder.h"
#include "../CaloAsymRingsBuilder.h"
#include "../CaloRingerInputReader.h"
#include "../CaloRingerElectronsReader.h"
#include "../CaloRingerPhotonsReader.h"

using namespace Ringer;

DECLARE_TOOL_FACTORY( CaloRingsBuilder          )
DECLARE_TOOL_FACTORY( CaloAsymRingsBuilder      )
DECLARE_TOOL_FACTORY( CaloRingerInputReader     )
DECLARE_TOOL_FACTORY( CaloRingerElectronsReader )
DECLARE_TOOL_FACTORY( CaloRingerPhotonsReader   )

DECLARE_FACTORY_ENTRIES( CaloRingerTools  )
{
  DECLARE_TOOL( CaloRingsBuilder          )
  DECLARE_TOOL( CaloAsymRingsBuilder      )
  DECLARE_TOOL( CaloRingerInputReader     )
  DECLARE_TOOL( CaloRingerElectronsReader )
  DECLARE_TOOL( CaloRingerPhotonsReader   )
}

