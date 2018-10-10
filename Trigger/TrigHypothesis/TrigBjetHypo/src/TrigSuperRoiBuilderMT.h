// emacs: this is -*- c++ -*-
//
//   @file    TrigSuperRoiBuilderAllTE.h        
//
//                   
//  
//   Copyright (C) 2014 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigSuperRoiBuilderAllTE.h 797477 2017-02-16 09:56:10Z kleney $


#ifndef  TRIGSUPERROIBUILDER_MT_H
#define  TRIGSUPERROIBUILDER_MT_H

#include <iostream>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class TrigSuperRoiBuilderMT : public AthAlgorithm {

 public:
  TrigSuperRoiBuilderMT(const std::string&, ISvcLocator*);
  ~TrigSuperRoiBuilderMT();
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:
  Gaudi::Property< float > m_etaHalfWidth {this,"EtaHalfWidth",0.1,"Eta Half Width"};
  Gaudi::Property< float > m_phiHalfWidth {this,"PhiHalfWidth",0.1,"Phi Half Width"};
  Gaudi::Property< float > m_minJetEt {this,"JetMinEt",30.0,"Jet Min Et"};
  Gaudi::Property< float > m_maxJetEta {this,"JetMaxEta",2.6,"Jet Max Eta : 2.5 + Eta Half Width"};
  Gaudi::Property< int >   m_nJetsMax {this,"NJetsMax",-1,"Option to limit the number of jets that form the super RoI  "};
  Gaudi::Property< bool >  m_dynamicMinJetEt {this,"DynamicMinJetEt",false,"if (X > -1 && nJets > X) minJetEt = m_minJetEt + (nJets-X)*Y  "};
  Gaudi::Property< int >   m_dynamicNJetsMax {this,"DynamicNJetsMax",9999,"variable X above"};
  Gaudi::Property< float > m_dynamicEtFactor {this,"DynamicEtFactor",0,"variable Y above "};

  SG::ReadHandleKey< xAOD::JetContainer > m_jetInputKey {this,"JetInputKey","TrigJetRec","Input Jet Collection Key"};
  SG::WriteHandleKey< xAOD::JetContainer > m_jetOutputKey {this,"JetOutputKey","SuperRoi","Output Jet Collection Key"};
  SG::WriteHandleKey< TrigRoiDescriptor > m_superRoIOutputKey {this,"SuperRoIOutputKey","SuperRoi","Output Jet Collection Key"};
};
 

#endif

//** ---------------------------------------------------------------------------------------

