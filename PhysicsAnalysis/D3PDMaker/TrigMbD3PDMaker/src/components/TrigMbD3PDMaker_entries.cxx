#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigMbD3PDMaker/fillers/BcmRdoFillerTool.h"
#include "TrigMbD3PDMaker/fillers/BCM_RawDataFillerTool.h"
#include "TrigMbD3PDMaker/fillers/CtpRdoFillerTool.h"
#include "TrigMbD3PDMaker/fillers/CtpDecisionFillerTool.h"
#include "TrigMbD3PDMaker/fillers/LucidDigitFillerTool.h"
#include "TrigMbD3PDMaker/fillers/LucidRawDataFillerTool.h"
#include "TrigMbD3PDMaker/fillers/MbtsContainerFillerTool.h"
#include "TrigMbD3PDMaker/fillers/MbtsLvl2FillerTool.h"
#include "TrigMbD3PDMaker/fillers/SpLvl2FillerTool.h"
#include "TrigMbD3PDMaker/fillers/TrtLvl2FillerTool.h"
#include "TrigMbD3PDMaker/fillers/TrkCntsEfFillerTool.h"
#include "TrigMbD3PDMaker/associators/BCM_RDO_CollectionRawDataAssocTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, BcmRdoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, BCM_RawDataFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, CtpRdoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, CtpDecisionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, LucidDigitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, LucidRawDataFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, MbtsContainerFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, MbtsLvl2FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, SpLvl2FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, TrtLvl2FillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, TrkCntsEfFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, BcmRdoGetterTool) // tmp
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, MbtsCollectionGetterTool) // tmp
DECLARE_NAMESPACE_TOOL_FACTORY(TrigMbD3PD, BCM_RDO_CollectionRawDataAssocTool) // tmp


DECLARE_FACTORY_ENTRIES(TrigMbD3PDMaker) {
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, BcmRdoFillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, BCM_RawDataFillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, CtpRdoFillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, CtpDecisionFillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, LucidDigitFillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, LucidRawDataFillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, MbtsContainerFillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, MbtsLvl2FillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, SpLvl2FillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, TrtLvl2FillerTool)
  DECLARE_NAMESPACE_TOOL(TrigMbD3PD, TrkCntsEfFillerTool)
  DECLARE_NAMESPACE_TOOL(MinBiasD3PD, BcmRdoGetterTool) // tmp
  DECLARE_NAMESPACE_TOOL(MinBiasD3PD, MbtsCollectionGetterTool) // tmp
  DECLARE_NAMESPACE_TOOL(MinBiasD3PD, BCM_RDO_CollectionRawDataAssocTool) // tmp
}

