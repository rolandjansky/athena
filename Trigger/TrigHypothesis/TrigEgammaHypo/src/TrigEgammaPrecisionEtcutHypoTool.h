/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_PRECISIONETCUTHYPOTOOL_H
#define TRIGEGAMMAHYPO_PRECISIONETCUTHYPOTOOL_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaPrecisionEtcutHypoTool.h"

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigEgammaPrecisionEtcutHypoTool : public extends<AthAlgTool, ITrigEgammaPrecisionEtcutHypoTool> { 
 public: 
  TrigEgammaPrecisionEtcutHypoTool( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~TrigEgammaPrecisionEtcutHypoTool();
  virtual StatusCode initialize() override;

  virtual StatusCode decide( std::vector<ITrigEgammaPrecisionEtcutHypoTool::ClusterInfo>& input )  const override;

  virtual bool decide( const ITrigEgammaPrecisionEtcutHypoTool::ClusterInfo& i ) const override;

 private:
  HLT::Identifier m_decisionId;
  
  //Calorimeter electron ID  cuts
  Gaudi::Property< bool > m_acceptAll { this, "AcceptAll", false , "Ignore selection" };  

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  int findCutIndex( float eta ) const;
}; 

#endif //> !TRIGEGAMMAHYPO_PRECISIONETCUTHYPOTOOL_H
