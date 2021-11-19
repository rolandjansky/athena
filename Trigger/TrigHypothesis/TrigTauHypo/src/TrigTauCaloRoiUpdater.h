// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TrigTauCaloRoiUpdater_H
#define  TrigTauCaloRoiUpdater_H

#include <iostream>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
//#include "xAODJet/JetContainer.h"
//#include "xAODJet/JetAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class TrigTauCaloRoiUpdater : public AthAlgorithm {

 public:
  TrigTauCaloRoiUpdater(const std::string&, ISvcLocator*);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  Gaudi::Property< float > m_dRForCenter {this,"dRForCenter",0.2,"Delta R from the center of ROI"};
  Gaudi::Property< float > m_etaHalfWidth {this,"etaHalfWidth",0.1,"eta Half width for tracking"};
  Gaudi::Property< float > m_phiHalfWidth {this,"phiHalfWidth",0.1,"phi Half width for tracking"};

  //SG::ReadHandleKey< xAOD::JetContainer > m_jetInputKey {this,"JetInputKey","TrigJetRec","Input Jet Collection Key, retrieved from reconstructed jets"};
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roIInputKey {this,"RoIInputKey","InputRoI","RoI input collection key"};
  SG::ReadHandleKey< xAOD::CaloClusterContainer > m_clustersKey { this, "CaloClustersKey", "CaloClusters", "caloclusters in view key" };
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roIOutputKey {this,"RoIOutputKey","TauViewRoIs","Output RoI collection key"};
};
 

#endif

//** ---------------------------------------------------------------------------------------

