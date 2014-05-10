/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigGenericAlgs_PrescaleAlgo_h
#define TrigGenericAlgs_PrescaleAlgo_h

#include <string>
#include <vector>

#include "TrigInterfaces/AllTEAlgo.h"


namespace HLT {
  class TriggerElement;
}

namespace CLHEP {
  class HepRandomEngine;
}

/**
 * @class All TE algorithm which mimics prescaling.
 * When the algorithm is executed it takes all inputs and creates 
 * output TEs for all of them with probablity set as it's property.
 * @warning The prescaling is coherent for all inputs. 
 * @warning The algorithm is not cached. That means all sequences
 *          using it can share the same instance.
 * @warning Only one TE type can be given as input.
 *
 * The convention used for the scaler is identical as for HLT chains.
 * That is: <=0 means infinite 1 -> no scaling, X -> 1/X events to be passed.
 * 
 **/

class PrescaleAlgo: public HLT::AllTEAlgo {
public:
  PrescaleAlgo(const std::string& name, ISvcLocator* pSvcLocator); 
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize()   { return HLT::OK; } 
  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
			    unsigned int output);


private: 
  CLHEP::HepRandomEngine* m_engine; //!< std CLHEP random engine
  float m_scaler;
};


#endif
