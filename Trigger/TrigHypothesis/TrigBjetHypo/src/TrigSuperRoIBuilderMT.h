/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETHYPO_TRIGSUPERROIBUILDER_H
#define TRIGBJETHYPO_TRIGSUPERROIBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

class TrigSuperRoIBuilderMT : public AthAlgorithm {

 public:
  TrigSuperRoIBuilderMT(const std::string&, ISvcLocator*);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  Gaudi::Property< float > m_etaHalfWidth {this,"EtaHalfWidth",0.1,"Eta Half Width"};
  Gaudi::Property< float > m_phiHalfWidth {this,"PhiHalfWidth",0.1,"Phi Half Width"};
  Gaudi::Property< float > m_minJetEt {this,"JetMinEt",30.0,"Jet Min Et"};
  Gaudi::Property< float > m_maxJetEta {this,"JetMaxEta",2.6,"Jet Max Eta : 2.5 + Eta Half Width"};

  SG::ReadHandleKey< xAOD::JetContainer > m_jetInputKey {this,"InputJets","Unspecified","Input Jet Collection Key, retrieved from reconstructed jets"};
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roIOutputKey {this,"OutputRoIs","Unspecified","Output RoI Collection Key"};
};

#endif
