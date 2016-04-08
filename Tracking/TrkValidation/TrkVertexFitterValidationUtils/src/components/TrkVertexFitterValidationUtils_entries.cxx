#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkVertexFitterValidationUtils/TrkPriVxPurityTool.h"
#include "TrkVertexFitterValidationUtils/TrkPriVxPurity.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( TrkPriVxPurityTool )

DECLARE_FACTORY_ENTRIES( TrkVertexFitterValidationUtils ) 
{
   DECLARE_TOOL( TrkPriVxPurityTool );
}
