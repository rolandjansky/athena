#include "GeometryJiveXML/GeometryWriter.h"
#include "GeometryJiveXML/MuonGeometryWriter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(GeometryWriter)
DECLARE_TOOL_FACTORY(MuonGeometryWriter)

DECLARE_FACTORY_ENTRIES(GeometryJiveXML) {
  DECLARE_ALGTOOL(GeometryWriter)
  DECLARE_ALGTOOL(MuonGeometryWriter)
}
