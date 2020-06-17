/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostAfterburner/BoostEvent.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "GeneratorObjects/McEventCollection.h"
#include <iomanip>

BoostEvent::BoostEvent(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator)
{
   declareProperty("McInputKey",     m_inkey="GEN_EVENT");
   declareProperty("McOutputKey",      m_outkey="BOOSTED_EVENT");
   declareProperty("BetaX",      m_beta_x=0);
   declareProperty("BetaY",      m_beta_y=0);
   declareProperty("BetaZ",      m_beta_z=0);
}

StatusCode BoostEvent::initialize()
{
  msg(MSG::DEBUG) << "Properties:\n"
		  << std::setw(60) << "McInputKey "  << m_inkey << "\n"
		  << std::setw(60) << "McOutputKey " << m_outkey << "\n"
		  << std::setw(60) << "BetaX " << m_beta_x << "\n"
		  << std::setw(60) << "BetaY " << m_beta_y << "\n"
		  << std::setw(60) << "BetaZ " << m_beta_z << "\n"
		  << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode BoostEvent::execute()
{
  const McEventCollection* input_collection;
  if ( evtStore()->retrieve(input_collection, m_inkey).isFailure() )
  {
    msg(MSG::ERROR) << "Could not retrieve truth McEventCollection " << m_inkey << endmsg;
    return StatusCode::FAILURE;
  }

  // Loop over all events in original McEventCollection and
  // Copy to a new (modifiable) collection 
  McEventCollection* output_collection = new  McEventCollection();
  for (McEventCollection::const_iterator citr = input_collection->begin(); citr!=input_collection->end(); citr++)
  {
    output_collection->push_back(new HepMC::GenEvent(*(*citr)));
  }

  //now loop on particles
  msg(MSG::VERBOSE) << "Boosting event with beta=(" << m_beta_x << "," << m_beta_y << "," << m_beta_z << ")" << endmsg;
  msg(MSG::VERBOSE) << std::setw(10) << "pt"
		    << std::setw(10) << "E"
		    << std::setw(10) << "eta"
		    << std::setw(10) << "phi"
		    << std::setw(10) << "pt'"
		    << std::setw(10) << "E'"
		    << std::setw(10) << "eta'"
		    << std::setw(10) << "phi'"
		    << endmsg;

  for (McEventCollection::iterator itr = output_collection->begin(); itr!=output_collection->end(); itr++)
  {
    for ( auto particle: **itr){
      CLHEP::HepLorentzVector momentum(particle->momentum().px(),
				particle->momentum().py(),
				particle->momentum().pz(),
				particle->momentum().e());

      momentum.boost(m_beta_x,m_beta_y,m_beta_z);
      msg(MSG::VERBOSE) << std::setw(10) << particle->momentum().perp()
			<< std::setw(10) << particle->momentum().e()
			<< std::setw(10) << particle->momentum().eta()
			<< std::setw(10) << particle->momentum().phi()
			<< std::setw(10) << momentum.perp()
			<< std::setw(10) << momentum.e()
			<< std::setw(10) << momentum.eta()
			<< std::setw(10) << momentum.phi()
			<< endmsg;

      particle->set_momentum( HepMC::FourVector(momentum.px(),momentum.py(),momentum.pz(),momentum.e()) );
    }
  }

  if(evtStore()->record(output_collection, m_outkey).isFailure())
  {
    msg(MSG::ERROR) << "Could not record boosted McEventCollection " << m_outkey << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode BoostEvent::finalize()
{
  return StatusCode::SUCCESS;
}
