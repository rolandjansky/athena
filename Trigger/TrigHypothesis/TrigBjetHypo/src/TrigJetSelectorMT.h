/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigJetSplitterMT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Carlo Varni
// EMAIL:    carlo.varni@ge.infn.it
//
// ************************************************

#ifndef TRIGJETSELECTOR_MT_H
#define TRIGJETSELECTOR_MT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

class TrigJetSelectorMT : public AthAlgorithm {
 public:
  TrigJetSelectorMT(const std::string&, ISvcLocator*);

  StatusCode initialize();
  StatusCode execute();

 private:
  const xAOD::Vertex* getPrimaryVertex( const xAOD::VertexContainer* ) const;

 private:
  Gaudi::Property< double > m_minJetEt {this,"JetMinEt",15,"Minimum of Output Jet Et in GeV"};
  Gaudi::Property< double > m_maxJetEta {this,"JetMaxEta",3.2,"Maximum eta acceptance of output Jet (ID acceptance)"};

  Gaudi::Property< double > m_etaHalfWidth {this,"EtaHalfWidth",0.4,"Eta Half Width"};
  Gaudi::Property< double > m_phiHalfWidth {this,"PhiHalfWidth",0.4,"Phi Half Width"};
  Gaudi::Property< double > m_zHalfWidth {this,"ZHalfWidth",10.0,"Z Half Width in mm"};

  //=========== Handles                                                                                                                                                                      
  SG::ReadHandleKey< xAOD::JetContainer > m_inputJetsKey {this,"InputJets","Undefined","Input Jet Container Key"};
  SG::ReadHandleKey< xAOD::VertexContainer > m_inputVertexKey {this,"InputVertex","Undefined","Input Vertex Key"};

  SG::WriteHandleKey< xAOD::JetContainer > m_outputJetKey {this,"OutputJets","Undefined","Output Jet Container Key"};
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_outputRoiKey {this,"OutputRoi","Undefined","Output RoI Container Key"};
};

#endif
