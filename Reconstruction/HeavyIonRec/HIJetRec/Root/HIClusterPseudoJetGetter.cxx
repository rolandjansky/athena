/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIClusterPseudoJetGetter.h"

HIClusterPseudoJetGetter::HIClusterPseudoJetGetter(const std::string &name) : PseudoJetGetter(name)
{
  
}

int HIClusterPseudoJetGetter::appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const 
{ 
  bool doGhosts=bool(m_ghostscale)&!m_negEnergyAsGhosts;
  ATH_MSG_DEBUG("Entering appendTo(PseudoJetVector)...");

  if ( evtStore()->contains<xAOD::IParticleContainer>(m_incoll) ) 
  {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_incoll << ", ghost scale="
                  << m_ghostscale  <<  ", isGhost=" << doGhosts);
    const xAOD::IParticleContainer* ppars = 0;
    ppars = evtStore()->retrieve<const xAOD::IParticleContainer>(m_incoll);
    if ( ppars != 0 ) 
    {
      //for now direct copy/paste of PseudoJetGetter::append method with local mods
      //explicitly using IParticle instead of templated version
      const xAOD::IParticleContainer& inputs=*ppars;
      int nskip = 0;
      jet::IConstituentUserInfo::Index labidx = 0;
      if ( pli != 0 ) labidx = pli->index(m_label);
      else ATH_MSG_WARNING("Index-to-label map is not supplied.");
      if ( doGhosts ) labidx = -labidx;
      ATH_MSG_DEBUG( "Ghost scale =  " << m_ghostscale << "; idx = " << labidx );
      
      /// Loop over input, buid CUI and PseudoJets
      for ( auto iinp=inputs.begin(); iinp!=inputs.end(); ++iinp ) 
      {
	auto ppar = *iinp; // IParticle pointer
	if ( ppar == 0 || (m_skipNegativeEnergy && ppar->e() <= 0.0) ) 
	{
	  if ( ppar == 0 ) ATH_MSG_DEBUG("NUll object!");
	  else ATH_MSG_VERBOSE("Skipping cluster with E = " << ppar->e());
	  ++nskip;
	  continue;
	}
	// Take momentum from IParticle.
	fastjet::PseudoJet psj(ppar->p4());
	// Form EM scale, use uncalibrated four-vector.
	if ( m_emtopo ) 
	{
	  const xAOD::CaloCluster* pclu = dynamic_cast<const xAOD::CaloCluster*>(ppar);
	  if ( pclu == 0 ) {
	    ATH_MSG_WARNING("EM particle is not type CaloCluster");
	    continue;
	  }
	  psj.reset(pclu->p4(xAOD::CaloCluster::UNCALIBRATED));
	}
	if(m_negEnergyAsGhosts)
	{
	  if(ppar->e() <=0.)
	  {
	    psj.reset_momentum(psj.px(), psj.py(), psj.pz(), std::abs(ppar->e()));
	    psj*=m_ghostscale;
	  }
	}
	else if ( doGhosts ) psj *= m_ghostscale;

	bool show = psjs.size() < 20;
	if ( show ) ATH_MSG_VERBOSE("index/label: " << labidx << "/" << m_label);
	if ( show ) ATH_MSG_VERBOSE("old/p4/new"
				    << " pt: "   << ppar->pt()  << "/" << ppar->p4().Pt()  << "/" << psj.pt()
				    << ", pz: /"                       << ppar->p4().Pz()  << "/" << psj.pz()
				    << ", p: /"                        << ppar->p4().P()   << "/"
				    << ", E: "   << ppar->e()   << "/" << ppar->p4().E()   << "/" << psj.e()
				    << ", m: "   << ppar->m()   << "/" << ppar->p4().M()   << "/" << psj.m()
				    << ", eta: " << ppar->eta() << "/" << ppar->p4().Eta() << "/" << psj.eta()
				    );
	if ( pli != 0 ) 
	{
	  jet::IConstituentUserInfo* pcui = this->buildCUI(ppar, labidx, pli);
	  psj.set_user_info(pcui);
	  if ( show ) ATH_MSG_VERBOSE("User info particle: " << pcui->particle());
	}
	psjs.push_back(psj);
      } // end loop over input 
      if ( nskip ) ATH_MSG_DEBUG("Skipped constituent count: " << nskip);
      ATH_MSG_DEBUG("After append, PseudoJet count is " << psjs.size());
      return 0;
    }
  }
  ATH_MSG_ERROR("Unable to find input collection: " << m_incoll);
  return 1;
}
