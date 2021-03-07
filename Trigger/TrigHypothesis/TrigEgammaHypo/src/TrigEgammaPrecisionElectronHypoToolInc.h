/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONPRECISIONHYPOTOOLINC_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONPRECISIONHYPOTOOLINC_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "ITrigEgammaPrecisionElectronHypoTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "StoreGate/ReadDecorHandle.h"
#include "xAODEventInfo/EventInfo.h"

/**
 * @class Implementation of the precision selection for electrons
 * @brief 
 **/

class TrigEgammaPrecisionElectronHypoToolInc : public extends<AthAlgTool, ITrigEgammaPrecisionElectronHypoTool> { 
 public: 
  TrigEgammaPrecisionElectronHypoToolInc( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent );

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaPrecisionElectronHypoTool::ElectronInfo>& input, const EventContext& ctx)  const override;
  virtual bool decide( const ITrigEgammaPrecisionElectronHypoTool::ElectronInfo& i, const EventContext& ctx) const override;

 private:
  HLT::Identifier m_decisionId;
  
  //Calorimeter electron ID  cuts
  Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for PRECISION electron selection:eta bins
  Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };
  Gaudi::Property< float > m_detacluster { this, "dETACLUSTERthr", 0. , "" };
  Gaudi::Property< float > m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  
  Gaudi::Property< float > m_RelPtConeCut { this, "RelPtConeCut", -999., "Track isolation cut" };

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  ToolHandle<IAsgElectronLikelihoodTool> m_egammaElectronLHTool{ this, "ElectronLHSelector", "", "Likelihood tool" };

  /*Luminosity info*/
  SG::ReadDecorHandleKey<xAOD::EventInfo> m_avgMuKey { this, "averageInteractionsPerCrossingKey", "EventInfo.averageInteractionsPerCrossing", "Decoration for Average Interaction Per Crossing" };
  int findCutIndex( float eta ) const;

}; 

#endif //> !TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONPRECISIONHYPOTOOL_H
