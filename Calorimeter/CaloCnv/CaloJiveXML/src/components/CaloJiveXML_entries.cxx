#include "CaloJiveXML/CaloLArRetriever.h"
#include "CaloJiveXML/CaloHECRetriever.h"
#include "CaloJiveXML/CaloFCalRetriever.h"
#include "CaloJiveXML/CaloMBTSRetriever.h"
#include "CaloJiveXML/CaloTileRetriever.h"
#include "CaloJiveXML/CaloClusterRetriever.h"
#include "CaloJiveXML/LArDigitRetriever.h"
#include "CaloJiveXML/BadTileRetriever.h"
#include "CaloJiveXML/BadLArRetriever.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(CaloTileRetriever)
DECLARE_TOOL_FACTORY(CaloMBTSRetriever)
DECLARE_TOOL_FACTORY(CaloLArRetriever)
DECLARE_TOOL_FACTORY(CaloFCalRetriever)
DECLARE_TOOL_FACTORY(CaloHECRetriever)
DECLARE_TOOL_FACTORY(CaloClusterRetriever)
DECLARE_TOOL_FACTORY(LArDigitRetriever)
DECLARE_TOOL_FACTORY(BadTileRetriever)
DECLARE_TOOL_FACTORY(BadLArRetriever)

DECLARE_FACTORY_ENTRIES(CaloJiveXML) {
  DECLARE_ALGTOOL(CaloTileRetriever)
  DECLARE_ALGTOOL(CaloMBTSRetriever)
  DECLARE_ALGTOOL(CaloLArRetriever)
  DECLARE_ALGTOOL(CaloHECRetriever)
  DECLARE_ALGTOOL(CaloFCalRetriever)
  DECLARE_ALGTOOL(CaloClusterRetriever)
  DECLARE_ALGTOOL(LArDigitRetriever)
  DECLARE_ALGTOOL(BadTileRetriever)
  DECLARE_ALGTOOL(BadLArRetriever)
}
