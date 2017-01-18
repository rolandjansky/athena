// MuonPerformanceAlgs_entries.cxx

#include "../MuonTPAlg.h"
#include "../MuonResonanceAlg.h"
#include "../HighPtTrackPicker.h"
#include "../TrackIsolationDecorAlg.h"
#include "../MuonTPMetaDataAlg.h"
#include "../MuonTPJpsiVertexFittingAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(MuonTPAlg)
DECLARE_ALGORITHM_FACTORY(HighPtTrackPicker)
DECLARE_ALGORITHM_FACTORY(MuonResonanceAlg)
DECLARE_ALGORITHM_FACTORY(TrackIsolationDecorAlg)
DECLARE_ALGORITHM_FACTORY(MuonTPMetaDataAlg)
DECLARE_ALGORITHM_FACTORY(MuonTPJpsiVertexFittingAlg)


DECLARE_FACTORY_ENTRIES(MuonPerformanceAlgs) {
  DECLARE_ALGORITHM(MuonTPMetaDataAlg );
  DECLARE_ALGORITHM(MuonTPAlg)
  DECLARE_ALGORITHM(MuonResonanceAlg)
  DECLARE_ALGORITHM(HighPtTrackPicker)
  DECLARE_ALGORITHM(TrackIsolationDecorAlg)
  DECLARE_ALGORITHM(MuonTPJpsiVertexFittingAlg)
}

