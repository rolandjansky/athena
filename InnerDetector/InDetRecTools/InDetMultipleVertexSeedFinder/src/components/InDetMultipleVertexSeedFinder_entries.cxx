#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetMultipleVertexSeedFinder/SlidingWindowMultiSeedFinder.h"
#include "InDetMultipleVertexSeedFinder/DivisiveMultiSeedFinder.h"
#include "InDetMultipleVertexSeedFinder/HistogrammingMultiSeedFinder.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( SlidingWindowMultiSeedFinder )
DECLARE_TOOL_FACTORY( DivisiveMultiSeedFinder )
DECLARE_TOOL_FACTORY( HistogrammingMultiSeedFinder )

DECLARE_FACTORY_ENTRIES( TrkMultipleVertexSeedFinder )  
{ 
  DECLARE_TOOL( SlidingWindowMultiSeedFinder );
  DECLARE_TOOL( DivisiveMultiSeedFinder );
  DECLARE_TOOL( HistogrammingMultiSeedFinder );
}
