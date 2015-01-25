#include "TruthJiveXML/TruthTrackRetriever.h"
#include "TruthJiveXML/TruthMuonTrackRetriever.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(TruthTrackRetriever)
DECLARE_TOOL_FACTORY(TruthMuonTrackRetriever)

DECLARE_FACTORY_ENTRIES(TruthJiveXML) {
  DECLARE_ALGTOOL(TruthTrackRetriever)
  DECLARE_ALGTOOL(TruthMuonTrackRetriever)
}
