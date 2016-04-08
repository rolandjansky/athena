// $Id: ForwardDetectorsD3PDMaker_entries.cxx 597942 2014-05-20 12:31:43Z nozka $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../ZdcFillerTool.h"
#include "../ZdcDigitsFillerTool.h"
#include "../ZdcRecoFillerTool.h"

#include "../AlfaHitCollectionFillerTool.h"
#include "../AlfaEventHeaderFillerTool.h"
#include "../AlfaTrackingDataFillerTool.h"
#include "../AlfaTruthInfoFillerTool.h"
#include "../AlfaMetaDataTool.h"

#include "../AfpSIDSimHitCollectionFillerTool.h"
#include "../AfpSIDDigiCollectionFillerTool.h"
#include "../AfpSIDLocRecoEvCollectionFillerTool.h"
#include "../AfpTDSimHitCollectionFillerTool.h"
#include "../AfpTDDigiCollectionFillerTool.h"
#include "../AfpTDLocRecoEvCollectionFillerTool.h"
#include "../AfpTruthInfoFillerTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, ZdcFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, ZdcDigitsFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, ZdcRecoFillerTool )

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AlfaHitCollectionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AlfaEventHeaderFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AlfaTrackingDataFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AlfaTruthInfoFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AlfaMetaDataTool )

DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AfpSIDSimHitCollectionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AfpSIDDigiCollectionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AfpSIDLocRecoEvCollectionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AfpTDSimHitCollectionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AfpTDDigiCollectionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AfpTDLocRecoEvCollectionFillerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( D3PD, AfpTruthInfoFillerTool )

DECLARE_FACTORY_ENTRIES( ForwardDetectorsD3PDMaker ) {
  DECLARE_NAMESPACE_TOOL( D3PD, ZdcFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, ZdcDigitsFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, ZdcRecoFillerTool )
  
  DECLARE_NAMESPACE_TOOL( D3PD, AlfaHitCollectionFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AlfaEventHeaderFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AlfaTrackingDataFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AlfaTruthInfoFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AlfaMetaDataTool )

  DECLARE_NAMESPACE_TOOL( D3PD, AfpSIDSimHitCollectionFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AfpSIDDigiCollectionFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AfpSIDLocRecoEvCollectionFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AfpTDSimHitCollectionFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AfpTDDigiCollectionFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AfpTDLocRecoEvCollectionFillerTool )
  DECLARE_NAMESPACE_TOOL( D3PD, AfpTruthInfoFillerTool )
}
