#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkHiggs/SkimmingToolHIGG1.h"
#include "DerivationFrameworkHiggs/SkimmingToolHIGG2.h"
#include "DerivationFrameworkHiggs/SkimmingToolHIGG5VBF.h"
#include "DerivationFrameworkHiggs/TruthCategoriesDecorator.h"
#include "DerivationFrameworkHiggs/DiphotonVertexDecorator.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolHIGG1 )
DECLARE_TOOL_FACTORY( SkimmingToolHIGG2 )
DECLARE_TOOL_FACTORY( SkimmingToolHIGG5VBF )
DECLARE_TOOL_FACTORY( TruthCategoriesDecorator )
DECLARE_TOOL_FACTORY( DiphotonVertexDecorator )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkHiggs ) {
   DECLARE_TOOL( SkimmingToolHIGG1 )
   DECLARE_TOOL( SkimmingToolHIGG2 )
   DECLARE_TOOL( SkimmingToolHIGG5VBF )
   DECLARE_TOOL( TruthCategoriesDecorator ) 
   DECLARE_TOOL( DiphotonVertexDecorator )
}

