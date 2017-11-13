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
  declareProperty("Alpha", m_alpha=0.);
  declareProperty("MaxEta", m_maxEta=4.5);
  declareProperty("MaxRapForRhoComputation", m_maxRapForRhoComputation=2.0);
  declareProperty("GhostArea", m_ghost_area=0.01);
  declareProperty("CommonBGEForRhoAndRhom",m_common_bge_for_rho_and_rhom=false);
}

StatusCode ConstituentSubtractorTool::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);   

  return StatusCode::FAILURE;
}

// Apply PU weighting and decorate the CaloCluster container appropriately:
	
StatusCode ConstituentSubtractorTool::process(xAOD::CaloClusterContainer* cont) const {


  contrib::ConstituentSubtractor subtractor;
  
  // free parameter for the maximal allowed distance sqrt((y_i-y_k)^2+(phi_i-phi_k)^2) between particle i and ghost k
  subtractor.set_max_standardDeltaR(m_maxDeltaR); 

  // free parameter for the distance measure (the exponent of particle pt). The larger the parameter alpha, the more are favoured the lower pt particles in the subtraction process
  subtractor.set_alpha(m_alpha);  

  // free parameter for the density of ghosts. The smaller, the better - but also the computation is slower.
  subtractor.set_ghost_area(m_ghost_area); 

  // prepare PseudoJet inputs 
  std::vector<PseudoJet>  inputs_to_correct, inputs_to_not_correct;
  inputs_to_correct.reserve(cont->size());
  int i=0;
  for (xAOD::CaloCluster * part: *cont){
    if (part->e() > 0 ) {
      PseudoJet pj(part->p4());
      pj.set_user_index(i);
      if (fabs(pj.eta()) <= m_maxEta) inputs_to_correct.push_back(pj);
      else inputs_to_not_correct.push_back(pj);
    }
    i++;
  }

  // create what we need for the background estimation
  //----------------------------------------------------------
  // maximal rapidity is used (not pseudo-rapidity). Since the inputs are massless, it does not matter
  GridMedianBackgroundEstimator bge_rho(m_maxRapForRhoComputation, 0.5);
  bge_rho.set_particles(inputs_to_correct);
  subtractor.set_background_estimator(&bge_rho);

  // this sets the same background estimator to be used for deltaMass density, rho_m, as for pt density, rho:
  subtractor.set_common_bge_for_rho_and_rhom(m_common_bge_for_rho_and_rhom); 
  // for massless input particles it does not make any difference (rho_m is always zero)

  std::vector<PseudoJet> corrected_event=subtractor.subtract_event(inputs_to_correct,m_maxEta);

  
  // Now reset the clusters 4-vec to 0
  for (xAOD::CaloCluster * cl: *cont){
    cl->setE(0);
  }  
  // Revive the clusters from corrected_event (some clusters are removed during the correction procedure)
  for (PseudoJet & pj : corrected_event) {    
    xAOD::CaloCluster * cl = (*cont)[pj.user_index()];
    cl->setE(pj.e());
    cl->setPhi(pj.phi());
    cl->setEta(pj.eta());
  }
  // Revive the clusters from inputs_to_not_correct
  for (PseudoJet & pj : inputs_to_not_correct) {    
    xAOD::CaloCluster * cl = (*cont)[pj.user_index()];
    cl->setE(pj.e());
    cl->setPhi(pj.phi());
    cl->setEta(pj.eta());
  }

  return StatusCode::SUCCESS;
}

