/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthHSDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Patrick McCormack (wpmccormack@cern.ch)
//

#include "DerivationFrameworkMCTruth/TruthHSDecorator.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  TruthHSDecorator::TruthHSDecorator(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty ("TruthParticleKey", m_truthParticleKey = "TruthParticles", "Name of truth particle container key for input");
    declareProperty ("TruthEventKey", m_truthEventKey = "TruthEvents", "Name of truth event container key for input");
    declareProperty ("DecorationName", m_decorationName = "HSBool", "Decoration Name");

  }

  StatusCode TruthHSDecorator::addBranches() const
  {

      // Set up the decorators 
      SG::AuxElement::Decorator< short > decoratorHS(m_decorationName); 

      const xAOD::TruthParticleContainer* xTruthParticleContainer = 0;
      if (evtStore()->retrieve(xTruthParticleContainer,m_truthParticleKey).isFailure()) {
	ATH_MSG_ERROR("could not retrieve TruthParticleContainer " <<m_truthParticleKey);
	return StatusCode::FAILURE;
      }

      //Initialize all particles to False before looping over the HS particles
      for(xAOD::TruthParticleContainer::const_iterator PItr = xTruthParticleContainer->begin(); PItr!=xTruthParticleContainer->end(); ++PItr){
	
	decoratorHS(**PItr) = 0;

      }

      //Get TruthEvents
      const xAOD::TruthEventContainer* xTruthEventContainer = 0;
      if (evtStore()->retrieve(xTruthEventContainer,m_truthEventKey).isFailure()) {
	ATH_MSG_ERROR("could not retrieve TruthEventContainer " <<m_truthEventKey);
	return StatusCode::FAILURE;
      }

      //Now loop over HS particles
      for ( const auto* truthevent : *xTruthEventContainer ) {

	typedef std::vector<ElementLink< xAOD::TruthParticleContainer >> Vec_EL_TPC;

        if( ! (truthevent)->isAvailable< Vec_EL_TPC > ("truthParticleLinks") ) continue;

        //const Vec_EL_TPC & truthPartLinks = (truthevent)->auxdata< Vec_EL_TPC >("truthParticleLinks");
        const Vec_EL_TPC & truthPartLinks = (truthevent)->truthParticleLinks();
        for(unsigned int tpl = 0; tpl < truthPartLinks.size(); tpl++){
	  if(!truthPartLinks.at(tpl).isValid()) continue;
	  const auto  pp = *(truthPartLinks.at(tpl));
	  pp->auxdecor<short>("HSBool") = 1;
	}

      }

      return StatusCode::SUCCESS;
  }
}
