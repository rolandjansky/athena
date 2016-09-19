/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "boost/foreach.hpp"
#include "TrigNavigation/TriggerElement.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"



#include "TrigGenericAlgs/PrescaleAlgo.h"
PrescaleAlgo::PrescaleAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_engine(0)
{
   declareProperty("Prescale", m_scaler = 0, "Prescaler value, default is 0 which means no output is created.");
  
}
HLT::ErrorCode PrescaleAlgo::hltInitialize() {
  // copy/paste from steering random scaler
  IAtRndmGenSvc*  p_AtRndmGenSvc(0);

  StatusCode rndmStatus = service("AtRanluxGenSvc", p_AtRndmGenSvc, true);
  
  if ( !rndmStatus.isSuccess() || 0 == p_AtRndmGenSvc ) {
    msg() << MSG::ERROR << " Could not initialize Random Number Service" << endmsg;
    return HLT::ERROR;
  }

  p_AtRndmGenSvc->CreateStream(1857585203,  1857585203, name());  // this random engine only needs one seed
  m_engine = p_AtRndmGenSvc->GetEngine(name());
  return HLT::OK;
}

HLT::ErrorCode PrescaleAlgo::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
			  unsigned int output) {
   beforeExecMonitors().ignore();
  // check missonfiguration
  if ( input.size() != 1 ) {
    msg() << MSG::ERROR << "Missconfiguration of the " << name() << "  to many/to few input types: "
	  << input.size() << " while expecting only 1" << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  
  std::vector<HLT::TriggerElement*>& in = input[0];
  if ( m_scaler >= 1. ) {
    bool decision = false;

    if ( m_scaler == 1. ) {
      decision = true;
    } else {
      double rand = CLHEP::RandFlat::shoot(m_engine, 0., 1.); // pick random number between 0 and 1
      if (1./m_scaler > rand) 
	decision = true;
    }

    if ( decision ) {
      BOOST_FOREACH( HLT::TriggerElement* te, in) {
	config()->getNavigation()->addNode(te, output);
      }
    }
  } 

  afterExecMonitors().ignore();
  return HLT::OK;
}
