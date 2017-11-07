/*                 
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

  In progress header for TrigMuonEFTagandProbe algorithm
  JJamieson, University of Glasgow, 09/10/2017, Edit:07/11/2017                                                                                 
*/

#ifndef TRIGMUONEFTAGANDPROBE_TRIGMUONEFTAGANDPROBE_H
#define TRIGMUONEFTAGANDPROBE_TRIGMUONEFTAGANDPROBE_H

#include <string>
#include <vector>
#include <map>

//Will almost definitely need these

#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigT1Interfaces/RecMuonRoI.h"



class TriggerElement;

class TrigMuonEFTagandProbe: public virtual HLT::FexAlgo { //TaP algorithm will inherit from HLT::FexAlgo
 public:

  //constructor has to have same inputs as HLT::FexAlgo constructor, these are algorithm name and Service locator
  TrigMuonEFTagandProbe (const std::string &name, ISvcLocator *pSvcLocator); 

  virtual ~TrigMuonEFTagandProbe(); //destructor

  virtual HLT::ErrorCode hltInitialize();
  virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*); //Execute takes in two TEs
  //  virtual HLT::ErrorCode hltEndEvent();
  virtual HLT::ErrorCode hltFinalize();

 private:

  void trim_container(std::vector<const xAOD::Muon*>& good_muons);

  std::vector<const xAOD::Muon*> m_good_muons;
  std::vector<float> m_dimuon_invmass;
  std::vector<float> m_delta_r;

  const DataVector<LVL1::RecMuonRoI>* m_l1_muon_RoIs; //To access Storegate need to use DataVector pointer which always assumes pointer elements

  //std::vector<const LVL1::RecMuonRoI*> m_l1_muon_RoIs; //Keep in case we need a container of good RoIs
  
  bool m_debug; //Flag for conditional DEBUG output
  bool m_verbose;

};

#endif
