/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigEFHadronicRecoil_h__
#define __TrigEFHadronicRecoil_h__
/*
  TrigEFHadronicRecoil.h
*/
#include <string>
#include <vector>
#include "TrigInterfaces/AllTEAlgo.h"

class TrigEFHadronicRecoil : public HLT::AllTEAlgo {
public:
  TrigEFHadronicRecoil(const std::string& name, ISvcLocator* svcloc);
  ~TrigEFHadronicRecoil();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<HLT::TEVec>& inputs, 
			    unsigned int type_out);
  HLT::ErrorCode hltFinalize();

  HLT::TriggerElement* makeOutputTE(std::vector<HLT::TEVec>& inputs, 
				    unsigned int output);

private:
  float mRecoilCut;

};

#endif // __TrigEFHadronicRecoil_h__
