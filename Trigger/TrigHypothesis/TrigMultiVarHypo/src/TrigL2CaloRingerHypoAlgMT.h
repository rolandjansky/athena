/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGL2CALORINGERHYPOALGMT_H
#define TRIGL2CALORINGERHYPOALGMT_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "AthViews/View.h"

#include "TrigL2CaloRingerHypoToolMT.h"



class TrigL2CaloRingerHypoAlgMT
  : public ::HypoBase
{ 
 public: 

  TrigL2CaloRingerHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigL2CaloRingerHypoAlgMT(); 
  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override;
  virtual StatusCode  execute(const EventContext& context) const override;

 
 private: 
  TrigL2CaloRingerHypoAlgMT();
  ToolHandleArray< TrigL2CaloRingerHypoToolMT > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
  SG::ReadHandleKey<xAOD::TrigRingerRingsContainer> m_ringsKey { this, "RingerKey","CaloRings",""};
  SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_clustersKey { this,"ClustersKey","CaloClusters",""};



}; 
//DECLARE_ALGORITHM_FACTORY( TrigL2CaloRingerHypoAlgMT )

#endif //> !MULTIVARHYPO_TRIGL2CALORINGERHYPOALG_H
