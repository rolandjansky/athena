#include "CaloJiveXML/CaloLArRetriever.h"
#include "CaloJiveXML/CaloHECRetriever.h"
#include "CaloJiveXML/CaloFCalRetriever.h"
#include "CaloJiveXML/CaloMBTSRetriever.h"
#include "CaloJiveXML/CaloTileRetriever.h"
#include "CaloJiveXML/CaloClusterRetriever.h"
#include "CaloJiveXML/LArDigitRetriever.h"
#include "CaloJiveXML/BadTileRetriever.h"
#include "CaloJiveXML/BadLArRetriever.h"

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

