/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrigBjetRoiBuilderMT.h        
//
#ifndef  TRIGROI_FROMJETS_MT_H
#define  TRIGROI_FROMJETS_MT_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

class TrigRoIFromJetsMT : public AthAlgorithm {

 public:
  TrigRoIFromJetsMT(const std::string&, ISvcLocator*);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  Gaudi::Property< float > m_etaHalfWidth {this,"EtaHalfWidth",0.1,"Eta Half Width"};
  Gaudi::Property< float > m_phiHalfWidth {this,"PhiHalfWidth",0.1,"Phi Half Width"};
  Gaudi::Property< float > m_minJetEt {this,"JetMinEt",30.0,"Jet Min Et"};
  Gaudi::Property< float > m_maxJetEta {this,"JetMaxEta",2.6,"Jet Max Eta : 2.5 + Eta Half Width"};
  Gaudi::Property< int >   m_nJetsMax {this,"NJetsMax",-1,"Option to limit the number of jets that form the super RoI  "};
  Gaudi::Property< bool >  m_dynamicMinJetEt {this,"DynamicMinJetEt",false,"if (X > -1 && nJets > X) minJetEt = m_minJetEt + (nJets-X)*Y  "};
  Gaudi::Property< int >   m_dynamicNJetsMax {this,"DynamicNJetsMax",9999,"variable X above"};
  Gaudi::Property< float > m_dynamicEtFactor {this,"DynamicEtFactor",0,"variable Y above "};

  SG::ReadHandleKey< xAOD::JetContainer > m_jetInputKey {this,"JetInputKey","TrigJetRec","Input Jet Collection Key, retrieved from reconstructed jets"};
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roIOutputKey {this,"RoIOutputKey","EMViewRoIs","Output RoI Collection Key"};
};

#endif
