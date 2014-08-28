//#include "TrigSerializeCnv/FooBar.h"

#include "TrigSerializeCnvSvc/TrigSerializeCnvSvc.h"
#include "TrigSerializeCnvSvc/TrigSerializeConverter.h"
#include "TrigSerializeCnvSvc/ITrigSerConvHelper.h"
#include "TrigSerializeCnvSvc/TrigSerializeConvHelper.h"
#include "../TrigSerializeGuidHelper.h"



#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( TrigSerializeConvHelper )
DECLARE_TOOL_FACTORY( TrigSerializeGuidHelper )
DECLARE_SERVICE_FACTORY( TrigSerializeCnvSvc )

DECLARE_FACTORY_ENTRIES( TrigSerializeCnvSvc ) {
  DECLARE_SERVICE( TrigSerializeCnvSvc )
  DECLARE_TOOL( TrigSerializeConvHelper )
  DECLARE_TOOL( TrigSerializeGuidHelper )
}

