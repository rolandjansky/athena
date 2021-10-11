/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMISSINGETHYPO_TRIGMISSINGETHYPOALG_H
#define TRIGMISSINGETHYPO_TRIGMISSINGETHYPOALG_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "DecisionHandling/HypoBase.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


#include "ITrigMissingETHypoTool.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

/**
 * @class TrigMissingETHypoAlg
 * @brief Implements MET selection for the HLT framework
 **/
class TrigMissingETHypoAlg : public ::HypoBase {
 public: 

  TrigMissingETHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:

  // TODO: Consider taking muonContainer etc in case they are needed in future.
  StatusCode decide(const xAOD::TrigMissingETContainer* metContainer,
                    TrigCompositeUtils::DecisionContainer* newDecisions,
                    const TrigCompositeUtils::DecisionContainer* oldDecisions,
                    const EventContext& context
        ) const;

  ToolHandleArray<ITrigMissingETHypoTool> m_hypoTools {
    this, "HypoTools", {}, "Hypo tools"};
     
  SG::ReadHandleKey< xAOD::TrigMissingETContainer > m_metKey {
    this, "METContainerKey", "HLT_MET", "MET Container name"};

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };  

}; 

#endif

