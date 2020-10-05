/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONPRECISIONHYPOTOOLINC_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONPRECISIONHYPOTOOLINC_H 1

#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaPrecisionElectronHypoTool.h"
#include "PATCore/AcceptData.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "StoreGate/ReadDecorHandle.h"

/**
 * @class Implementation of the precision selection for electrons
 * @brief 
 **/

class TrigEgammaPrecisionElectronHypoToolInc : public extends<AthAlgTool, ITrigEgammaPrecisionElectronHypoTool> { 
 public: 
  TrigEgammaPrecisionElectronHypoToolInc( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent );

  virtual ~TrigEgammaPrecisionElectronHypoToolInc();
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

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

  /*Likelihood tool*/
  ToolHandle<IAsgElectronLikelihoodTool> m_egammaElectronLHTool;

  /* TRack isolation cut */
  float m_RelPtConeCut;
  
  /*Luminosity info*/
  SG::ReadDecorHandleKey<xAOD::EventInfo> m_avgMuKey { this, "AveIntPerXKey", "EventInfo.AveIntPerXDecor", "Decoration for Average Interaction Per Crossing" };
  int findCutIndex( float eta ) const;

}; 

#endif //> !TRIGEGAMMAHYPO_TRIGPRECISIONELECTRONPRECISIONHYPOTOOL_H
