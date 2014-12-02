// $Id$
/**
 * @file EventCommonD3PDMaker/src/components/EventCommonD3PDMaker_entries.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief List Gaudi components.
 */


#include "../EventTypeFillerTool.h"
#include "../EventWeightFillerTool.h"
#include "../EventErrorStateFillerTool.h"
#include "../EventTrigInfoFillerTool.h"
#include "../ChargeFillerTool.h"
#include "../FourMomFillerTool.h"
#include "../DRAssociationTool.h"
#include "../DRConeAssociationTool.h"
#include "../CompositeParticleAssociationTool.h"
#include "../NavigableConstituentAssociationTool.h"
#include "../IN4MCollectionGetterTool.h"
#include "../INav4MomLinkContainerGetterTool.h"
#include "../IN4MLinkContainerFlagFillerTool.h"
#include "../LBMetadataTool.h"
#include "../SimMetadataTool.h"
#include "../IParticleLinksCollectionGetterTool.h"
#include "../SkimDecisionFillerTool.h"
#include "../SkimDecisionVectorFillerTool.h"
#include "../LumiBlockLengthFillerTool.h"
#include "../LeadingPtAssociationTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, EventTypeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, EventWeightFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, EventErrorStateFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, EventTrigInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, ChargeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, FourMomFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, DRAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, DRConeAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, CompositeParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, NavigableConstituentAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, IN4MCollectionGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, INav4MomLinkContainerGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, IN4MLinkContainerFlagFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LBMetadataTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SimMetadataTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, IParticleLinksCollectionGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SkimDecisionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, SkimDecisionVectorFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LumiBlockLengthFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LeadingPtAssociationTool)

DECLARE_FACTORY_ENTRIES(EventCommonD3PDMaker) {
  DECLARE_NAMESPACE_TOOL   (D3PD, EventTypeFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, EventWeightFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, EventErrorStateFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, EventTrigInfoFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, ChargeFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, FourMomFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, DRAssociationTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, DRConeAssociationTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, CompositeParticleAssociationTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, NavigableConstituentAssociationTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, IN4MCollectionGetterTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, INav4MomLinkContainerGetterTools)
  DECLARE_NAMESPACE_TOOL   (D3PD, IN4MLinkContainerFlagFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, LBMetadataTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, SimMetadataTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, IParticleLinksCollectionGetterTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, SkimDecisionFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, SkimDecisionVectorFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, LumiBlockLengthFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, LeadingPtAssociationTool)
}
