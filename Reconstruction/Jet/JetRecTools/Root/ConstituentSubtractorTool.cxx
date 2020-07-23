/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Source code for the ConstituentSubtractorTool implementation class
//
//
#include "JetRecTools/ConstituentSubtractorTool.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/contrib/ConstituentSubtractor.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include "fastjet/Selector.hh"

#include "xAODPFlow/PFO.h"
#include "xAODPFlow/TrackCaloCluster.h"

using namespace fastjet;
ConstituentSubtractorTool::ConstituentSubtractorTool(const std::string & name): JetConstituentModifierBase(name) {

  declareProperty("MaxDeltaR", m_maxDeltaR=0.25);
  declareProperty("Alpha", m_alpha=0.);
  declareProperty("MaxEta", m_maxEta=2.5);

  // Option to disregard cPFOs in the weight calculation
  declareProperty("IgnoreChargedPFO", m_ignoreChargedPFOs=true);
}


StatusCode ConstituentSubtractorTool::initialize() {

  if(m_inputType==xAOD::Type::ParticleFlow) {
    if(m_ignoreChargedPFOs && m_applyToChargedPFO) {
      ATH_MSG_ERROR("Incompatible configuration: setting both IgnoreChargedPFO and ApplyToChargedPFO to true"
		    <<  "will set all cPFOs to zero");
      return StatusCode::FAILURE;
    }
    if(!m_applyToNeutralPFO) {
      ATH_MSG_ERROR("Incompatible configuration: ApplyToNeutralPFO=False -- what kind of pileup do you wish to suppress?");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


// Apply PU weighting and decorate the CaloCluster container appropriately:

StatusCode ConstituentSubtractorTool::process_impl(xAOD::IParticleContainer* cont) const {

  contrib::ConstituentSubtractor subtractor;
  subtractor.set_max_standardDeltaR(m_maxDeltaR); // free parameter for the maximal allowed distance sqrt((y_i-y_k)^2+(phi_i-phi_k)^2) between particle i and ghost k
  subtractor.set_alpha(m_alpha);  // free parameter for the distance measure (the exponent of particle pt). The larger the parameter alpha, the more are favoured the lower pt particles in the subtraction process
  subtractor.set_ghost_area(0.01); // free parameter for the density of ghosts. The smaller, the better - but also the computation is slower.

  // prepare PseudoJet input from
  std::vector<PseudoJet>  full_event; full_event.reserve( cont->size() );
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
    if(m_inputType==xAOD::Type::TrackCaloCluster) {
      xAOD::TrackCaloCluster* tcc = static_cast<xAOD::TrackCaloCluster*>(part);
      accept &= (tcc->taste()!= 0);
    }
    // Reject object if outside maximum eta range
    accept &= fabs(part->eta()) <= m_maxEta;

    PseudoJet pj( part->p4() );
    pj.set_user_index( i );
    if(accept) {
      PseudoJet pj( part->p4() );
      pj.set_user_index( i );
      full_event.push_back(pj);
    }
    ++i;
  }


  // keep the particles up to 4 units in rapidity
  full_event = SelectorAbsEtaMax(m_maxEta)(full_event);

  // do the clustering with ghosts and get the jets
  //----------------------------------------------------------
  // the area definiton is used only for the jet backgroud estimator. It is not important for the ConstituentSubtractor when subtracting the whole event - this is not true when subtracting the individual jets
  AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(m_maxEta,fastjet::active_area_explicit_ghosts));

  std::vector<int> seed (2);
  const EventContext& ctx = Gaudi::Hive::currentContext();
  std::hash<std::string> h;
  seed[0] = ctx.eventID().run_number() + ctx.eventID().event_number();
  seed[1] = h(name());
  area_def.ghost_spec().set_random_status (seed);


  // create what we need for the background estimation
  //----------------------------------------------------------
  JetDefinition jet_def_for_rho(kt_algorithm, 0.4);
  Selector rho_range =  SelectorAbsEtaMax(m_maxEta-0.4);

  JetMedianBackgroundEstimator bge_rho(rho_range, jet_def_for_rho, area_def);
  BackgroundJetScalarPtDensity *scalarPtDensity=new BackgroundJetScalarPtDensity();
  bge_rho.set_jet_density_class(scalarPtDensity); // this changes the computation of pt of patches from vector sum to scalar sum. The scalar sum seems more reasonable.
  bge_rho.set_particles(full_event);
  subtractor.set_background_estimator(&bge_rho);

  std::vector<PseudoJet> corrected_event=subtractor.subtract_event(full_event,m_maxEta);

  // Define a vector holding the corrected four-momenta for all output constituents
  // This is defaulted to zero, because fastjet will only return non-zero pseudojets
  std::vector<xAOD::JetFourMom_t> corrected_p4s(cont->size(),xAOD::JetFourMom_t(0.,0.,0.,0.));
  // Set the corrected four-vectors
  for(PseudoJet & pj : corrected_event) {
    ATH_MSG_VERBOSE("Setting four-mom for constituent " << pj.user_index() << ", pt = " << pj.pt());
    corrected_p4s[pj.user_index()].SetCoordinates(pj.pt(),pj.eta(),pj.phi(),pj.m());
  }

  // Set every constituent's four-vector
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
