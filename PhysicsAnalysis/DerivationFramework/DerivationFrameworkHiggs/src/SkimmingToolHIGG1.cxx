/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SkimmingToolHIGG1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Based on DerivationFramework::SkimmingToolExample

#include "DerivationFrameworkHiggs/SkimmingToolHIGG1.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "PhotonVertexSelection/IPhotonVertexSelectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"


// Constructor
DerivationFramework::SkimmingToolHIGG1::SkimmingToolHIGG1(const std::string& t,
							    const std::string& n,
							    const IInterface* p) : 
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_isMC(false),
  n_tot(0),
  n_passGRL(0),
  n_passLArError(0),
  n_passTrigger(0),
  n_passPreselect(0),
  n_passSingleElectronPreselect(0),
  n_passDoubleElectronPreselect(0),
  n_passSingleMuonPreselect(0),
  n_passSinglePhotonDoubleMuonPreselect(0),
  n_passSinglePhotonDoubleElectronPreselect(0),
  n_passSinglePhotonMergedElectronPreselect(0),
  n_passHighPtPhotonMergedElectronPreselect(0),
  n_passKinematic(0),
  n_passQuality(0),
  n_passIsolation(0),
  n_passInvariantMass(0),
  n_pass(0),
  e_tot(false),
  e_passGRL(false),
  e_passLArError(false),
  e_passTrigger(false),
  e_passPreselect(false),
  e_passSingleElectronPreselect(false),
  e_passDoubleElectronPreselect(false),
  e_passSingleMuonPreselect(false),
  e_passKinematic(false),
  e_passQuality(false),
  e_passIsolation(false),
  e_passInvariantMass(false),
  e_pass(false),
  ph_pos_lead(0), 
  ph_pos_subl(0),
  ph_pt_lead(0.), 
  ph_eta_lead(0.), 
  ph_phi_lead(0.), 
  ph_e_lead(0.),
  ph_pt_subl(0.), 
  ph_eta_subl(0.), 
  ph_phi_subl(0.), 
  ph_e_subl(0.),
  ph_tight_lead(0), 
  ph_tight_subl(0.),
  ph_iso_lead(0), 
  ph_iso_subl(0),
  e_invariantMass(0.),
  e_diphotonZ(0.),  
  m_mergedCutTools("")
{

  declareInterface<DerivationFramework::ISkimmingTool>(this);
 
 
  declareProperty("PhotonContainerKey",    m_photonSGKey="Photons");
  declareProperty("ElectronContainer",     m_electronSGKey = "Electrons");
  declareProperty("MuonContainer",         m_muonSGKey = "Muons");

  declareProperty("RequireGRL",            m_reqGRL           = true);
  declareProperty("ReqireLArError",        m_reqLArError      = true);
  declareProperty("RequireTrigger",        m_reqTrigger       = true);
  declareProperty("RequirePreselection",   m_reqPreselection  = true);
  declareProperty("IncludeSingleElectronPreselection",   m_incSingleElectron  = true);
  declareProperty("IncludeDoubleElectronPreselection",   m_incDoubleElectron  = false);
  declareProperty("IncludeSingleMuonPreselection",       m_incSingleMuon  = true);
  declareProperty("IncludeDoubleMuonPreselection",       m_incDoubleMuon  = false);
  declareProperty("IncludePhotonDoubleElectronPreselection", m_incDoubleElectronPhoton  = false);
  declareProperty("IncludePhotonMergedElectronPreselection", m_incMergedElectron = false);
  declareProperty("IncludeHighPtPhotonElectronPreselection", m_incHighPtElectronPhoton = false);
  declareProperty("IncludeDoublePhotonPreselection",     m_incTwoPhotons  = true);

  declareProperty("RequireKinematic",      m_reqKinematic     = true);
  declareProperty("RequireQuality",        m_reqQuality       = true);
  declareProperty("RequireIsolation",      m_reqIsolation     = true);
  declareProperty("RequireInvariantMass",  m_reqInvariantMass = true);

  declareProperty("GoodRunList",           m_goodRunList = "");

  declareProperty("DefaultTrigger",        m_defaultTrigger = "EF_g35_loose_g25_loose");
  declareProperty("Triggers",              m_triggers = std::vector<std::string>()); 
  declareProperty("MergedElectronTriggers",m_mergedtriggers = std::vector<std::string>() );

  declareProperty("MinimumPhotonPt",       m_minPhotonPt = 20*CLHEP::GeV);
  declareProperty("MinimumElectronPt",     m_minElectronPt = 20*CLHEP::GeV);
  declareProperty("MinimumMuonPt",         m_minMuonPt = 20*CLHEP::GeV);
  declareProperty("MaxMuonEta",            m_maxMuonEta = 2.7);
  declareProperty("RemoveCrack",           m_removeCrack = true);
  declareProperty("MaxEta",                m_maxEta = 2.47);

  declareProperty("RelativePtCuts",        m_relativePtCuts     = true);
  declareProperty("LeadingPhotonPtCut",    m_leadingPhotonPt    = 0.35);
  declareProperty("SubleadingPhotonPtCut", m_subleadingPhotonPt = 0.25);

  declareProperty("MinInvariantMass",      m_minInvariantMass = 105*CLHEP::GeV);
  declareProperty("MaxInvariantMass",      m_maxInvariantMass = 160*CLHEP::GeV);

  declareProperty("MergedElectronCutTool",  m_mergedCutTools);

}
  
// Destructor
DerivationFramework::SkimmingToolHIGG1::~SkimmingToolHIGG1() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolHIGG1::initialize()
{
  ATH_MSG_VERBOSE("INITIALIZING HSG1 SELECTOR TOOL");

  ////////////////////////////
  // trigger decision tool
  if(m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
    return StatusCode::FAILURE;
  }
  if (!m_triggers.size()) m_triggers.push_back(m_defaultTrigger);
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);
  ////////////////////////////
  //
  if(m_incMergedElectron){
    if( m_mergedCutTools.retrieve().isFailure() )
    {
      ATH_MSG_FATAL("Failed to retrieve tool: ElectronPhotonSelectorTools");
      return StatusCode::FAILURE;
    }
  }
  ////////////////////////////
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::SkimmingToolHIGG1::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << n_tot << " events, " << n_pass << " events passed filter ");

  
  ATH_MSG_INFO("GRL       :: " << n_passGRL);
  ATH_MSG_INFO("lar       :: " << n_passLArError);
  ATH_MSG_INFO("trig      :: " << n_passTrigger);
  ATH_MSG_INFO("----------------------------");
  if(m_incDoubleElectron)
    ATH_MSG_INFO("2e        :: " << n_passDoubleElectronPreselect);
  if(m_incSingleElectron)
    ATH_MSG_INFO("1y1e      :: " << n_passSingleElectronPreselect);
  if(m_incSingleMuon)
    ATH_MSG_INFO("1y1mu     :: " << n_passSingleMuonPreselect);
  if(m_incDoubleMuon)
    ATH_MSG_INFO("1y2mu     :: " << n_passSinglePhotonDoubleMuonPreselect);
  if(m_incDoubleElectronPhoton)
    ATH_MSG_INFO("1y2e      :: " << n_passSinglePhotonDoubleElectronPreselect);
  if(m_incMergedElectron)
    ATH_MSG_INFO("1y1eMerge :: " << n_passSinglePhotonMergedElectronPreselect);
  if(m_incHighPtElectronPhoton)
    ATH_MSG_INFO("1y1e HiPt :: " << n_passHighPtPhotonMergedElectronPreselect);

  if(m_incTwoPhotons){
    ATH_MSG_INFO("2y        :: " << n_passPreselect);
    ATH_MSG_INFO("----------------------------");
    ATH_MSG_INFO("2y - kin       :: " << n_passKinematic);
    ATH_MSG_INFO("2y - qual      :: " << n_passQuality);
    ATH_MSG_INFO("2y - iso       :: " << n_passIsolation);
    ATH_MSG_INFO("2y - inv       :: " << n_passInvariantMass);
  }
  ATH_MSG_INFO("----------------------------");
  ATH_MSG_INFO("passed     :: " << n_pass);
  
  return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::SkimmingToolHIGG1::eventPassesFilter() const
{

  n_tot++;

  bool writeEvent(false);

  const xAOD::EventInfo *eventInfo(0);
  if((evtStore()->retrieve(eventInfo)).isFailure()) return false;
  m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  
  if (m_reqGRL      && !SubcutGoodRunList() ) return false;
  if (m_reqLArError && !SubcutLArError()    ) return false;
  if (m_reqTrigger  && !SubcutTrigger()     ) return false;

  SubcutPreselect();
  if (m_incTwoPhotons && !m_reqPreselection) writeEvent = true;	    

  // ey, ee, muy events
  if (m_incSingleElectron && SubcutOnePhotonOneElectron() ) writeEvent = true;
  if (m_incDoubleElectron && SubcutTwoElectrons()         ) writeEvent = true;
  if (m_incSingleMuon     && SubcutOnePhotonOneMuon()     ) writeEvent = true;
  
  // eey, mumuy events
  if (m_incMergedElectron       && SubcutOnePhotonMergedElectrons()) writeEvent = true;               
  if (m_incDoubleMuon           && SubcutOnePhotonTwoMuons()       ) writeEvent = true;
  if (m_incDoubleElectronPhoton && SubcutOnePhotonTwoElectrons()   ) writeEvent = true;
  if (m_incHighPtElectronPhoton && SubcutHighPtOnePhotonOneElectron() ) writeEvent = true;

  // There *must* be two photons for the remaining 
  // pieces, but you can still save the event...
  if (m_incTwoPhotons && e_passPreselect) {
    GetDiphotonVertex(); 
    CalculateInvariantMass();

    bool passTwoPhotonCuts(true);     
    if (m_reqQuality         && !SubcutQuality()       ) passTwoPhotonCuts = false; 
    if (m_reqKinematic       && !SubcutKinematic()     ) passTwoPhotonCuts = false; 
    if (m_reqIsolation       && !SubcutIsolation()     ) passTwoPhotonCuts = false; 
    if (m_reqInvariantMass   && !SubcutInvariantMass() ) passTwoPhotonCuts = false;
    // yy events
    if (passTwoPhotonCuts) writeEvent = true; 
    
  }
   

  if (!writeEvent) return false;
  
  n_pass++;
  return true;
}

bool DerivationFramework::SkimmingToolHIGG1::SubcutGoodRunList() const {

  // Placeholder

  e_passGRL = true;
  
  if (e_passGRL) n_passGRL++;
  return e_passGRL;

}
  
  
bool DerivationFramework::SkimmingToolHIGG1::SubcutLArError() const {

  // Retrieve EventInfo
  const xAOD::EventInfo *eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo)); 

  e_passLArError = !(eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error);
  
  if (e_passLArError) n_passLArError++;
  return e_passLArError;

}


bool DerivationFramework::SkimmingToolHIGG1::SubcutTrigger() const {

  //just for counting purposes
  e_passTrigger = m_reqTrigger ? false : true;
  
  if(m_triggers.size()==0) e_passTrigger = true;

  for (unsigned int i = 0; i < m_triggers.size(); i++) {
    ATH_MSG_DEBUG("TRIGGER = " << m_triggers.at(i));
    if(m_trigDecisionTool->isPassed(m_triggers.at(i)))
      e_passTrigger = true;
  }
  
  if (e_passTrigger) n_passTrigger++;
  return e_passTrigger;

}


bool DerivationFramework::SkimmingToolHIGG1::SubcutPreselect() const {

  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  e_passPreselect = false;
  ph_pos_lead = ph_pos_subl = -1;
  ph_pt_lead = ph_pt_subl= 0;

  for(int i = 0; ph_itr != ph_end; ++ph_itr, ++i) {

    if (PhotonPreselect(*ph_itr)) {

      if ((*ph_itr)->pt() > ph_pt_lead) {

        ph_pos_subl = ph_pos_lead; ph_pos_lead = i;
        ph_pt_subl = ph_pt_lead; 
        ph_pt_lead = (*ph_itr)->pt();

      } else if ((*ph_itr)->pt() > ph_pt_subl) {
        ph_pos_subl = i;
        ph_pt_subl = (*ph_itr)->pt();
      }
    }
  }

  // save this for the derivation.
  //std::vector<int> *leadingV = new std::vector<int>();
  //leadingV->push_back(ph_pos_lead);
  //leadingV->push_back(ph_pos_subl);
  //if (!evtStore()->contains<std::vector<int> >("leadingV")) CHECK(evtStore()->record(leadingV, "leadingV"));

  // save this for this code.
  if (ph_pos_subl != -1) {
    e_leadingPhotons.clear();
    e_leadingPhotons.push_back(*(photons->begin() + ph_pos_lead));
    e_leadingPhotons.push_back(*(photons->begin() + ph_pos_subl));
    e_passPreselect = true;
    n_passPreselect++;
    
    return true;
  }

  e_passPreselect = false;
  return false;

}


bool DerivationFramework::SkimmingToolHIGG1::PhotonPreselect(const xAOD::Photon *ph) const {

  if (!ph) return false;

  if (!ph->isGoodOQ(34214)) return false;

  bool val(false);
  bool defined(false);

  if(ph->isAvailable<char>("DFCommonPhotonsIsEMLoose")){
    defined = true;
    val = static_cast<bool>(ph->auxdata<char>("DFCommonPhotonsIsEMLoose"));
  }
  else{
    defined = ph->passSelection(val, "Loose");
  }
  
  if(!defined || !val) return false;
  

  // veto topo-seeded clusters 
  // uint16_t author = 0;
  // author = ph->author();  
  // if (author & xAOD::EgammaParameters::AuthorCaloTopo35) return false;

  // Check which variable versions are best...
  const xAOD::CaloCluster *caloCluster(ph->caloCluster());
  double eta = fabs(caloCluster->etaBE(2));

  if (eta > m_maxEta)             return false;
  if (m_removeCrack && 
      1.37 <= eta && eta <= 1.52) return false;
  if (caloCluster->e()/cosh(eta) < m_minPhotonPt) return false;

  return true;

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutKinematic() const {

  if (m_relativePtCuts) {
    e_passKinematic =  (ph_pt_lead > e_invariantMass * m_leadingPhotonPt);
    e_passKinematic &= (ph_pt_subl > e_invariantMass * m_subleadingPhotonPt);
  } else {
    e_passKinematic =  (ph_pt_lead > m_leadingPhotonPt);
    e_passKinematic &= (ph_pt_subl > m_subleadingPhotonPt);
  }

  if (e_passKinematic) n_passKinematic++;
  return e_passKinematic;

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutQuality() const {

  bool val(0);
  e_passQuality = false;
  e_leadingPhotons.at(0)->passSelection(val, "Tight");
  ph_tight_lead = val;

  e_leadingPhotons.at(1)->passSelection(val, "Tight");
  ph_tight_subl = val;

  e_passQuality = (ph_tight_lead && ph_tight_subl);

  if (e_passQuality) n_passQuality++;
  return e_passQuality;

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutIsolation() const {

  // PLACEHOLDER!!!

  e_passIsolation= true;
  
  if (e_passIsolation) n_passIsolation++;
  return e_passIsolation;

}


bool DerivationFramework::SkimmingToolHIGG1::SubcutInvariantMass() const {

// ATH_MSG_INFO("val=" << e_invariantMass << "  min=" << m_minInvariantMass << "  max=" << m_maxInvariantMass);

  e_passInvariantMass =  (!m_minInvariantMass || 
                           m_minInvariantMass < e_invariantMass);

  e_passInvariantMass &= (!m_maxInvariantMass || 
                           e_invariantMass < m_maxInvariantMass);

  if (e_passInvariantMass) n_passInvariantMass++;
  return e_passInvariantMass;

}

void DerivationFramework::SkimmingToolHIGG1::CalculateInvariantMass() const {

  /// CAUTION - PLACEHOLDERS
  ph_e_lead   = CorrectedEnergy(e_leadingPhotons.at(0));
  ph_e_subl   = CorrectedEnergy(e_leadingPhotons.at(1));

  /// CAUTION - CONSTANTS SHOULD BE UPDATED.
  ph_eta_lead = CorrectedEta(e_leadingPhotons.at(0));
  ph_eta_subl = CorrectedEta(e_leadingPhotons.at(1));

  ph_phi_lead = e_leadingPhotons.at(0)->phi();
  ph_phi_subl = e_leadingPhotons.at(1)->phi();

  ph_pt_lead  = ph_e_lead / cosh(ph_eta_lead);
  ph_pt_subl  = ph_e_subl / cosh(ph_eta_subl);


  leadPhotonLV.SetPtEtaPhiM(ph_pt_lead, ph_eta_lead, ph_phi_lead, 0.);
  sublPhotonLV.SetPtEtaPhiM(ph_pt_subl, ph_eta_subl, ph_phi_subl, 0.);

  e_invariantMass = (leadPhotonLV + sublPhotonLV).M();

  return;

}



void DerivationFramework::SkimmingToolHIGG1::GetDiphotonVertex() const {

  e_diphotonZ = 0; 

}

//// THIS IS A PLACEHOLDER!!
double DerivationFramework::SkimmingToolHIGG1::CorrectedEnergy(const xAOD::Photon *ph) const {

  return ph->e();

}


//////////  THE FOLLOWING TWO FUNCTIONS ARE ADAPTED FROM 
//////////  RUN I HSG1 CUT FLOWS: USE WITH CARE AND CHECK!!!
double DerivationFramework::SkimmingToolHIGG1::CorrectedEta(const xAOD::Photon *ph) const {

  double eta1 = ph->caloCluster()->etaBE(1); 

  double R_photon_front, Z_photon_front;
  if (fabs(eta1) < 1.5) { // barrel
    R_photon_front = ReturnRZ_1stSampling_cscopt2(eta1);
    Z_photon_front = R_photon_front*sinh(eta1);
  } else { // endcap
    Z_photon_front = ReturnRZ_1stSampling_cscopt2(eta1);
    R_photon_front = Z_photon_front/sinh(eta1);
  }

  return asinh((Z_photon_front - e_diphotonZ)/R_photon_front);

}


double DerivationFramework::SkimmingToolHIGG1::ReturnRZ_1stSampling_cscopt2(double eta1) const {

  float abs_eta1 = fabs(eta1);

  double radius = -99999;
  if (abs_eta1 < 0.8) {
    radius = 1558.859292 - 4.990838  * abs_eta1 - 21.144279 * abs_eta1 * abs_eta1;
  } else if (abs_eta1 < 1.5) {
    radius = 1522.775373 + 27.970192 * abs_eta1 - 21.104108 * abs_eta1 * abs_eta1;
  } else { //endcap
    radius = 3790.671754;
    if (eta1 < 0.) radius = -radius;
  }

  return radius;

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutOnePhotonOneElectron() const {

  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  const xAOD::ElectronContainer *electrons(0);
  ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));
  xAOD::ElectronContainer::const_iterator el_itr(electrons->begin());
  xAOD::ElectronContainer::const_iterator el_end(electrons->end());

  e_passSingleElectronPreselect = false;

  for( ; ph_itr != ph_end; ++ph_itr){
    if(PhotonPreselect(*ph_itr)){
      for( ; el_itr != el_end; ++el_itr){
        if(ElectronPreselect(*el_itr)){
          e_passSingleElectronPreselect = true;
        }
      }
    }
  }


  if(e_passSingleElectronPreselect) n_passSingleElectronPreselect++;
  return e_passSingleElectronPreselect;
}

bool DerivationFramework::SkimmingToolHIGG1::SubcutTwoElectrons() const {

  const xAOD::ElectronContainer *electrons(0);
  ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));
  xAOD::ElectronContainer::const_iterator el_itr(electrons->begin());
  xAOD::ElectronContainer::const_iterator el_end(electrons->end());

  int nEle(0);
  e_passDoubleElectronPreselect = false;
  
  for( ; el_itr != el_end; ++el_itr){
    if(ElectronPreselect(*el_itr))
      nEle++;
  }
  
  if(nEle >=2) e_passDoubleElectronPreselect = true;
  
  if(e_passDoubleElectronPreselect) n_passDoubleElectronPreselect++;
  return e_passDoubleElectronPreselect;
}


bool DerivationFramework::SkimmingToolHIGG1::SubcutOnePhotonOneMuon() const {

  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  const xAOD::MuonContainer *muons(0);
  ATH_CHECK(evtStore()->retrieve(muons, m_muonSGKey));
  xAOD::MuonContainer::const_iterator mu_itr(muons->begin());
  xAOD::MuonContainer::const_iterator mu_end(muons->end());

  e_passSingleMuonPreselect = false;

  for( ; ph_itr != ph_end; ++ph_itr){
    if(PhotonPreselect(*ph_itr)){
      for( ; mu_itr != mu_end; ++mu_itr){
        if(MuonPreselect(*mu_itr)){
          e_passSingleMuonPreselect = true;
        }
      }
    }
  }


  if(e_passSingleMuonPreselect) n_passSingleMuonPreselect++;
  return e_passSingleMuonPreselect;
}

bool DerivationFramework::SkimmingToolHIGG1::SubcutOnePhotonTwoMuons() const
{
  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  const xAOD::MuonContainer *muons(0);
  ATH_CHECK(evtStore()->retrieve(muons, m_muonSGKey));
  xAOD::MuonContainer::const_iterator mu_itr(muons->begin());
  xAOD::MuonContainer::const_iterator mu_end(muons->end());

  int nPhoton = 0;
  int nMuon   = 0;

  for( ; ph_itr != ph_end; ++ph_itr){
    if(PhotonPreselect(*ph_itr)){
      ++nPhoton;
    }
  }

  for( ; mu_itr != mu_end; ++mu_itr){
    if(MuonPreselect(*mu_itr)){
      ++nMuon;
    }
  }


  if(nPhoton >= 1 &&  nMuon >= 2){
    ATH_MSG_DEBUG("Event selected with " << nPhoton << " photons and " << nMuon << " muons");
    n_passSinglePhotonDoubleMuonPreselect++;
    return true;
  } else {
    return false;
  }
}


bool DerivationFramework::SkimmingToolHIGG1::SubcutOnePhotonTwoElectrons() const
{
  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  const xAOD::ElectronContainer *electrons(0);
  ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));
  xAOD::ElectronContainer::const_iterator el_itr(electrons->begin());
  xAOD::ElectronContainer::const_iterator el_end(electrons->end());

  int nPhoton    = 0;
  int nElectron  = 0;

  for( ; ph_itr != ph_end; ++ph_itr){
    if(PhotonPreselect(*ph_itr)){
      ++nPhoton;
    }
  }

  for( ; el_itr != el_end; ++el_itr){
    if(ElectronPreselect(*el_itr)){
      ++nElectron; 
    }
  }

  if(nPhoton >= 1 &&  nElectron >= 2){
    ATH_MSG_DEBUG("Event selected with " << nPhoton << " photons and " << nElectron << " electrons");
    ++n_passSinglePhotonDoubleElectronPreselect;
    return true;
  } else {
    return false;
  }

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutOnePhotonMergedElectrons() const
{


  bool passTrigger=false;
  if(m_mergedtriggers.size()!=0) {
    for (unsigned int i = 0; i < m_mergedtriggers.size(); i++) {
      ATH_MSG_DEBUG("TRIGGER = " << m_mergedtriggers.at(i));
      if(m_trigDecisionTool->isPassed(m_mergedtriggers.at(i)))
        passTrigger = true;
    }
  } else {
    ATH_MSG_WARNING("Selecting Merged electrons but no Merged Triggers Selected ! -- was that intentional?");
    passTrigger =  true;
  }
  if(!passTrigger)
    return false;
   

  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));

  const xAOD::ElectronContainer *electrons(0);
  ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));
  
  bool passSelection = false;

  for(auto el : *electrons){
    if(MergedElectronPreselect(el)){
      for(auto ph: *photons){
        if(PhotonPreselect(ph)){
          passSelection = true;
          auto eph = ph->p4() + el->p4();
          if(eph.M() >  90 * CLHEP::GeV)  
          {  
            break;
          }
        }
      }
    }
    if(passSelection)
    { 
      break;
    }
  }

  if(passSelection){
    ATH_MSG_DEBUG("Event selected with a photons and a merged electron");
    ++n_passSinglePhotonMergedElectronPreselect;
    return true;
  } else {
    return false;
  }

}


bool DerivationFramework::SkimmingToolHIGG1::SubcutHighPtOnePhotonOneElectron() const
{


  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  const xAOD::ElectronContainer *electrons(0);
  ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));
  xAOD::ElectronContainer::const_iterator el_itr(electrons->begin());
  xAOD::ElectronContainer::const_iterator el_end(electrons->end());

  int nPhoton    = 0;
  int nElectron  = 0;

  for( ; ph_itr != ph_end; ++ph_itr){
    if(PhotonPreselect(*ph_itr) && (*ph_itr)->pt() > 500*CLHEP::GeV){
      ++nPhoton;
    }
  }

  for( ; el_itr != el_end; ++el_itr){
    if( fabs((*el_itr)->eta()) <= m_maxEta  && (*el_itr)->pt() > m_minElectronPt){
      ++nElectron; 
    }
  }

  if(nPhoton >= 1 &&  nElectron >= 1 ){
    ATH_MSG_DEBUG("Event selected with " << nPhoton << " high pt photons and " << nElectron << " merged electron");
    ++n_passHighPtPhotonMergedElectronPreselect;
    return true;
  } else {
    return false;
  }

}



bool DerivationFramework::SkimmingToolHIGG1::ElectronPreselect(const xAOD::Electron *el) const {

  if (!el) return false;

  bool val(false);
  bool defined(false);

  if(el->isAvailable<char>("DFCommonElectronsLoose")){
    defined = true;
    val = val || static_cast<bool>(el->auxdata<char>("DFCommonElectronsLoose"));
  }else{
    defined = el->passSelection(val, "Loose");
  } 

  if(el->isAvailable<char>("DFCommonElectronsLHLoose")){
    defined = true;
    val = val || static_cast<bool>(el->auxdata<char>("DFCommonElectronsLHLoose"));
  }

  if(!defined || !val) return false;

  double eta = fabs(el->eta());
  double pt = el->pt();

  if (eta > m_maxEta) return false;
  if (m_removeCrack && 1.37 <= eta && eta <= 1.52) return false;
  if (pt <= m_minElectronPt) return false;

  return true;

}

bool DerivationFramework::SkimmingToolHIGG1::MergedElectronPreselect(const xAOD::Electron *el) const {

  if (!el) return false;

  double eta = fabs(el->eta());
  double pt = el->pt();

  if (eta > m_maxEta) return false;
  if (m_removeCrack && 1.37 <= eta && eta <= 1.52) return false;
  if (pt <= m_minElectronPt) return false;

  return m_mergedCutTools->accept(el);

}

bool DerivationFramework::SkimmingToolHIGG1::MuonPreselect(const xAOD::Muon *mu) const {

  if (!mu) return false;

  if(mu->isAvailable<char>("DFCommonGoodMuon"))
    if( !static_cast<bool>(mu->auxdata<char>("DFCommonGoodMuon")) )
      return false;
  
  if(mu->isAvailable<char>("DFCommonMuonsPreselection"))
    if( !static_cast<bool>(mu->auxdata<char>("DFCommonMuonsPreselection")) )
      return false;

  double eta = fabs(mu->eta());
  double pt = mu->pt();

  if (eta > m_maxMuonEta) return false;
  if (pt <= m_minMuonPt) return false;

  return true;

}

const double DerivationFramework::SkimmingToolHIGG1::s_MZ = 91187.6*CLHEP::MeV; 



