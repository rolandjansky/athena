/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOLINC_H
#define TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOLINC_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "ITrigL2CaloHypoTool.h"

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigL2CaloHypoToolInc : public extends<AthAlgTool, ITrigL2CaloHypoTool> { 
 public: 
  TrigL2CaloHypoToolInc( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent );

  virtual ~TrigL2CaloHypoToolInc();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigL2CaloHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide( const ITrigL2CaloHypoTool::ClusterInfo& i ) const override;

 private:
  HLT::Identifier m_decisionId;
  
  //Calorimeter electron ID  cuts
  Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for L2 calo selection:eta bins
  Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };
  Gaudi::Property< std::vector<float> > m_eT2thr { this, "ET2thr", {}, "Second layer ET threshold" };
  Gaudi::Property< std::vector<float> > m_hadeTthr { this, "HADETthr", {}, "" };
  Gaudi::Property< std::vector<float> > m_hadeT2thr { this, "HADET2thr", {}, "" };
  Gaudi::Property< std::vector<float> > m_carcorethr { this, "CARCOREthr", {}, "" };
  Gaudi::Property< std::vector<float> > m_caeratiothr { this, "CAERATIOthr", {}, "" };
  Gaudi::Property< std::vector<float> > m_F1thr { this, "F1thr", {}, "" };
  Gaudi::Property< std::vector<float> > m_WETA2thr { this, "WETA2thr", {}, "" };
  Gaudi::Property< std::vector<float> > m_WSTOTthr { this, "WSTOTthr", {}, "" };
  Gaudi::Property< std::vector<float> > m_F3thr { this, "F3thr", {}, "" };
  Gaudi::Property< float > m_detacluster { this, "dETACLUSTERthr", 0. , "" };
  Gaudi::Property< float > m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  
  Gaudi::Property< bool > m_acceptAll { this, "AcceptAll", false , "Ignore selection" };
  
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  
  int findCutIndex( float eta ) const;
}; 
DECLARE_TOOL_FACTORY( TrigL2CaloHypoToolInc )
#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
