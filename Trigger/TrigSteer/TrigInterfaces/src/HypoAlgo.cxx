/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigPassFlags.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "xAODTrigger/TrigPassBits.h"
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/Navigation.h"

using namespace HLT;

HypoAlgo::HypoAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : Algo(name, pSvcLocator) {
  declareProperty("forceAccept", m_forceAccept=false, "Forces the decision of hypo to be always 'passed'. Algorithm is run though.");
}

ErrorCode HypoAlgo::plugin_processTE(TEConstVec& /*inputTEs*/, TriggerElement* outputTE)
{
  //  if (inputTEs.size() != 1) return HLT::BAD_ALGO_CONFIG; // this is problematic in case of hypo run in topo sequence

  bool pass = false;

  ErrorCode ec = hltExecute(outputTE, pass);

  //TB  if (m_returnErrorCode.action() > HLT::Action::CONTINUE ) return m_returnErrorCode; //??? strange

  outputTE->setActiveState(pass || m_forceAccept);

  return ec;
}



HLT::ErrorCode HypoAlgo::createAndProcessTEs(std::vector<unsigned int>& inputTypes, 
					     unsigned int outputType,
					     TEVec& outputTEs, 
					     const std::vector<unsigned int>& /*topologicalStartFrom*/) {
  if (inputTypes.size() != 1 ) { // missconfiguration
    ATH_MSG_ERROR ( "Hypo algo should never be placed first in sequences with more than one input TE!" );
    return m_returnErrorCode = HLT::BAD_JOB_SETUP;
  }
  TEVec inTEs;
  m_config->getNavigation()->getAllOfType(inputTypes[0], inTEs);
  
  TEVec::iterator input_it;
  for (  input_it = inTEs.begin(); input_it != inTEs.end(); ++input_it ) {
    // create output
    outputTEs.push_back(m_config->getNavigation()->addNode(*input_it, outputType));
  }
  ATH_MSG_DEBUG ( "Pre-created input TEs (size: " << outputTEs.size() << "). Now will run regular processing on them." );
  return processTEs(outputTEs);
}


HLT::ErrorCode HypoAlgo::attachBits(const TriggerElement* outputTE, const TrigPassBits* bits, const std::string& label) {
  std::string key;
  if (!bits) {
    return HLT::FATAL;
  }
  
  {
    const HLT::ErrorCode ec = attachFeature(outputTE, bits, label);
    if ( ec != HLT::OK ) return ec;
  }  

  xAOD::TrigPassBits* xbits = new xAOD::TrigPassBits();
  xbits->makePrivateStore();
  xbits->setSize(bits->size());
    
  if( bits->size() == 0 ) {
    xbits->setPassBits( std::vector< uint32_t >() );
  } else {

    xbits->setPassBits( std::vector< uint32_t >( ( ( bits->size() - 1 ) / 32 ) + 1 ) );
  }

  for ( size_t b = 0, bmax = bits->size(); b < bmax; ++b ) {
    xbits->markPassing(b, bits->isPassing(b));
  }

  return attachFeature(outputTE, xbits, label);
}

HLT::ErrorCode HypoAlgo::attachFlags(const TriggerElement* outputTE, const TrigPassFlags* flags, const std::string& label) {
  std::string key;
  if (!flags) {
    return HLT::FATAL;
  }
  return attachFeature(outputTE, flags, label);
}
