/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGCALOHYPO_TRIGL2CALOLAYERSHYPOTOOL_H
#define TRIGCALOHYPO_TRIGL2CALOLAYERSHYPOTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCaloHypo/ITrigL2CaloLayersHypoTool.h"

/**
 * @class Implementation of the CaloCell Noise Burst selection 
 * @brief 
 **/

class TrigL2CaloLayersHypoTool : public extends<AthAlgTool, ITrigL2CaloLayersHypoTool> { 
 public: 
  TrigL2CaloLayersHypoTool( const std::string& type, 
             const std::string& name, 
             const IInterface* parent );

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<CaloLayersInfo>& input )  const override;

  virtual bool decide( const CaloLayersInfo& i ) const override;


 private:
  HLT::Identifier m_decisionId;

  Gaudi::Property< std::vector<float> > m_EnergyFracCut {this,"EnergyFractionCut",{1.0,1.0,1.0,1.0},"Energy Fraction Cuts"};
  Gaudi::Property< std::vector<float> > m_EnergyAbsCut {this,"AbsoluteEnergyCut",{999999.0, 999999.0, 999999.0, 999999.0},"Absolute Energy Cuts"};
  Gaudi::Property<bool> m_acceptAll {this,"AcceptAll",false,"Accept All"};
  //ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  
}; 

#endif //> !TRIGCALOHYPO_TRIGL2CALOLAYERSHYPOTOOL_H

