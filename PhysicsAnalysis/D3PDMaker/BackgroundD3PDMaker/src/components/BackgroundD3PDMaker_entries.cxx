// $Id: BackgroundD3PDMaker_entries.cxx 461566 2011-10-05 11:39:02Z salekd $
/**
 * @file BackgroundD3PDMaker/src/components/BackgroundD3PDMaker_entries.cxx
 * @author Mark Tibbetts <mark.james.tibbetts@cern.ch>
 * @date Feb, 2011
 * @brief List Gaudi components.
 */


#include "../LucidPMTHitFillerTool.h"
#include "../RawTagInDetSPFillerTool.h"
#include "../BackgroundWordFillerTool.h"
#include "../BeamBackgroundDecisionFillerTool.h"
#include "../BeamBackgroundClusterFillerTool.h"
#include "../BeamBackgroundClusterAssociationTool.h"
#include "../BeamBackgroundJetFillerTool.h"
#include "../BeamBackgroundJetAssociationTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, LucidPMTHitFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, RawTagInDetSPFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, BackgroundWordFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, BeamBackgroundDecisionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, BeamBackgroundClusterFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, BeamBackgroundClusterAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, BeamBackgroundJetFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, BeamBackgroundJetAssociationTool)

DECLARE_FACTORY_ENTRIES(BackgroundD3PDMaker) {
  DECLARE_NAMESPACE_TOOL   (D3PD, LucidPMTHitFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, RawTagInDetSPFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, BackgroundWordFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, BeamBackgroundDecisionFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, BeamBackgroundClusterFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, BeamBackgroundClusterAssociationTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, BeamBackgroundJetFillerTool)
  DECLARE_NAMESPACE_TOOL   (D3PD, BeamBackgroundJetAssociationTool)
}
