#include "GaudiKernel/DeclareFactoryEntries.h"

#define TRUTHTOTRACK_IMP
#include "TrkTruthToTrack/TruthToTrack.h"
#include "TrkTruthToTrack/TruthTrackRecordToTrack.h"

DECLARE_NAMESPACE_TOOL_FACTORY(Trk, TruthToTrack)
DECLARE_NAMESPACE_TOOL_FACTORY(Trk, TruthTrackRecordToTrack)

DECLARE_FACTORY_ENTRIES(TrkTruthToTrack) {
  DECLARE_NAMESPACE_TOOL(Trk, TruthToTrack)
  DECLARE_NAMESPACE_TOOL(Trk, TruthTrackRecordToTrack)
}
