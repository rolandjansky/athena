#include "TrkJiveXML/SegmentRetriever.h"
#include "TrkJiveXML/TrackRetriever.h"
#include "TrkJiveXML/VertexRetriever.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(SegmentRetriever)
DECLARE_TOOL_FACTORY(TrackRetriever)
DECLARE_TOOL_FACTORY(VertexRetriever)

DECLARE_FACTORY_ENTRIES(TrkJiveXML) {
  DECLARE_ALGTOOL(SegmentRetriever)
  DECLARE_ALGTOOL(TrackRetriever)
  DECLARE_ALGTOOL(VertexRetriever)
}
