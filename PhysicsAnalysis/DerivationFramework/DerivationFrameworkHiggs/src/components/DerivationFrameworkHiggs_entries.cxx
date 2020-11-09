#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkHiggs/SkimmingToolHIGG1.h"
#include "DerivationFrameworkHiggs/SkimmingToolHIGG2.h"
#include "DerivationFrameworkHiggs/SkimmingToolHIGG5VBF.h"
#include "DerivationFrameworkHiggs/HIGG3TruthDecorator.h"
#include "DerivationFrameworkHiggs/TruthCategoriesDecorator.h"
#include "DerivationFrameworkHiggs/DiphotonVertexDecorator.h"
#include "DerivationFrameworkHiggs/MergedElectronDetailsDecorator.h"
#include "DerivationFrameworkHiggs/ZeeVertexRefittingTool.h"
#include "DerivationFrameworkHiggs/SoftBVertexTool.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolHIGG1 )
DECLARE_TOOL_FACTORY( SkimmingToolHIGG2 )
DECLARE_TOOL_FACTORY( SkimmingToolHIGG5VBF )
DECLARE_TOOL_FACTORY( HIGG3TruthDecorator )
DECLARE_TOOL_FACTORY( TruthCategoriesDecorator )
DECLARE_TOOL_FACTORY( DiphotonVertexDecorator )
DECLARE_TOOL_FACTORY( MergedElectronDetailsDecorator )
DECLARE_TOOL_FACTORY( ZeeVertexRefittingTool )
DECLARE_TOOL_FACTORY( SoftBVertexTool )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkHiggs ) {
   DECLARE_TOOL( SkimmingToolHIGG1 )
   DECLARE_TOOL( SkimmingToolHIGG2 )
   DECLARE_TOOL( SkimmingToolHIGG5VBF )
   DECLARE_TOOL( HIGG3TruthDecorator )
   DECLARE_TOOL( TruthCategoriesDecorator ) 
   DECLARE_TOOL( DiphotonVertexDecorator )
   DECLARE_TOOL( MergedElectronDetailsDecorator )
   DECLARE_TOOL( ZeeVertexRefittingTool )
   DECLARE_TOOL( SoftBVertexTool )
}

