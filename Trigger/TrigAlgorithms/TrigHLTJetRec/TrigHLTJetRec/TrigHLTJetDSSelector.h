/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetDSSelector.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetDSSelector
//
// AUTHOR:   Caterina Doglioni
// 
// Description: FexAlgo for selecting jets out of the DS collection
// 
// ********************************************************************

#ifndef TRIGHLTJETDSSELECTOR_TRIGHLTJETDSSELECTOR_H
#define TRIGHLTJETDSSELECTOR_TRIGHLTJETDSSELECTOR_H

#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "xAODJet/JetContainer.h"

class TrigHLTJetDSSelector: public HLT::FexAlgo {

 public:
  TrigHLTJetDSSelector(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetDSSelector();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE);
  HLT::ErrorCode hltFinalize();
 
private:

  std::string m_jetCollectionName;
  double m_jetPtThreshold;
  int m_maxNJets;

  HLT::ErrorCode attachJetCollection(HLT::TriggerElement*,
                                     const xAOD::JetContainer*,
                                     std::string);

  //this code is duplicated - but unused in TrigHLTJetRec
  xAOD::JetContainer*  make_empty_jetcontainer(){
    
    //QUESTION: should this be a VIEW container?
    xAOD::JetContainer* j_container = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
    //in case it shouldn't be, just remove SG::VIEW_ELEMENTS and uncomment line below
    //j_container->setStore(new xAOD::JetTrigAuxContainer);
    return j_container;
  }

}; 
#endif
