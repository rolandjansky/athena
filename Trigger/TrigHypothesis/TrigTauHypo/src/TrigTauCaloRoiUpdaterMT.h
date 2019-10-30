// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrigRoiBuilderMT.h        
//


#ifndef  TrigTauCaloRoiUpdaterMT_H
#define  TrigTauCaloRoiUpdaterMT_H

#include <iostream>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
//#include "xAODJet/JetContainer.h"
//#include "xAODJet/JetAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class TrigTauCaloRoiUpdaterMT : public AthAlgorithm {

 public:
  TrigTauCaloRoiUpdaterMT(const std::string&, ISvcLocator*);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  Gaudi::Property< float > m_dRForCenter {this,"dRForCenter",0.2,"Delta R from the center of ROI"};

  //SG::ReadHandleKey< xAOD::JetContainer > m_jetInputKey {this,"JetInputKey","TrigJetRec","Input Jet Collection Key, retrieved from reconstructed jets"};
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roIInputKey {this,"RoIInputKey","Undefined",""};
  SG::ReadHandleKey< xAOD::CaloClusterContainer > m_clustersKey { this, "CaloClustersKey", "Undefined", "caloclusters in view" };
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roIOutputKey {this,"RoIOutputKey","TauViewRoIs","Output RoI Collection Key"};
};
 

#endif

//** ---------------------------------------------------------------------------------------

