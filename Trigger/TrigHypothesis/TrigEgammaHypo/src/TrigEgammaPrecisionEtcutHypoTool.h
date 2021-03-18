/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_PRECISIONETCUTHYPOTOOL_H
#define TRIGEGAMMAHYPO_PRECISIONETCUTHYPOTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "ITrigEgammaPrecisionEtcutHypoTool.h"

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigEgammaPrecisionEtcutHypoTool : public extends<AthAlgTool, ITrigEgammaPrecisionEtcutHypoTool> { 
 public: 
  TrigEgammaPrecisionEtcutHypoTool( const std::string& type, const std::string& name, const IInterface* parent );

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaPrecisionEtcutHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide() const override;

 private:
  HLT::Identifier m_decisionId;
  
  //Calorimeter electron ID  cuts
  Gaudi::Property< bool > m_acceptAll { this, "AcceptAll", false , "Ignore selection" };  

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  int findCutIndex( float eta ) const;
}; 

#endif //> !TRIGEGAMMAHYPO_PRECISIONETCUTHYPOTOOL_H

