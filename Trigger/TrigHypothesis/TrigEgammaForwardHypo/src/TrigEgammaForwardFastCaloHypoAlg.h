/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaForwardFastCaloHypoAlg_h
#define TrigEgammaForwardFastCaloHypoAlg_h

#include <string>
#include "DecisionHandling/HypoBase.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "ITrigEgammaForwardFastCaloHypoTool.h"

/**
 * @class TrigEgammaForwardFastCaloHypoAlg
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaForwardFastCaloHypoAlg : public ::HypoBase {
  public:

    TrigEgammaForwardFastCaloHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;

  private:
  
    ToolHandle< GenericMonitoringTool > m_monTool{ this, "MonTool", "", "Monitoring tool" };
    ToolHandleArray< ITrigEgammaForwardFastCaloHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
    SG::ReadHandleKey< xAOD::TrigEMClusterContainer > m_clustersKey { this, "CaloClusters", "HLT_ForwardFastCaloEMClusters", "CaloClusters in view" };
    SG::ReadHandleKey<xAOD::TrigRingerRingsContainer> m_ringsKey { this, "RingerKey","HLT_ForwardFastCaloRinger","Point to RingerKey"};
}; 

#endif
