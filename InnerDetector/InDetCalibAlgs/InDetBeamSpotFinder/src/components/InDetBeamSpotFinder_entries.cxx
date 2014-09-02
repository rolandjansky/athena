#include "../InDetBeamSpotFinder.h"
#include "../InDetBeamSpotReader.h"
#include "../InDetBeamSpotVertex.h"
#include "../InDetBeamSpotRooFit.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDet, InDetBeamSpotFinder )
DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDet, InDetBeamSpotReader )

DECLARE_NAMESPACE_TOOL_FACTORY(InDet, InDetBeamSpotVertex)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet, InDetBeamSpotRooFit)

DECLARE_FACTORY_ENTRIES(InDetBeamSpotFinder){
  DECLARE_NAMESPACE_ALGORITHM(InDet, InDetBeamSpotFinder)
  DECLARE_NAMESPACE_ALGORITHM(InDet, InDetBeamSpotReader)
  DECLARE_NAMESPACE_TOOL(InDet, InDetBeamSpotVertex)
  DECLARE_NAMESPACE_TOOL(InDet, InDetBeamSpotRooFit)
}

