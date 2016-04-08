#include "TrigJiveXML/EmTauROIRetriever.h"
#include "TrigJiveXML/TrigInDetTrackRetriever.h"
#include "TrigJiveXML/JetROIRetriever.h"
#include "TrigJiveXML/LVL1JetElementRetriever.h"
#include "TrigJiveXML/LVL1ResultRetriever.h"
#include "TrigJiveXML/LVL1TrigTowerRetriever.h"
#include "TrigJiveXML/MuonROIRetriever.h"
#include "TrigJiveXML/TrigSiSpacePointRetriever.h"
#include "TrigJiveXML/TriggerInfoRetriever.h"
#include "TrigJiveXML/CTPDecisionRetriever.h"
#include "TrigJiveXML/xAODEmTauROIRetriever.h"
#include "TrigJiveXML/xAODJetROIRetriever.h"
#include "TrigJiveXML/xAODMuonROIRetriever.h"
#include "TrigJiveXML/xAODTrigDecisionRetriever.h"
#include "TrigJiveXML/xAODTriggerTowerRetriever.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(EmTauROIRetriever)
DECLARE_TOOL_FACTORY(TrigInDetTrackRetriever)
DECLARE_TOOL_FACTORY(JetROIRetriever)
DECLARE_TOOL_FACTORY(LVL1JetElementRetriever)
DECLARE_TOOL_FACTORY(LVL1ResultRetriever)
DECLARE_TOOL_FACTORY(LVL1TrigTowerRetriever)
DECLARE_TOOL_FACTORY(MuonROIRetriever)
DECLARE_TOOL_FACTORY(TrigSiSpacePointRetriever)
DECLARE_TOOL_FACTORY(TriggerInfoRetriever)
DECLARE_TOOL_FACTORY(CTPDecisionRetriever)
DECLARE_TOOL_FACTORY(xAODEmTauROIRetriever)
DECLARE_TOOL_FACTORY(xAODJetROIRetriever)
DECLARE_TOOL_FACTORY(xAODMuonROIRetriever)
DECLARE_TOOL_FACTORY(xAODTrigDecisionRetriever)
DECLARE_TOOL_FACTORY(xAODTriggerTowerRetriever)

DECLARE_FACTORY_ENTRIES(TrigJiveXML) {
  DECLARE_ALGTOOL(EmTauROIRetriever)
  DECLARE_ALGTOOL(TrigInDetTrackRetriever)
  DECLARE_ALGTOOL(JetROIRetriever)
  DECLARE_ALGTOOL(LVL1JetElementRetriever)
  DECLARE_ALGTOOL(LVL1ResultRetriever)
  DECLARE_ALGTOOL(LVL1TrigTowerRetriever)
  DECLARE_ALGTOOL(MuonROIRetriever)
  DECLARE_ALGTOOL(TrigSiSpacePointRetriever)
  DECLARE_ALGTOOL(TriggerInfoRetriever)
  DECLARE_ALGTOOL(CTPDecisionRetriever)
  DECLARE_ALGTOOL(xAODEmTauROIRetriever)
  DECLARE_ALGTOOL(xAODJetROIRetriever)
  DECLARE_ALGTOOL(xAODMuonROIRetriever)
  DECLARE_ALGTOOL(xAODTrigDecisionRetriever)
  DECLARE_ALGTOOL(LVL1TriggerTowerRetriever)
  DECLARE_ALGTOOL(xAODTriggerTowerRetriever)
}
