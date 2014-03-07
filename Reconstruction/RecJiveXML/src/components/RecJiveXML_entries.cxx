#include "RecJiveXML/JetRecJetRetriever.h"
#include "RecJiveXML/MissingETRetriever.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(JetRecJetRetriever)
DECLARE_TOOL_FACTORY(MissingETRetriever)

DECLARE_FACTORY_ENTRIES(RecJiveXML) {
  DECLARE_ALGTOOL(JetRecJetRetriever)  
  DECLARE_ALGTOOL(MissingETRetriever)
}
