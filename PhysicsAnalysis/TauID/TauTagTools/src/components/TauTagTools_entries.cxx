#include "TauTagTools/TauIdentificationTagTool.h"
#include "TauTagTools/TauJetTagTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
 
DECLARE_TOOL_FACTORY( TauIdentificationTagTool )
DECLARE_TOOL_FACTORY( TauJetTagTool )
 
DECLARE_FACTORY_ENTRIES( TauTagTools ) {
  DECLARE_TOOL( TauIdentificationTagTool )
  DECLARE_TOOL( TauJetTagTool )
}

