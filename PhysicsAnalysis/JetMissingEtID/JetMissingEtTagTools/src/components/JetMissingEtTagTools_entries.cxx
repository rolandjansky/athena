#include "JetMissingEtTagTools/JetMissingEtIdentificationTagTool.h"
#include "JetMissingEtTagTools/JetMissingEtTagTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
 
DECLARE_TOOL_FACTORY( JetMissingEtIdentificationTagTool )
DECLARE_TOOL_FACTORY( JetMissingEtTagTool )
 
DECLARE_FACTORY_ENTRIES( JetMissingEtTagTools ) {
  DECLARE_TOOL( JetMissingEtIdentificationTagTool )
  DECLARE_TOOL( JetMissingEtTagTool )
}

