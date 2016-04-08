#include "TrkRefitAlg/ReFitTrack.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace Trk;
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, ReFitTrack )

DECLARE_FACTORY_ENTRIES(TrkRefitAlg) {
    DECLARE_NAMESPACE_ALGORITHM( Trk, ReFitTrack)
}
