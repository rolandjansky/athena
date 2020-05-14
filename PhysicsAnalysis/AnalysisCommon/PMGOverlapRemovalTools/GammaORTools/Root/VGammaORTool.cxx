/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GammaORTools/VGammaORTool.h"

VGammaORTool::VGammaORTool(const std::string& name)
  : asg::AsgTool(name),
    m_truthClassifier("MCTruthClassifier",this) {

  declareProperty("max_barcode", m_max_barcode = 1e5);

  declareProperty("n_leptons", m_n_leptons = -2);
  declareProperty("lepton_pdgIds", m_lepton_pdgIds = {11, -11, 13, -13, 15, -15});
  declareProperty("veto_lepton_origins", m_lepton_veto_origins = {3, 5, 6, 7, 8, 9, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                                                                  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42});
  declareProperty("preferred_lepton_origins", m_preferred_lepton_origins = {1, 2, 4, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21});

  declareProperty("veto_photon_origins", m_veto_photon_origins = {9, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
                                                                  42});

  declareProperty("photon_pT_cuts", m_photon_pT_cuts = {});
  declareProperty("dR_lepton_photon_cut", m_dR_lepton_photon_cut = 0.1);
  declareProperty("dR_lepton_photon_cuts", m_dR_lepton_photon_cuts = {0.0, 0.05, 0.075, 0.1, 0.125, 0.15, 0.2});

  declareProperty("use_gamma_iso", m_use_gamma_iso = false);
  declareProperty("frixione_dR", m_frixione_dR = 0.1);
  declareProperty("frixione_exponent", m_frixione_exponent = 2);
  declareProperty("frixione_epsilon", m_frixione_epsilon = 0.1);
  declareProperty("abs_pdgids_excluded_from_iso", m_abs_pdgids_excluded_from_iso = {11, 12, 13, 14, 15, 16, 22});
  declareProperty("min_considered_photon_pT", m_min_considered_photon_pT = 3.e3);

  declareProperty("truthparticle_collection_name", m_truthparticle_collection_name = "TruthParticles");

  for (int origin : m_preferred_lepton_origins) {
    if (std::find(m_lepton_veto_origins.begin(), m_lepton_veto_origins.end(), origin) != m_lepton_veto_origins.end()) {
      ATH_MSG_ERROR(origin << " in both lepton origin and lepton veto origin, this is not correct.");
      break;
    }
  }
  // pT cuts are sorted in descending order
  std::sort(m_photon_pT_cuts.begin(), m_photon_pT_cuts.end(), std::greater<float>());
}

VGammaORTool::~VGammaORTool() {
}


//============================================================================
// PUBLIC IN OVERLAP FUNCTION
// See header for description
StatusCode VGammaORTool::inOverlap(bool& result,
				   const std::vector<TLorentzVector>* leptons,
				   const std::vector<TLorentzVector>* photons,
				   const std::vector<int>* lepton_origins,
				   const std::vector<int>* photon_origins) const {
  std::vector<float> photon_pts;
  ANA_CHECK(photonPtsOutsideDr(photon_pts,leptons,photons,lepton_origins,photon_origins));
  result = checkPhotonPts(photon_pts);  
  return StatusCode::SUCCESS;
}

//============================================================================
// PUBLIC PTS OUTSIDE DR FUNCTION
// See header for description
StatusCode VGammaORTool::photonPtsOutsideDr(std::vector<float>& result,
					    const std::vector<TLorentzVector>* leptons,
					    const std::vector<TLorentzVector>* photons,
					    const std::vector<int>* lepton_origins,
					    const std::vector<int>* photon_origins) const {
  std::map<float, std::vector<float> > photon_pt_map;
  ANA_CHECK(photonPtsOutsideDrs(photon_pt_map,std::vector<float>(1, m_dR_lepton_photon_cut),leptons,photons,lepton_origins,photon_origins));
  result = photon_pt_map[m_dR_lepton_photon_cut];
  return StatusCode::SUCCESS;
}

//============================================================================
// PUBLIC PTS OUTSIDE DR*S* FUNCTION
// See header for description
StatusCode VGammaORTool::photonPtsOutsideDrs(std::map<float, std::vector<float> >& result,
					     const std::vector<TLorentzVector>* leptons,
					     const std::vector<TLorentzVector>* photons,
					     const std::vector<int>* lepton_origins,
					     const std::vector<int>* photon_origins) const {
  ANA_CHECK(photonPtsOutsideDrs(result,m_dR_lepton_photon_cuts,leptons,photons,lepton_origins,photon_origins));
  return StatusCode::SUCCESS;
}


//============================================================================
// PRIVATE PTS OUTSIDE DRS FUNCTION
// This function exectutes the OR algorithm if one of the public funtions is called
StatusCode VGammaORTool::photonPtsOutsideDrs(std::map<float, std::vector<float> >& result,
					     const std::vector<float>& drCuts,
					     const std::vector<TLorentzVector>* leptons,
					     const std::vector<TLorentzVector>* photons,
					     const std::vector<int>* lepton_origins,
					     const std::vector<int>* photon_origins) const {

  std::vector<TLorentzVector> good_leptons;
  std::vector<TLorentzVector> good_photons;
  ANA_CHECK(setInput(good_leptons,good_photons,leptons,photons,lepton_origins,photon_origins));
  
  // the actual OR algorithm is here, pts of photon outside dRs are determined first
  for (const auto& drCut : drCuts) {
    result[drCut] = std::vector<float>();
    for (const auto& photon : good_photons) {
      bool tooCloseToLepton = false;
      for (uint i_lep = 0; i_lep < good_leptons.size() && (m_n_leptons<0 || int(i_lep) < m_n_leptons); i_lep++) {
        const float dr = photon.DeltaR(good_leptons[i_lep]);
        if (dr < drCut) {
          tooCloseToLepton = true;
          break;
        }
      }
      if (!tooCloseToLepton) {
        result[drCut].push_back(photon.Pt());
      }
    }
    // photon pts are sorted and returned
    sort(result[drCut].begin(), result[drCut].end(), std::greater<float>());
  }

  return StatusCode::SUCCESS;
}

//============================================================================
// find the right leptons, get them either from user or the current event
StatusCode VGammaORTool::setInput(std::vector<TLorentzVector>& leptons_out,
				  std::vector<TLorentzVector>& photons_out,
				  const std::vector<TLorentzVector>* lepton_p4s,
				  const std::vector<TLorentzVector>* photon_p4s,
				  const std::vector<int>* lepton_origins,
				  const std::vector<int>* photon_origins) const {

  // truth particles are retrieved from event if not given by user
  const xAOD::TruthParticleContainer* truthParticles = nullptr;
  if(lepton_p4s==0 || photon_p4s==0){
    ANA_CHECK( evtStore()->retrieve(truthParticles, m_truthparticle_collection_name) );
  }

  // relevant photons and leptons identified
  if(lepton_p4s==0){
    leptons_out = getLeptonP4s(*truthParticles);
  }
  else{
    if(lepton_origins!=0){
      leptons_out = filterLeptonOrigins(*lepton_p4s,*lepton_origins);
    }
    else{
      leptons_out = *lepton_p4s;
    }
  }
  if(photon_p4s==0){
    photons_out = getPhotonP4s(*truthParticles);
  }
  else{
    if(photon_origins!=0){
      photons_out = filterPhotonOrigins(*photon_p4s,*photon_origins);
    }
    else{
      photons_out = *photon_p4s;
    }
  }

  ATH_MSG_DEBUG(BOOST_CURRENT_FUNCTION << ": Found " << photons_out.size() << " photons.");
  ATH_MSG_DEBUG(BOOST_CURRENT_FUNCTION << ": Found " << leptons_out.size() << " leptons.");

  if (m_n_leptons >=0 && int(leptons_out.size()) < m_n_leptons) {
    ATH_MSG_WARNING(
      BOOST_CURRENT_FUNCTION << ": Found " << leptons_out.size() << " leptons but expected " << m_n_leptons << ".");
  }
 
  return StatusCode::SUCCESS;
}


//============================================================================
// Functions to filter out particles from wrong origins
std::vector<TLorentzVector> VGammaORTool::filterPhotonOrigins(const std::vector<TLorentzVector>& photon_candidates,
                                                              const std::vector<int>& photon_origins) const {
  // this should only happen if the user gives the wrong input
  if (photon_candidates.size() != photon_origins.size()) {
    ATH_MSG_ERROR(
      BOOST_CURRENT_FUNCTION << ": size of photon candidates (" << photon_candidates.size() << ") different from number of photon origins (" << photon_origins.size() <<
        ").");
  }
  // filter out vetoed photons
  std::vector<TLorentzVector> photon_p4s;
  for (uint i = 0; i < photon_candidates.size(); i++) {
    const TLorentzVector p4 = photon_candidates[i];
    const int p_origin = photon_origins[i];
    const bool vetoed =
      std::find(m_veto_photon_origins.begin(), m_veto_photon_origins.end(), p_origin) != m_veto_photon_origins.end();
    if (!vetoed) {
      photon_p4s.push_back(p4);
    }
  }
  return photon_p4s;
}

std::vector<TLorentzVector> VGammaORTool::filterLeptonOrigins(const std::vector<TLorentzVector>& lepton_candidates,
                                                              const std::vector<int>& lepton_origins) const {
  // this should only happen if the user gives the wrong input
  if (lepton_candidates.size() != lepton_origins.size()) {
    ATH_MSG_ERROR(
      BOOST_CURRENT_FUNCTION << ": size of lepton candidates (" << lepton_candidates.size() << ") different from number of lepton origins (" << lepton_origins.size() <<
        ").");
  }
  std::vector<TLorentzVector> lepton_p4s;
  std::vector<TLorentzVector> leptons_not_vetoed_p4s;
  // find both good photons and photons that are just not vetoed (lower quality, used as fall back)
  for (uint i = 0; i < lepton_candidates.size(); i++) {
    const TLorentzVector p4 = lepton_candidates[i];
    const int p_origin = lepton_origins[i];
    const bool use = std::find(m_preferred_lepton_origins.begin(), m_preferred_lepton_origins.end(), p_origin) != m_preferred_lepton_origins.end();
    if (use) {
      lepton_p4s.push_back(p4);
      if (m_n_leptons>=0 && int(lepton_p4s.size()) >= m_n_leptons) {
        // as soon as enough leptons are found, return them
        // this is why the order of lepton candidates matters
        // if this tool identifies the leptons, taus will be before mu/el and otherwise leptons are ordered by pT
        // in most cases the origin should unambiguosly identify the lepton anyway
        return lepton_p4s;
      }
    } else {
      bool vetoed =
        std::find(m_lepton_veto_origins.begin(), m_lepton_veto_origins.end(), p_origin) != m_lepton_veto_origins.end();
      if (!vetoed) {
        leptons_not_vetoed_p4s.push_back(p4);
      }
    }
  }
  // if not enough good leptons were found (or no expected number of leptons was set),
  // the result is filled with leptons that were merely not vetoed
  for (const auto& l : leptons_not_vetoed_p4s) {
    lepton_p4s.push_back(l);
    if (m_n_leptons>=0 && int(lepton_p4s.size()) >= m_n_leptons) {
      return lepton_p4s;
    }
  }
  return lepton_p4s;
}

//============================================================================
// Get the right leptons and photons from the event
std::vector<TLorentzVector> VGammaORTool::getLeptonP4s(const xAOD::TruthParticleContainer& truthParticles) const {
  std::vector<const xAOD::TruthParticle*> tau_candidates;
  std::vector<const xAOD::TruthParticle*> elmu_candidates;
  for (const auto& p : truthParticles) {
    // ignore all particles with geant barcodes
    if (p->barcode() > m_max_barcode) {
      continue;
    }
    // ignore all particles with the wrong pdgid
    if (std::find(m_lepton_pdgIds.begin(), m_lepton_pdgIds.end(), p->pdgId()) == m_lepton_pdgIds.end()) {
      continue;
    }
    // handle taus: use tau instances before decay into non-tau
    if (std::abs(p->pdgId()) == 15) {
      bool childIsTau = false;
      bool hasChildren = false;
      // make sure tau has no tau children, i.e. is tau before decay
      for (uint i = 0; i < p->nChildren(); i++) {
        if (p->child(i) == nullptr) continue;
        if (p->child(i)->barcode() == p->barcode()) continue;
        hasChildren = true;
        if (p->child(i)->pdgId() == p->pdgId()) {
          childIsTau = true;
          break;
        }
      }
      if (hasChildren && !childIsTau) {
        tau_candidates.push_back(p);
      }
    }
    // electron and muons: use all status 1 not from tau
    else if (p->status() == 1 && !isFromTau(*p)) {
      elmu_candidates.push_back(p);
    }
  }
  // sort leptons by pT
  sort(tau_candidates.begin(), tau_candidates.end(),
       [](const xAOD::TruthParticle*& p1, const xAOD::TruthParticle*& p2) {
    return p1->pt() > p2->pt();
  });
  sort(elmu_candidates.begin(), elmu_candidates.end(),
       [](const xAOD::TruthParticle*& p1, const xAOD::TruthParticle*& p2) {
    return p1->pt() > p2->pt();
  });
  // put taus before other leptons in a vector
  std::vector<const xAOD::TruthParticle*> lepton_candidates(tau_candidates);
  lepton_candidates.insert(lepton_candidates.end(), elmu_candidates.begin(), elmu_candidates.end());
  // determine lepton origins
  std::vector<TLorentzVector> lepton_p4s;
  std::vector<int> lepton_origins;
  for (const auto& p : lepton_candidates) {
    auto res = const_cast<asg::AnaToolHandle<MCTruthClassifier>*>(&m_truthClassifier)->get()->particleTruthClassifier(p);
    lepton_origins.push_back(res.second);
    lepton_p4s.push_back(p->p4());
  }
  // filter out bad origins
  return filterLeptonOrigins(lepton_p4s, lepton_origins);
}

std::vector<TLorentzVector> VGammaORTool::getPhotonP4s(const xAOD::TruthParticleContainer& truthParticles) const {
  std::vector<TLorentzVector> photon_p4s;
  std::vector<int> photon_origins;
  for (const auto& p : truthParticles) {
    // consider only final state photons, not from geant, above a lower pt cut
    if (p->status() != 1 || p->barcode() > m_max_barcode || p->pdgId() != 22 || p->pt() < m_min_considered_photon_pT) {
      continue;
    }
    // require photons to be isolated if use_gamma_iso is true
    if (m_use_gamma_iso &&
        !frixioneIsolated(*p, truthParticles, m_frixione_dR, m_frixione_exponent, m_frixione_epsilon)) {
      continue;
    }
    // determine photon origin
    auto res = const_cast<asg::AnaToolHandle<MCTruthClassifier>*>(&m_truthClassifier)->get()->particleTruthClassifier(p);
    photon_origins.push_back(res.second);
    photon_p4s.push_back(p->p4());
  }
  // filter out bad photons
  return filterPhotonOrigins(photon_p4s, photon_origins);
}

bool VGammaORTool::isFromTau(const xAOD::TruthParticle& lepton, int nRecursions) const{
  // avoid being stuck in some weird particle family tree
  if(nRecursions>20){
    return false;
  }
  for(uint i=0; i<lepton.nParents(); i++){
    const xAOD::TruthParticle* parent=lepton.parent(i);
    if(abs(parent->pdgId())==15){
      return true;
    }
    if(parent->pdgId()==lepton.pdgId()){
      return isFromTau(lepton, nRecursions+1);
    }
  }  
  return false;

}

bool VGammaORTool::checkPhotonPts(const std::vector<float>& photon_pts) const{
  if(m_photon_pT_cuts.size()==0){
    ATH_MSG_ERROR("photon_pT_cuts needs to be a non-empty list, when inOverlap is called, set as property or in constructor.");
    return false;
  }
  if(m_photon_pT_cuts.size()>photon_pts.size()){
    return false;
  }
  for(uint i=0; i<m_photon_pT_cuts.size();i++){
    if(photon_pts[i]<m_photon_pT_cuts[i]){
      return false;
    }
  }
  return true;
}
//============================================================================
// Functions related to photon isolation
bool VGammaORTool::frixioneIsolated(const xAOD::TruthParticle& photon,
                                    const xAOD::TruthParticleContainer& truthParticles, float dR0, float exponent,
                                    float epsilon) const {
  // all photons are isolated if dR is negative
  if (dR0 <= 0.) {
    return true;
  }
  // create map between hadron-photon dr and hadron pt
  std::map<float, float> dr_to_pt;
  for (const auto& p : truthParticles) {
    // consider status 1  not from geant
    if (p->status() != 1 || p->barcode() > m_max_barcode) {
      continue;
    }
    // ignore what typically is leptons and photons
    if (std::find(m_abs_pdgids_excluded_from_iso.begin(), m_abs_pdgids_excluded_from_iso.end(),
                  std::abs(p->pdgId())) != m_abs_pdgids_excluded_from_iso.end()) {
      continue;
    }
    const float dRgamma = photon.p4().DeltaR(p->p4());
    // consider only photons withon isolation cone dR0
    if (dRgamma < dR0) {
      // should two hadrons have the same dr  they are added up
      if (dr_to_pt.count(dRgamma) > 0) {
        dr_to_pt[dRgamma] += p->pt();
      }
      dr_to_pt[dRgamma] = p->pt();
    }
  }
  // use map to determine whether photon is Frixione isolated
  // map is sorted from low to high dr
  float sumPt = 0.;
  for (const auto& pair : dr_to_pt) {
    sumPt += pair.second;
    const float dRgamma = pair.first;
    // if for any dRgamma the sum of pt of hadrons with a distrance of <=dRgamma to photon
    // does not fulfill the Frixione condition, the photon is not isolated
    if (sumPt > photon.pt() * frixioneFunc(dRgamma, dR0, exponent, epsilon)) {
      return false;
    }
  }
  // otherwise the photon is isolated
  return true;
}

float VGammaORTool::frixioneFunc(float dR, float dR0, float exponent, float epsilon) const {
  return epsilon * TMath::Power((1 - TMath::Cos(dR)) / (1 - TMath::Cos(dR0)), exponent);
}
