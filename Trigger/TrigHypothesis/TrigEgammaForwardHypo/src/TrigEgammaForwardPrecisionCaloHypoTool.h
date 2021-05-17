/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaForwardPrecisionCaloHypoTool_h
#define TrigEgammaForwardPrecisionCaloHypoTool_h

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaForwardPrecisionCaloHypoTool.h"

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigEgammaForwardPrecisionCaloHypoTool : public extends<AthAlgTool, ITrigEgammaForwardPrecisionCaloHypoTool> { 
 public: 
  TrigEgammaForwardPrecisionCaloHypoTool( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent );

  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaForwardPrecisionCaloHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide( const ITrigEgammaForwardPrecisionCaloHypoTool::ClusterInfo& i ) const override;

 private:
  HLT::Identifier m_decisionId;
  
  //Calorimeter electron ID  cuts
  Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for PRECISION calo selection:eta bins
  Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };
  Gaudi::Property< std::vector<float> > m_eT2thr { this, "ET2thr", {}, "Second layer ET threshold" };
  Gaudi::Property< float > m_detacluster { this, "dETACLUSTERthr", 0. , "" };
  Gaudi::Property< float > m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  int findCutIndex( float eta ) const;
}; 

#endif //> !TRIGEGAMMAHYPO_PRECISIONCALOHYPOTOOL_H
