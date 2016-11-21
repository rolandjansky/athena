/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source code for the ConstituentSubtractorTool implementation class
//
//
#include "JetRecTools/ConstituentSubtractorTool.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/contrib/ConstituentSubtractor.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include "fastjet/Selector.hh"

using namespace fastjet;
ConstituentSubtractorTool::ConstituentSubtractorTool(const std::string & name): JetConstituentModifierBase(name) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("MaxDeltaR", m_maxDeltaR=0.25);
  declareProperty("Alpha", m_alpha=0.);
  declareProperty("MaxEta", m_maxEta=2.5);
}

StatusCode ConstituentSubtractorTool::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);   

  return StatusCode::FAILURE;
}

// Apply PU weighting and decorate the CaloCluster container appropriately:
	
StatusCode ConstituentSubtractorTool::process(xAOD::CaloClusterContainer* cont) const {


  contrib::ConstituentSubtractor subtractor;
  subtractor.set_max_standardDeltaR(m_maxDeltaR); // free parameter for the maximal allowed distance sqrt((y_i-y_k)^2+(phi_i-phi_k)^2) between particle i and ghost k
  subtractor.set_alpha(m_alpha);  // free parameter for the distance measure (the exponent of particle pt). The larger the parameter alpha, the more are favoured the lower pt particles in the subtraction process
  subtractor.set_ghost_area(0.01); // free parameter for the density of ghosts. The smaller, the better - but also the computation is slower.

  // prepare PseudoJet input from 
  std::vector<PseudoJet>  full_event; full_event.reserve( cont->size() );
  int i =0;
  for(xAOD::CaloCluster * part: *cont){
    if(part->e() > 0 ) {
      PseudoJet pj( part->p4() );
      pj.set_user_index( i );
      full_event.push_back(pj);
    }
    i++;
  }


  // keep the particles up to 4 units in rapidity
  full_event = SelectorAbsEtaMax(m_maxEta)(full_event);

  // do the clustering with ghosts and get the jets
  //----------------------------------------------------------
  AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(m_maxEta,1)); // the area definiton is used only for the jet backgroud estimator. It is not important for the ConstituentSubtractor when subtracting the whole event - this is not true when subtracting the individual jets


  // create what we need for the background estimation
  //----------------------------------------------------------
  JetDefinition jet_def_for_rho(kt_algorithm, 0.4);
  Selector rho_range =  SelectorAbsEtaMax(m_maxEta-0.4);

  JetMedianBackgroundEstimator bge_rho(rho_range, jet_def_for_rho, area_def);
  BackgroundJetScalarPtDensity *scalarPtDensity=new BackgroundJetScalarPtDensity();
  bge_rho.set_jet_density_class(scalarPtDensity); // this changes the computation of pt of patches from vector sum to scalar sum. The scalar sum seems more reasonable.
  bge_rho.set_particles(full_event);
  subtractor.set_background_estimator(&bge_rho);

  // this sets the same background estimator to be used for deltaMass density, rho_m, as for pt density, rho:
  subtractor.set_common_bge_for_rho_and_rhom(false); // for massless input particles it does not make any difference (rho_m is always zero)

  std::vector<PseudoJet> corrected_event=subtractor.subtract_event(full_event,m_maxEta);
  
  // Now reset the clusters 4-vec to 0
  for(xAOD::CaloCluster * cl: *cont){
    cl->setE(0);
  }  
  // Revive only the clusters from corrected_event
  for(PseudoJet & pj : corrected_event) {    
    xAOD::CaloCluster * cl = (*cont)[pj.user_index()];
    cl->setE(pj.e());
    cl->setPhi(pj.phi());
    cl->setEta(pj.eta());
  }

  return StatusCode::SUCCESS;
}

