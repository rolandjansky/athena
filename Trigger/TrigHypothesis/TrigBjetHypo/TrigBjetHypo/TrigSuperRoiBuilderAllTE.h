// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrigSuperRoiBuilderAllTE.h        
//
//                   
//  
//
//   $Id: TrigSuperRoiBuilderAllTE.h 797477 2017-02-16 09:56:10Z kleney $


#ifndef  TRIGSUPERROIBUILDERALLTE_H
#define  TRIGSUPERROIBUILDERALLTE_H

#include <iostream>

#include "TrigInterfaces/AllTEAlgo.h"


class TrigSuperRoiBuilderAllTE : public HLT::AllTEAlgo {

 public:
  TrigSuperRoiBuilderAllTE(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigSuperRoiBuilderAllTE();
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  HLT::ErrorCode hltFinalize();

 private:

  std::string m_jetInputKey;
  std::string m_jetOutputKey;

  double m_etaHalfWidth;
  double m_phiHalfWidth;

  double m_minJetEt; 
  double m_maxJetEta;

  int m_nJetsMax;        
  bool m_dynamicMinJetEt;  
  int m_dynamicNJetsMax;  
  double m_dynamicEtFactor;  

};
 

#endif


