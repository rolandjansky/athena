 /*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Source code for the ConstituentSubtractorTool implementation class
//
//
#include "JetRecTools/ConstituentSubtractorTool.h"

#include "AsgDataHandles/ReadHandle.h"


#include "fastjet/PseudoJet.hh"
#include "fastjet/contrib/ConstituentSubtractor.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/GridMedianBackgroundEstimator.hh"
#include "fastjet/contrib/RescalingClasses.hh"

#include "xAODPFlow/PFO.h"
#include "xAODPFlow/TrackCaloCluster.h"
#include "xAODPFlow/FlowElement.h"
#include "PathResolver/PathResolver.h"
#include "TFile.h"

using namespace fastjet;
ConstituentSubtractorTool::ConstituentSubtractorTool(const std::string & name): JetConstituentModifierBase(name) {
}


StatusCode ConstituentSubtractorTool::initialize() {

  if(m_inputType==xAOD::Type::ParticleFlow || m_inputType==xAOD::Type::FlowElement) {
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
  ATH_CHECK( m_eventinfokey.initialize() );
  

  if(m_doRapidityRescaling && m_doRapidityPhiRescaling) {
    ATH_MSG_ERROR("Incompatible configuration: You set both, DoRapidityRescaling and DoRapidityPhiRescaling, to true. Use maximally only one of them.");
    return StatusCode::FAILURE;
  }

  if((m_doRapidityRescaling || m_doRapidityPhiRescaling) && (m_rescalingFileName=="" || m_rescalingHistogramName=="")) {
    ATH_MSG_ERROR("Incompatible configuration: You have chosen a background rescaling, but you have not specified the path to the file with rescaling histograms or the name of the histogram. Specify properties FileRescaling and HistogramRescaling.");
    return StatusCode::FAILURE;
  }

  if(m_doRapidityRescaling || m_doRapidityPhiRescaling){
    std::string fullPathToFile=PathResolverFindCalibFile(m_rescalingFileName); // returns "" if file not found
    if (fullPathToFile.empty()){
      ATH_MSG_ERROR("Incompatible configuration: The provided file for rescaling was not found using PathResolver.");
      return StatusCode::FAILURE;
    }
    std::unique_ptr<TFile> file(TFile::Open(fullPathToFile.data(), "READ"));
    if (file->IsZombie()){
      ATH_MSG_ERROR("Incompatible configuration: The file for rescaling has been tried to open, but it was found it is zombie.");
      return StatusCode::FAILURE;
    }
    std::unique_ptr<TObject> object(file->Get(m_rescalingHistogramName.value().data()));
    if (!object){
      ATH_MSG_ERROR("Incompatible configuration: The provided histogram name was not found in the root file.");
      return StatusCode::FAILURE;
    }

    if (m_doRapidityRescaling){
      if (object->InheritsFrom(TH1D::Class())){
        m_hist.reset(static_cast<TH1D*>(object->Clone("hist_cloned")));
        m_hist->SetDirectory(0);
        m_rescaling.reset(static_cast<fastjet::FunctionOfPseudoJet<double>*>(new contrib::BackgroundRescalingYFromRoot<TH1D>(m_hist.get())));
      }
      else{
        ATH_MSG_ERROR("Incompatible configuration: You want to do rapidity rescaling, but the provided histogram name is not a TH1D.");
        return StatusCode::FAILURE;
      }
    }
    if (m_doRapidityPhiRescaling){
      if (object->InheritsFrom(TH2D::Class())){
        m_hist2D.reset(static_cast<TH2D*>(object->Clone("hist_cloned")));
        m_hist2D->SetDirectory(0);
        m_rescaling.reset(static_cast<fastjet::FunctionOfPseudoJet<double>*>(new contrib::BackgroundRescalingYPhiFromRoot<TH2D>(m_hist2D.get())));
      }
      else{
        ATH_MSG_ERROR("Incompatible configuration: You want to do rapidity-phi rescaling, but the provided histogram name is not a TH2D.");
        return StatusCode::FAILURE;
      }
    }
  }

  
  return StatusCode::SUCCESS;
}


// Apply PU weighting and decorate the CaloCluster container appropriately:

StatusCode ConstituentSubtractorTool::process_impl(xAOD::IParticleContainer* cont) const {

  contrib::ConstituentSubtractor subtractor;
 
  // free parameter for the maximal allowed distance sqrt((y_i-y_k)^2+(phi_i-phi_k)^2) between particle i and ghost k
  subtractor.set_max_standardDeltaR(m_maxDeltaR);
 
  // free parameter for the distance measure (the exponent of particle pt). The larger the parameter alpha, the more are favoured the lower pt particles in the subtraction process
  subtractor.set_alpha(m_alpha);
 
  // free parameter for the density of ghosts. The smaller, the better - but also the computation is slower.
  subtractor.set_ghost_area(m_ghostArea);

  // This is added to fix ATR-23552. It has no effect on the performance. Once the bug is fixed in fastjet-contrib, it can be removed.
  subtractor.set_use_nearby_hard(-1,-1);

  // prepare PseudoJet input
  std::vector<PseudoJet> inputs_to_correct, inputs_to_not_correct;
  inputs_to_correct.reserve(cont->size());
  inputs_to_not_correct.reserve(cont->size());
  size_t i =0; // Corresponds to the index in the input container
  // We don't use part->index() because it might be a view container
  // combining more than one owning container
 
  // Minimal and maximum rapidities needed for the workaround for the bug in fastjet-contrib ConstituentSubtractor, see ATLASG-1417
  double minRap=1000;
  double maxRap=-1000;
 
  for(xAOD::IParticle * part: *cont){
    // Only use positive E
    bool accept = part->e() > -1*FLT_MIN;
    // For PFlow we would only want to apply the correction to neutral PFOs,
    // because charged hadron subtraction handles the charged PFOs.
    // However, we might still want to use the cPFOs for the min pt calculation
    if(m_ignoreChargedPFOs){
      if(m_inputType==xAOD::Type::ParticleFlow){
        xAOD::PFO* pfo = static_cast<xAOD::PFO*>(part);
        accept &= fabs(pfo->charge())<FLT_MIN;
      }
      else if(m_inputType==xAOD::Type::FlowElement){
        xAOD::FlowElement* fe = static_cast<xAOD::FlowElement*>(part);
        accept &= !(fe->isCharged());
      }
    }
    if(m_inputType==xAOD::Type::TrackCaloCluster) {
      xAOD::TrackCaloCluster* tcc = static_cast<xAOD::TrackCaloCluster*>(part);
      accept &= (tcc->taste()!= 0)*(tcc->pt()>-1.*FLT_MIN);
    }
    // Reject object if outside maximum eta range
    accept &= fabs(part->eta()) <= m_maxEta;
 
    PseudoJet pj( part->p4() );
    pj.set_user_index( i );
    if(accept) {
      ATH_MSG_VERBOSE("Using " << part->type() << " with pt " << part->pt());
      inputs_to_correct.push_back(pj);
      // Minimal and maximum rapidities needed for the workaround for the bug in fastjet-contrib ConstituentSubtractor, see ATLASG-1417
      if (pj.rap()<minRap) minRap = pj.rap();
      if (pj.rap()>maxRap) maxRap = pj.rap();
    } else {
      ATH_MSG_VERBOSE("Will not correct " << part->type() << " with pt " << part->pt());
      inputs_to_not_correct.push_back(pj);
    }
 
    ++i;
  }
  
  // create what we need for the background estimation
  //----------------------------------------------------------
  // maximal rapidity is used (not pseudo-rapidity). Since the inputs are massless, it does not matter
  GridMedianBackgroundEstimator bge_rho(m_maxRapForRhoComputation, m_gridSize);
  bge_rho.set_rescaling_class(m_rescaling.get());
  bge_rho.set_particles(inputs_to_correct);
  subtractor.set_background_estimator(&bge_rho);
 
  // for massless input particles it does not make any difference (rho_m is always zero)
  
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
