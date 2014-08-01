#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigT2BeamSpot/T2VertexBeamSpot.h"

using namespace PESA;

DECLARE_ALGORITHM_FACTORY(T2VertexBeamSpot)

DECLARE_FACTORY_ENTRIES(TrigT2BeamSpot) {
  DECLARE_ALGORITHM(T2VertexBeamSpot);
}
