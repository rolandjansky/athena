#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkVertexFitters/SequentialVertexFitter.h"
#include "TrkVertexFitters/AdaptiveMultiVertexFitter.h"
#include "TrkVertexFitters/AdaptiveVertexFitter.h"
#include "TrkVertexFitters/SequentialVertexSmoother.h"
#include "TrkVertexFitters/DummyVertexSmoother.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( SequentialVertexFitter )
DECLARE_TOOL_FACTORY( AdaptiveMultiVertexFitter )
DECLARE_TOOL_FACTORY( SequentialVertexSmoother )
DECLARE_TOOL_FACTORY( AdaptiveVertexFitter )
DECLARE_TOOL_FACTORY( DummyVertexSmoother)

DECLARE_FACTORY_ENTRIES( TrkVertexFitters )
{
  DECLARE_TOOL( AdaptiveMultiVertexFitter );
  DECLARE_TOOL(SequentialVertexFitter);
  DECLARE_TOOL(SequentialVertexSmoother); 
  DECLARE_TOOL( AdaptiveVertexFitter );
  DECLARE_TOOL(DummyVertexSmoother );
}
