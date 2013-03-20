// $Id$
/**
 * @file TrigTauD3PDMaker/src/components/TrigTauD3PDMaker_entries.cxx
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date December, 2009
 * @brief List Gaudi components.
 */

#include "TrigTauD3PDMaker/TrigReadNavTool.h"

#include "../TrigTauChainFiller.h"
#include "../TrigL2TauClusterAssTool.h"
#include "../TrigL2TauTracksInfoAssTool.h"
#include "../TrigL2InDetTrackAssTool.h"
#include "../TrigL2InDetTracksAssTool.h"
#include "../TrigEFTauObjectDecisionFillerTool.h"
#include "../TrigL2TauObjectDecisionFillerTool.h"
#include "../TrigEFTauCommonDetailsFiller.h" 
#include "../TrigL2TauTrigTauFiller.h"
#include "../TrigL2TauClusterFiller.h"
#include "../TrigL2TauTracksInfoFiller.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY   (TrigTauTool, TrigReadNavTool)

DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigTauChainFiller)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigL2TauClusterAssTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigL2TauTracksInfoAssTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigL2InDetTrackAssTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigL2InDetTracksAssTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigEFTauObjectDecisionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigL2TauObjectDecisionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigEFTauCommonDetailsFiller)  
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigL2TauTrigTauFiller)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigL2TauClusterFiller)
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrigL2TauTracksInfoFiller)

DECLARE_FACTORY_ENTRIES(TrigTauD3PDMaker) {
  DECLARE_NAMESPACE_TOOL      (TrigTauTool, TrigReadNavTool)

  DECLARE_NAMESPACE_TOOL      (D3PD, TrigTauChainFiller)  
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigL2TauClusterAssTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigL2TauTracksInfoAssTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigL2InDetTrackAssTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigL2InDetTracksAssTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigEFTauObjectDecisionFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigL2TauObjectDecisionFillerTool) 
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigEFTauCommonDetailsFiller) 
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigL2TauTrigTauFiller)
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigL2TauClusterFiller)
  DECLARE_NAMESPACE_TOOL      (D3PD, TrigL2TauTracksInfoFiller)
}

