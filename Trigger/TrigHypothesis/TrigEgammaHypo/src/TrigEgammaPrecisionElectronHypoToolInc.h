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

  virtual StatusCode decide( std::vector<ITrigEgammaPrecisionElectronHypoTool::ElectronInfo>& input)  const override;
  virtual bool decide( const ITrigEgammaPrecisionElectronHypoTool::ElectronInfo& i) const override;

 private:
  HLT::Identifier m_decisionId;
  
  //Calorimeter electron ID  cuts
  Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for PRECISION electron selection:eta bins
  Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };
  Gaudi::Property< float > m_detacluster { this, "dETACLUSTERthr", 0. , "" };
  Gaudi::Property< float > m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  
  Gaudi::Property< float > m_RelPtConeCut { this, "RelPtConeCut", -999., "Track isolation cut" };
  Gaudi::Property<bool>                m_loose{this,"Loose", false, "Decision for Loose tune"};
  Gaudi::Property<bool>                m_vloose{this,"vLoose", false, "Decision for vLoose tune"};
  Gaudi::Property<bool>                m_tight{this,"Tight", false, "Decision for tight tune"};
  Gaudi::Property<bool>                m_medium{this,"Medium", false, "Decision for medium tune"};

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  int findCutIndex( float eta ) const;

}; 

#endif //> !TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONPRECISIONHYPOTOOL_H
