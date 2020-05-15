/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGL2CALORINGERHYPOALGMT_H
#define TRIGL2CALORINGERHYPOALGMT_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "ITrigL2CaloRingerHypoToolMT.h"



class TrigL2CaloRingerHypoAlgMT
  : public ::HypoBase
{
 public:

  TrigL2CaloRingerHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute(const EventContext& context) const override;


 private:

  ToolHandleArray< ITrigL2CaloRingerHypoToolMT > m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
  SG::ReadHandleKey<xAOD::TrigRingerRingsContainer> m_ringsKey { this, "RingerKey","HLT_FastCaloRinger","Point to RingerKey"};
  SG::ReadHandleKey<xAOD::TrigEMClusterContainer> m_clustersKey { this,"ClustersKey","ClustersKey","ClustersKey in view"};

};
//DECLARE_ALGORITHM_FACTORY( TrigL2CaloRingerHypoAlgMT )

#endif //> !MULTIVARHYPO_TRIGL2CALORINGERHYPOALG_H
