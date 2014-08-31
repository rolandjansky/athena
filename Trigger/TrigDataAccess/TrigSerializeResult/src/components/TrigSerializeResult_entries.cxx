#include "TrigSerializeResult/ITrigSerializerToolBase.h"
#include "../../src/TrigTSerializer.h"
#include "GaudiKernel/DeclareFactoryEntries.h"



DECLARE_TOOL_FACTORY( TrigTSerializer )


DECLARE_FACTORY_ENTRIES(TrigSerializeResult) {

    DECLARE_TOOL( TrigTSerializer )
}


