// emacs: this is -*- c++ -*-
//
//   @file    TrigSuperRoiBuilderAllTE.h        
//
//                   
//  
//   Copyright (C) 2014 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigSuperRoiBuilderAllTE.h 623211 2014-10-22 09:35:01Z kleney $


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

  double m_minJetEt; // is this needed still?

};
 

#endif


