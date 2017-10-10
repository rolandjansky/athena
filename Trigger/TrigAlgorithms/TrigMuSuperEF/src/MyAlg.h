/*                 
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

  Empty skeleton header for TBC_EF_muon_tag_and_probe_algorithm
  JJamieson, University of Glasgow, 09/10/2017                                                                                 
*/


#include <string>
#include <vector>
#include <map>

//Will almost definitely need these

#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/FexAlgo.h"

//Think these are needed for HLT::FexAlgo inheritance

class MyAlg: public virtual HLT::FexAlgo { //MyAlg will inherit from HLT::FexAlgo
 public:

  //constructor has to have same inputs as HLT::FexAlgo constructor, these are algorithm name and Service locator
  MyAlg (const std::string &name, ISvcLocator *pSvcLocator); 

  virtual ~MyAlg(); //destructor

  virtual HLT::ErrorCode hltInitialize();
  virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*); //Execute takes in two TEs
  //  virtual HLT::ErrorCode hltEndEvent();
  virtual HLT::ErrorCode hltFinalize();

  // Private:

};


