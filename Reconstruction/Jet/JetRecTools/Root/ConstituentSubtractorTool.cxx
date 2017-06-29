/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source code for the ConstituentSubtractorTool implementation class
//
//
#include "JetRecTools/ConstituentSubtractorTool.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/contrib/ConstituentSubtractor.hh"
#include "fastjet/tools/GridMedianBackgroundEstimator.hh"
#include "fastjet/Selector.hh"

using namespace fastjet;
ConstituentSubtractorTool::ConstituentSubtractorTool(const std::string & name): JetConstituentModifierBase(name) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("MaxDeltaR", m_maxDeltaR=0.25);
  declareProperty("Alpha", m_alpha=1.);
  declareProperty("MaxEta", m_maxEta=10.);
  declareProperty("MaxRapForRhoComputation", m_maxRapForRhoComputation=2.0);
}

// Apply PU weighting and decorate the IParticle container appropriately:
	
StatusCode ConstituentSubtractorTool::process_impl(xAOD::IParticleContainer* cont) const {

  contrib::ConstituentSubtractor subtractor;
  subtractor.set_max_standardDeltaR(m_maxDeltaR); // free parameter for the maximal allowed distance sqrt((y_i-y_k)^2+(phi_i-phi_k)^2) between particle i and ghost k
  subtractor.set_alpha(m_alpha);  // free parameter for the distance measure (the exponent of particle pt). The larger the parameter alpha, the more are favoured the lower pt particles in the subtraction process
  subtractor.set_ghost_area(0.01); // free parameter for the density of ghosts. The smaller, the better - but also the computation is slower.

  // prepare PseudoJet input
  std::vector<PseudoJet> inputs_to_correct, inputs_to_not_correct;
  inputs_to_correct.reserve(cont->size());
  inputs_to_not_correct.reserve(cont->size());
  size_t i =0; // Corresponds to the index in the input container
  // We don't use part->index() because it might be a view container
  // combining more than one owning container
  for(xAOD::IParticle * part: *cont){
    // Only use positive E
    bool accept = part->e() > -1*FLT_MIN;
    // For PFlow we would only want to apply the correction to neutral PFOs,
    // because charged hadron subtraction handles the charged PFOs.
    // However, we might still want to use the cPFOs for the min pt calculation
    if(m_inputType==xAOD::Type::ParticleFlow && m_ignoreChargedPFOs) {
      xAOD::PFO* pfo = static_cast<xAOD::PFO*>(part);
      accept &= fabs(pfo->charge())<FLT_MIN;
    }
    // Reject object if outside maximum eta range
    accept &= fabs(part->eta()) <= m_maxEta;

    PseudoJet pj( part->p4() );
    pj.set_user_index( i );
    if(accept) {
      ATH_MSG_VERBOSE("Using " << part->type() << " with pt " << part->pt());
      inputs_to_correct.push_back(pj);
    } else {
      ATH_MSG_VERBOSE("Will not correct " << part->type() << " with pt " << part->pt());
      inputs_to_not_correct.push_back(pj);
    }

    ++i;
  }

  // create what we need for the background estimation
  //----------------------------------------------------------
  GridMedianBackgroundEstimator bge_rho(m_maxRapForRhoComputation, 0.5); // maximal rapidity is used (not pseudo-rapidity). Since the inputs are massless, it does not matter
  bge_rho.set_particles(inputs_to_correct);
  subtractor.set_background_estimator(&bge_rho);

  // this sets the same background estimator to be used for deltaMass density, rho_m, as for pt density, rho:
  subtractor.set_common_bge_for_rho_and_rhom(false); // for massless input particles it does not make any difference (rho_m is always zero)

  ATH_MSG_DEBUG("Subtracting event density from constituents");
  std::vector<PseudoJet> corrected_event=subtractor.subtract_event(inputs_to_correct,m_maxEta);

  // Define a vector holding the corrected four-momenta for all output constituents
  // This is defaulted to zero, because fastjet will only return non-zero pseudojets
  std::vector<xAOD::JetFourMom_t> corrected_p4s(cont->size(),xAOD::JetFourMom_t(0.,0.,0.,0.));
  // Set the corrected four-vectors
  for(PseudoJet & pj : corrected_event) {
    ATH_MSG_VERBOSE("Setting four-mom for constituent " << pj.user_index() << ", pt = " << pj.pt());
    corrected_p4s[pj.user_index()].SetCoordinates(pj.pt(),pj.eta(),pj.phi(),pj.m());
  }
  for(PseudoJet & pj : inputs_to_not_correct) {
    ATH_MSG_VERBOSE("Setting four-mom for constituent " << pj.user_index() << ", pt = " << pj.pt());
    corrected_p4s[pj.user_index()].SetCoordinates(pj.pt(),pj.eta(),pj.phi(),pj.m());
  }

  // Set every constituent's four-vector in the output container
  i = 0; // Again, we need to track the input container index, not the owning container index
  const static SG::AuxElement::Accessor<float> weightAcc("CSWeight"); // Handle for PU weighting here
  for(xAOD::IParticle * part: *cont){
    ATH_MSG_VERBOSE("Now on constituent " << i);
    ATH_MSG_VERBOSE("Initial pt: " << part->pt() << ", subtracted pt: " << corrected_p4s[i].Pt());
    ATH_MSG_VERBOSE("Initial eta: " << part->eta() << ", subtracted pt: " << corrected_p4s[i].Eta());
    ATH_MSG_VERBOSE("Initial phi: " << part->phi() << ", subtracted pt: " << corrected_p4s[i].Phi());
    ATH_CHECK( setP4(part,corrected_p4s[i], &weightAcc) );
    ++i;
  }  

  return StatusCode::SUCCESS;
}

