/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigGenericAlgs_AcceptWhenInputPresent_h
#define TrigGenericAlgs_AcceptWhenInputPresent_h

#include <string>
#include "TrigInterfaces/AllTEAlgo.h"

namespace HLT {
  class TriggerElement;
}

/**
 * @brief Algorithm creating TE when all TEs specified in the input are present with desired multiplicity
 * The TEs created are "made out of blue" or span all the inputs according to the config.
 * The configuration of required multiplicty has to match seeding.
 **/

class AcceptWhenInputPresent : public HLT::AllTEAlgo {
 public:
  AcceptWhenInputPresent(const std::string& name, ISvcLocator* pSvcLocator); 
  HLT::ErrorCode hltInitialize() { return HLT::OK; } 
  HLT::ErrorCode hltFinalize()   { return HLT::OK; }
  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
			    unsigned int output);
 private:
  std::vector<int> m_requiredMultiplicity;
  bool m_seedFromInputs;
};


#endif 
