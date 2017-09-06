/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"


static const InterfaceID IID_TrigL2CaloHypoTool("TrigL2CaloHypoTool", 1, 0);

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/



class TrigL2CaloHypoTool
  : virtual public ::AthAlgTool
{ 
 public: 
  TrigL2CaloHypoTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent );

  virtual ~TrigL2CaloHypoTool();
  StatusCode initialize() override;
  StatusCode finalize() override;

  static const InterfaceID& interfaceID();

  TrigCompositeUtils::DecisionID decisionId() const {
    return m_id.numeric();
  }

  /**
   * @brief decides upon a single object
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   **/
  bool decide(  const xAOD::TrigEMCluster* cluster, const TrigRoiDescriptor* roi )  const;
  
 private:
  HLT::Identifier m_id;


  //Calorimeter electron ID  cuts
  std::vector<float> m_etabin; //!<  selection variable for L2 calo selection:eta bins
  std::vector<float> m_eTthr;
  std::vector<float> m_eT2thr;
  std::vector<float> m_hadeTthr;
  std::vector<float> m_hadeT2thr;
  std::vector<float> m_carcorethr;
  std::vector<float> m_caeratiothr;
  std::vector<float> m_F1thr;
  std::vector<float> m_WETA2thr;
  std::vector<float> m_WSTOTthr;
  std::vector<float> m_F3thr;
  float m_detacluster;
  float m_dphicluster;
  
  bool m_acceptAll;

  ToolHandle<GenericMonitoringTool> m_monTool;
}; 

inline const InterfaceID& TrigL2CaloHypoTool::interfaceID() 
{ 
   return IID_TrigL2CaloHypoTool; 
}


#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
