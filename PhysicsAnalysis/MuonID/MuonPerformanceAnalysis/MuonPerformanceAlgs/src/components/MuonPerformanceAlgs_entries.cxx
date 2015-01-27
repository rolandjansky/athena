// MuonPerformanceAlgs_entries.cxx

#include "../MuonTPAlg.h"
#include "../MuonResonanceAlg.h"
#include "../TrackIsolationDecorAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_ALGORITHM_FACTORY(MuonTPAlg)
DECLARE_ALGORITHM_FACTORY(MuonResonanceAlg)
DECLARE_ALGORITHM_FACTORY(TrackIsolationDecorAlg)

DECLARE_FACTORY_ENTRIES(MuonPerformanceAlgs) {
  DECLARE_ALGORITHM(MuonTPAlg)
  DECLARE_ALGORITHM(MuonResonanceAlg)
  DECLARE_ALGORITHM(TrackIsolationDecorAlg)
}

