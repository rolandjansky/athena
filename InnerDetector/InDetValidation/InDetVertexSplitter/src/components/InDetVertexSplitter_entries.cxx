#include "../InDetVertexSplitter.h"
#include "../InDetVertexSplitterHist.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( InDetVertexSplitter )
DECLARE_ALGORITHM_FACTORY( InDetVertexSplitterHist )

DECLARE_FACTORY_ENTRIES ( InDetVertexSplitter )
{
  DECLARE_ALGORITHM( InDetVertexSplitter )
  DECLARE_ALGORITHM( InDetVertexSplitterHist )
}
