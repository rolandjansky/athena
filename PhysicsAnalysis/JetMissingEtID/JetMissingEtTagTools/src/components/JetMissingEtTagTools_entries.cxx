#include "GaudiKernel/DeclareFactoryEntries.h"

#include "JetMissingEtTagTools/JetMissingEtIdentificationTagTool.h"
#include "JetMissingEtTagTools/JetMissingEtTagTool.h"

 
DECLARE_TOOL_FACTORY( JetMissingEtIdentificationTagTool )
DECLARE_TOOL_FACTORY( JetMetTagTool )
 
DECLARE_FACTORY_ENTRIES( JetMissingEtTagTools ) {
  DECLARE_TOOL( JetMissingEtIdentificationTagTool )
  DECLARE_TOOL( JetMetTagTool )
}

