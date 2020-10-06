/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOTOOLINC_H
#define TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOTOOLINC_H 1

#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaPrecisionPhotonHypoTool.h"
#include "PATCore/AcceptData.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "StoreGate/ReadDecorHandle.h"

/**
 * @class Implementation of the Egamma selection for Photons
 * @brief 
 **/

class TrigEgammaPrecisionPhotonHypoToolInc : public extends<AthAlgTool, ITrigEgammaPrecisionPhotonHypoTool> { 
 public: 
  TrigEgammaPrecisionPhotonHypoToolInc( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent );

  virtual ~TrigEgammaPrecisionPhotonHypoToolInc();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaPrecisionPhotonHypoTool::PhotonInfo>& input )  const override;

  virtual bool decide( const ITrigEgammaPrecisionPhotonHypoTool::PhotonInfo& i ) const override;

 private:
  HLT::Identifier m_decisionId;
  
  //Calorimeter electron ID  cuts
  Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for PRECISION calo selection:eta bins
  Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };
  Gaudi::Property< float > m_detacluster { this, "dETACLUSTERthr", 0. , "" };
  Gaudi::Property< float > m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  

 /* Calo isolation cut */
  float m_RelEtConeCut;

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  ToolHandle<IAsgPhotonIsEMSelector> m_egammaPhotonCutIDTool;
/*Luminosity info*/
  SG::ReadDecorHandleKey<xAOD::EventInfo> m_avgMuKey { this, "AveIntPerXKey", "EventInfo.AveIntPerXDecor", "Decoration for Average Interaction Per Crossing" };

  int findCutIndex( float eta ) const;
}; 

#endif //> !TRIGEGAMMAHYPO_TRIGPRECISIONPHOTONHYPOTOOL_H
