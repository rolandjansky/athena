/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  Tool used in EXOT15 derivation to include leptonic triggers into the derivation,
 *  but limiting the derivation size by asking for at least one trackless jet in the event.
 *
 *  Based on DerivationFramework::SkimmingToolExample
 *  Author: Paolo Sabatini (paolo.sabatini@cern.ch)
*/

#include "DerivationFrameworkExotics/SkimmingToolEXOT15.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "TLorentzVector.h"

namespace {
  constexpr float MeVtoGeV = 1.e-3;
  constexpr float GeVtoMeV = 1.e3;
}

/**
 *  Constructor of the skimming tool used in EXOT15.
 */

DerivationFramework::SkimmingToolEXOT15::SkimmingToolEXOT15(const std::string& t,
							    const std::string& n,
							    const IInterface* p) :
  AthAlgTool(t, n, p),
  m_ntot (0),
  m_npass(0) {

  declareInterface<DerivationFramework::ISkimmingTool>(this);
  
  declareProperty("JetContainer",            m_jetContainer = "AntiKt4EMTopoJets", "Jet collection used for the search of the trackless jet.");
  declareProperty("TrackContainer",          m_trkContainer = "InDetTrackParticles", "Track collection used for the search of the trackless jet");
  declareProperty("minDeltaR",               m_minDeltaR = 0.2, "Minimum dR between the trackless jet and the closest track.");
  declareProperty("minJetPt",                m_minJetPt = 25, "Minumum pT of the trackless jet.");
  declareProperty("jetEtaCut",               m_jetEtaCut = 2.5, "Eta range of the tracks used to flag the jet as trackless.");
  declareProperty("minTrackPt",              m_minTrackPt = 2, "Minimum pT of the tracks used in the search of the trackless jet.");
  declareProperty("cleanLLP",                m_cleanLLP = true, "Flag asking the trackless jet to pass BadLooseLLP criteria.");
  declareProperty("cutLeptonPt",             m_cutLeptonPt = false, "Flag asking a given lepton multiplicity.");
  declareProperty("minLeptonPt",             m_minLeptonPt = 15, "Minimum pT of the required leptons in the event.");
  declareProperty("minNLeptons",             m_minNLeptons = 1, "Mimumum multiplicity of leptons required in the event.");
  declareProperty("cutNMSRoIs",              m_cutNMSRoIs= false, "Flag asking a minimum multiplicity of MS RoIs in the event.");
  declareProperty("minNMSRoIs",              m_minNMSRoIs = 3, "Minimum multiplicity of MS RoIs in the event.");
  declareProperty("cutNMSVtx",               m_cutNMSVtx= false, "Flag asking a minimum multiplicity of MS vertices in the event.");
  declareProperty("minNMSVtx",               m_minNMSVtx = 1, "Minimum multiplicity of MS vertices in the event.");
  
}

/**
 *  Deconstructor of the EXOT15 skimming tool
 */

DerivationFramework::SkimmingToolEXOT15::~SkimmingToolEXOT15() {
}

/**
 *  Initialise the EXOT15 skimming tool. It only prints out the options of the tool.
 */

StatusCode DerivationFramework::SkimmingToolEXOT15::initialize()
{
  
  ATH_MSG_INFO("Initialising the tool..");
  ATH_MSG_INFO("Property jetContainer:\t "<< m_jetContainer );
  ATH_MSG_INFO("Property trkContainer:\t "<< m_trkContainer );
  ATH_MSG_INFO("Property minDeltar:   \t "<< m_minDeltaR );
  ATH_MSG_INFO("Property minJetPt:    \t "<< m_minJetPt);
  ATH_MSG_INFO("Property jetEtaCut:   \t "<< m_jetEtaCut);
  ATH_MSG_INFO("Property minTrackPt:  \t "<< m_minTrackPt);
  ATH_MSG_INFO("Property cleanLLP:    \t "<< m_cleanLLP);
  ATH_MSG_INFO("Property cutLeptonPt: \t "<< m_cutLeptonPt);
  ATH_MSG_INFO("Property minLeptonpT: \t "<< m_minLeptonPt);
  ATH_MSG_INFO("Property minNLeptons: \t "<< m_minNLeptons);
  ATH_MSG_INFO("Property cutNMSRoIs:  \t "<< m_cutNMSRoIs);
  ATH_MSG_INFO("Property minNMSRoIs:  \t "<< m_minNMSRoIs);
  ATH_MSG_INFO("Property cutNMSVtx:   \t "<< m_cutNMSVtx);
  ATH_MSG_INFO("Property minNMSVtx:   \t "<< m_minNMSVtx);

  return StatusCode::SUCCESS;
}

/**
 *  Finalise the EXOT15 skimming tool, printing summary info on total events passing the selection.
 */

StatusCode DerivationFramework::SkimmingToolEXOT15::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_ntot << " events, " << m_npass << " events passed filter ");

  return StatusCode::SUCCESS;
}

/**
 *  Real skimming function.
 *  It is based on three boolean flags: pass_cr, pass_ms_roi and pass_ms_vtx.
 *  - pass_cr: checks all the jet in the event and counts the number of trackless jets. If the flag cleanLLP is turned on, an
 *    isolation criteria is also required. If cutLeptonPt is set to true, cuts on minimum multiplicity of leptons with a pT above
 *    a given threshold are also applied.
 *  - pass_ms_roi: it requires the event to have at least minNMSRoI trigger MS RoI.
 *  - pass_ms_vtx: it requires the event to have at least minNMSVtx MS vertices.
 *  An event passes the skimming selection when:
 *  - pass_cr, if cutNMSRoIs and cutNMSVtx are set to false.
 *  - pass_cr || pass_ms_roi, if only cutNMSVtx is set to false.
 *  - pass_cr || pass_ms_vtx, if only cutNMSRoIs is set to false.
 *  - pass_cr || pass_ms_roi || pass_ms_vtx, otherwise.
 */

bool DerivationFramework::SkimmingToolEXOT15::eventPassesFilter() const
{
  m_ntot++;

  /*
   * Calculating pass_cr
   */

  const xAOD::JetContainer* jets = nullptr;
  ATH_CHECK(evtStore()->retrieve(jets, m_jetContainer));
  const xAOD::TrackParticleContainer* tracks = nullptr;
  ATH_CHECK(evtStore()->retrieve(tracks, m_trkContainer));
  
  int nj_trackless = 0;
  
  for ( auto j : *jets){
    int nt_in_dR = 0;
    if (j->p4().Pt() < m_minJetPt*GeVtoMeV) continue;
    if ( std::abs (j->p4().Eta()) > m_jetEtaCut) continue;
    for (auto t : *tracks){
      if (t->p4().Pt() < m_minTrackPt*GeVtoMeV) continue;
      float deltaR =  j->p4().DeltaR (t->p4());
      if (deltaR < m_minDeltaR) nt_in_dR ++;
    }

    /*
     * In case m_cleanLLP = true: the trackless jet is counted  is passes (!largeFMax && !poorQuality && !negE)
     */
    if (m_cleanLLP){
      /*
       * Reproduing the LooseBadLLP selection
       */

      static const SG::AuxElement::ConstAccessor<float> acc_fmax("FracSamplingMax");
      bool largeFMax = ( ( acc_fmax(*j) > 0.99 ) && ( std::abs (j->eta()) < 2. ) );
      if (largeFMax) continue;
      
      static const SG::AuxElement::ConstAccessor<float> acc_hecFrac("HECFrac");
      static const SG::AuxElement::ConstAccessor<float> acc_hecQ("HECQuality");
      static const SG::AuxElement::ConstAccessor<float> acc_avgLArQF("AverageLArQF");
      // 65535 = number of bits to express the LAr Quality factor 
      bool poorQuality = ( ( acc_hecFrac(*j) > 0.5 ) && ( std::abs (acc_hecQ(*j)) < 0.5 ) && ( acc_avgLArQF(*j)/65535. > 0.8 ) );
      if (poorQuality) continue;
            
      static const SG::AuxElement::ConstAccessor<float> acc_negE("NegativeE");
      bool negE = ( (std::abs(acc_negE(*j)*0.001) > 4) && (acc_fmax(*j) > 0.85) );
      if (negE) continue;

    }
    if (nt_in_dR==0) nj_trackless++;
  }

  // pass_cr = at least one trackless jet in the event with sufficient pT (and clean, in case m_cleanLLP activated)
  bool pass_cr = (nj_trackless!=0);

  /*
   * Modifying pass_cr in case further cuts on leptons are asked via cutLeptonPt flag.
   */

  if (m_cutLeptonPt){
    
    const xAOD::MuonContainer* muons = nullptr;
    ATH_CHECK(evtStore()->retrieve(muons, "Muons"));
    const xAOD::ElectronContainer* electrons = nullptr;
    ATH_CHECK(evtStore()->retrieve(electrons, "Electrons"));

    
    int n_leptons_over_threshold = 0;
    for (auto m : *muons) { if (m->pt() > m_minLeptonPt*GeVtoMeV) n_leptons_over_threshold++; }
    for (auto e : *electrons) { if (e->pt() > m_minLeptonPt*GeVtoMeV) n_leptons_over_threshold++; }
    if (n_leptons_over_threshold < m_minNLeptons) pass_cr = false;

  }

  /*
   * if pass_cr is satisfied: passes the condition
   */
  if (pass_cr) {
    m_npass++;  
    return true;
  }

  /*
   * Calculating pass_ms_roi
   */

  bool pass_ms_roi = false;
  if (m_cutNMSRoIs){
    const xAOD::MuonRoIContainer* l1muons = nullptr;
    ATH_CHECK(evtStore()->retrieve(l1muons, "LVL1MuonRoIs"));
    int n_ms_RoIs = l1muons->size();
    if (n_ms_RoIs >= m_minNMSRoIs) pass_ms_roi = true;
  }

  /*
   * if (pass_ms_roi && m_cutNMSRoIs) is satisfied: passes the condition
   */
  if (pass_ms_roi && m_cutNMSRoIs) {
    m_npass++;  
    return true;
  }

  /*
   * Calculating pass_ms_vtx
   */

  bool pass_ms_vtx = false;
  if (m_cutNMSVtx){
    const xAOD::VertexContainer* msvtx = nullptr;
    ATH_CHECK(evtStore()->retrieve(msvtx, "MSDisplacedVertex"));
    int n_ms_vtx = msvtx->size();
    if (n_ms_vtx >= m_minNMSVtx) pass_ms_vtx = true;
  }
 
  /*
   * if (pass_ms_roi && m_cutNMSRoIs) is satisfied: passes the condition
   */
  if (pass_ms_vtx && m_cutNMSVtx) {
    m_npass++;  
    return true;
  }

  /*
   * If arrived here: none of the previous conditions are passed, not passing
   */

  return false;

}
