// $Id$
/**
 * @file TauD3PDMaker/src/components/TauD3PDMaker_entries.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date September, 2009
 * @brief List Gaudi components.
 */

#include "../TauJetFillerTool.h" 
#include "../TauPIDFillerTool.h" 
#include "../TauLeadTrackFillerTool.h"
#include "../TauCommonDetailsFillerTool.h"
#include "../TauPanTauDetailsFillerTool.h"
#include "../PanTauDetailsFillerTool.h"
#include "../TruthTauFullDPDFillerTool.h"
#include "../EventCalcVarsFillerTool.h"
#include "../TauCalcVarsFillerTool.h" 
#include "../TauRadiusFillerTool.h" 
#include "../TauL1TriggerObjectAssociationTool.h"
#include "../TauL2TriggerObjectAssociationTool.h"
#include "../TauEFTriggerObjectAssociationTool.h"
#include "../TauJetAssociationTool.h"
#include "../TauTrackAssociationTool.h"
#include "../TauSeedCaloTrackAssociationTool.h"
#include "../TauWideTrackAssociationTool.h"
#include "../TauOtherTrackAssociationTool.h"
#include "../TauClustersDetailsFillerTool.h"
#include "../TauPi0ClusterFillerTool.h"
#include "../TauCellsDetailsFillerTool.h"
#include "../TauSecVtxFillerTool.h"
#include "../TauPriVtxFillerTool.h"
#include "../TauTrackPerigeeAssociationTool.h"
#include "../TauWideTrackPerigeeAssociationTool.h"
#include "../TauOtherTrackPerigeeAssociationTool.h"
#include "../TruthTauFillerTool.h"
#include "../TruthTausToSG.h"
#include "../ElectronIsoUD.h"
#include "../SimpleTrackFilter.h"
#include "../TrackCaloParamsFillerTool.h"
#include "../TrackParticleAtTJVAFillerTool.h"
#include "../TruthEgammaFillerTool.h"

//
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauJetFillerTool) // 
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauPIDFillerTool) // 
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauCommonDetailsFillerTool) //
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauPanTauDetailsFillerTool) //
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, PanTauDetailsFillerTool) //
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthTauFullDPDFillerTool) //
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, EventCalcVarsFillerTool) //  
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauCalcVarsFillerTool) // 
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauRadiusFillerTool) // 
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauL1TriggerObjectAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauL2TriggerObjectAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauEFTriggerObjectAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauJetAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauTrackAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauSeedCaloTrackAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauWideTrackAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauOtherTrackAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauClustersDetailsFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauPi0ClusterFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauCellsDetailsFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauSecVtxFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauPriVtxFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauTrackPerigeeAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauWideTrackPerigeeAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauOtherTrackPerigeeAssociationTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthTauFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackCaloParamsFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TauLeadTrackFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TrackParticleAtTJVAFillerTool)//
DECLARE_NAMESPACE_TOOL_FACTORY   (D3PD, TruthEgammaFillerTool)//

    
DECLARE_ALGORITHM_FACTORY(TruthTausToSG)
DECLARE_ALGORITHM_FACTORY(ElectronIsoUD)
DECLARE_ALGORITHM_FACTORY(SimpleTrackFilter)


DECLARE_FACTORY_ENTRIES(TauD3PDMaker) {
  DECLARE_NAMESPACE_TOOL      (D3PD, TauJetFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, TauPIDFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, TauCommonDetailsFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, TauPanTauDetailsFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, PanTauDetailsFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, TruthTauFullDPDFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, EventCalcVarsFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, TauCalcVarsFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, TauRadiusFillerTool) //
  DECLARE_NAMESPACE_TOOL      (D3PD, TauEFTriggerObjectAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauEFTriggerObjectAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauEFTriggerObjectAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauJetAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauTrackAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauSeedCaloTrackAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauWideTrackAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauOtherTrackAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauClustersDetailsFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauPi0ClusterFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauCellsDetailsFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauSecVtxFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauPriVtxFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauTrackPerigeeAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauWideTrackPerigeeAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauOtherTrackPerigeeAssociationTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TruthTauFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TrackCaloParamsFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TauLeadTrackFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TrackParticleAtTJVAFillerTool)//
  DECLARE_NAMESPACE_TOOL      (D3PD, TruthEgammaFillerTool)//

  DECLARE_ALGORITHM(TruthTausToSG)
  DECLARE_ALGORITHM(ElectronIsoUD)
  DECLARE_ALGORITHM(SimpleTrackFilter)
}
