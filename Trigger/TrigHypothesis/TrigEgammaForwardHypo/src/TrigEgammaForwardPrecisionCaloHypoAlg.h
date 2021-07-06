/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaForwardPrecisionCaloHypoAlg_h
#define TrigEgammaForwardPrecisionCaloHypoAlg_h

#include <string>

#include "AthViews/View.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "ITrigEgammaForwardPrecisionCaloHypoTool.h"

/**
 * @class TrigEgammaForwardPrecisionCaloHypoAlg
 * @brief Implements egamma calo selection for the new HLT framework
 **/
class TrigEgammaForwardPrecisionCaloHypoAlg : public ::HypoBase {
  public: 

    TrigEgammaForwardPrecisionCaloHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual StatusCode  initialize() override;
    virtual StatusCode  execute( const EventContext& context ) const override;

  private: 
    ToolHandleArray< ITrigEgammaForwardPrecisionCaloHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" }; 
    SG::ReadHandleKey< xAOD::CaloClusterContainer > m_clustersKey { this, "CaloClusters", "CaloClusters", "CaloClusters in roi" };  
  

}; 

#endif //> !TRIGEGAMMAHYPO_TESTTRIGPRECISIONCALOHYPOALG_H
