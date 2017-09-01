/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Based on DerivationFramework::SkimmingToolExample

#include "DerivationFrameworkExotics/SkimmingToolEXOT14.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEventInfo/EventInfo.h"
// #include "xAODTracking/TrackingPrimitives.h"

// #include "JetCalibTools/JetCalibrationTool.h"


// Constructor
DerivationFramework::SkimmingToolEXOT14::SkimmingToolEXOT14(const std::string& t,
							    const std::string& n,
							    const IInterface* p) : 
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_JESTool("JetCalibrationTool/m_JESTool"),
  n_tot(0),
  n_passGRL(0),
  n_passLArError(0),
  n_passTrigger(0),
  n_passPreselect(0),
  n_passJetPts(0),
  n_passJetsDEta(0),
  n_passDiJetMass(0),
  n_passJetsDPhi(0),
  n_pass(0)
{

  declareInterface<DerivationFramework::ISkimmingTool>(this);

  declareProperty("JetContainer",          m_jetSGKey = "AntiKt4EMTopoJets");

  declareProperty("RequireGRL",            m_reqGRL          = true);
  declareProperty("ReqireLArError",        m_reqLArError     = true);
  declareProperty("RequireTrigger",        m_reqTrigger      = true);
  declareProperty("RequirePreselection",   m_reqPreselection = true);
  declareProperty("RequireJetPts",         m_reqJetPts       = true);
  declareProperty("RequireJetsDEta",       m_reqJetsDEta     = true);
  declareProperty("RequireDiJetMass",      m_reqDiJetMass    = true);
  declareProperty("RequireJetsDPhi",       m_reqJetsDPhi     = true);

  declareProperty("GoodRunList",           m_goodRunList = "");

  declareProperty("DefaultTrigger",        m_defaultTrigger = "HLT_xe100");
  declareProperty("Triggers",              m_triggers = std::vector<std::string>()); 

  declareProperty("MinimumJetPt",          m_minJetPt = 25*CLHEP::GeV);
  declareProperty("MaxEta",                m_maxEta = 4.8);

  declareProperty("LeadingJetPtCut",       m_leadingJetPt    = 75*CLHEP::GeV);
  declareProperty("SubleadingJetPtCut",    m_subleadingJetPt = 50*CLHEP::GeV);

  declareProperty("EtaSeparation",         m_etaSeparation = 4.8);

  declareProperty("DiJetsMass",            m_dijetMass = 1000*CLHEP::GeV);

  declareProperty("DiJetDPhi",             m_jetDPhi = 2.5);

  m_isMC = true;
  j1TLV = TLorentzVector();
  j2TLV = TLorentzVector();

  e_passGRL = false;
  e_passLArError = false;
  e_passTrigger = false;
  e_passPreselect = false;
  e_passJetPts = false;
  e_passJetsDEta = false;
  e_passDiJetMass = false;
  e_passJetsDPhi = false;
  e_pass = false;

  e_JetsDEta = 0;
  e_DiJetMass = 0;
  e_JetsDPhi = 999;
}
  
// Destructor
DerivationFramework::SkimmingToolEXOT14::~SkimmingToolEXOT14() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolEXOT14::initialize()
{
  ATH_MSG_VERBOSE("INITIALIZING VBFINV SELECTOR TOOL");

  ////////////////////////////
  // trigger decision tool
  if(m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
    return StatusCode::FAILURE;
  }
  if (!m_triggers.size()) m_triggers.push_back(m_defaultTrigger);
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);
  ////////////////////////////
  

  ////////////////////////////
  // jet energy calibration
  m_JESTool.setTypeAndName("JetCalibrationTool/m_JESTool");
  CHECK( m_JESTool.retrieve() ); //optional, just forces initializing the tool here instead of at first use
  ATH_MSG_INFO("Retrieved tool: " << m_JESTool);
  ////////////////////////////


  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::SkimmingToolEXOT14::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << n_tot << " events, " << n_pass << " events passed filter ");

  ATH_MSG_INFO("GRL       :: " << n_passGRL);
  ATH_MSG_INFO("LAr Error :: " << n_passLArError);
  ATH_MSG_INFO("Trigger   :: " << n_passTrigger);
  ATH_MSG_INFO("Preselect :: " << n_passPreselect);
  ATH_MSG_INFO("JetPts    :: " << n_passJetPts);
  ATH_MSG_INFO("JetsDEta  :: " << n_passJetsDEta);
  ATH_MSG_INFO("DijetMass :: " << n_passDiJetMass);
  ATH_MSG_INFO("JetsDPhi  :: " << n_passJetsDPhi);

  return StatusCode::SUCCESS;

}

// The filter itself
bool DerivationFramework::SkimmingToolEXOT14::eventPassesFilter() const
{

  n_tot++;

  bool writeEvent(false);

  const xAOD::EventInfo *eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo));
  m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);   

  // int *leading    = new int(0);
  // if (!evtStore()->contains<int>("leading"))    CHECK(evtStore()->record(leading,    "leading"));

  if (!SubcutGoodRunList() && m_reqGRL      ) return false;
  if (!SubcutLArError()    && m_reqLArError ) return false;
  if (!SubcutTrigger()     && m_reqTrigger  ) return false;

  SubcutPreselect();
  if (!m_reqPreselection) writeEvent = true;	    

  // There *must* be two jets for the remaining 
  // pieces, but you can still save the event...
  if (e_passPreselect) {

    bool passDiJets(true);     
    if (!SubcutJetPts()        && m_reqJetPts   ) passDiJets = false;
    if (!SubcutJetDEta()       && m_reqJetsDEta ) passDiJets = false;
    if (!SubcutDijetMass()     && m_reqDiJetMass) passDiJets = false;
    if (!SubcutJetDPhi()       && m_reqJetsDPhi ) passDiJets = false;
    if (passDiJets) writeEvent = true; 
  }

  if (!writeEvent) return false;
  
  n_pass++;
  return true;

}

bool DerivationFramework::SkimmingToolEXOT14::SubcutGoodRunList() const {

  // Placeholder

  e_passGRL = true;
  
  if (e_passGRL) n_passGRL++;
  return e_passGRL;

}

bool DerivationFramework::SkimmingToolEXOT14::SubcutLArError() const {

  // Retrieve EventInfo
  const xAOD::EventInfo *eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo)); 

  e_passLArError = !(eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error);
  
  if (e_passLArError) n_passLArError++;
  return e_passLArError;

}

bool DerivationFramework::SkimmingToolEXOT14::SubcutTrigger() const {

  const xAOD::EventInfo *eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo));

  e_passTrigger = false;

  for (unsigned int i = 0; i < m_triggers.size(); i++) {
    bool thisTrig = m_trigDecisionTool->isPassed(m_triggers.at(i));
    eventInfo->auxdecor< bool >(TriggerVarName(m_triggers.at(i))) = thisTrig;
    // ATH_MSG_INFO("TRIGGER = " << m_triggers.at(i) <<  " -->> " << thisTrig);
    e_passTrigger |= thisTrig;
  }
  
  //  temporary pass-through of trigger cut for MC
  if (m_isMC) e_passTrigger = true;

  if (e_passTrigger) n_passTrigger++;
  return e_passTrigger;

}

bool DerivationFramework::SkimmingToolEXOT14::SubcutPreselect() const {

  // xAOD::TStore store;
  const xAOD::JetContainer *jets(0); 
  ATH_CHECK(evtStore()->retrieve(jets, m_jetSGKey));
  xAOD::JetContainer::const_iterator jet_itr(jets->begin());
  xAOD::JetContainer::const_iterator jet_end(jets->end());

  xAOD::Jet* jetC = 0; // new xAOD::Jet();
  j1TLV.SetPtEtaPhiE(0, 0, 0, 0);
  j2TLV.SetPtEtaPhiE(0, 0, 0, 0);

  TLorentzVector jFirst;
  jFirst.SetPtEtaPhiE(0,0,0,0); //Just take the first jet that passes as a reference
  bool filledFirstJet=false;

  for(int i = 0; jet_itr != jet_end; ++jet_itr, ++i) {

    jetC = 0;
    CP::CorrectionCode cc = m_JESTool->calibratedCopy(**jet_itr, jetC);

    if (abs(jetC->eta()) > m_maxEta) { delete jetC; continue; }
    // ATH_MSG_INFO("jet uncalib=" << (*jet_itr)->pt() << "   calib=" << jetC->pt());

    if (jetC->pt() > j1TLV.Pt()) {

      j2TLV = j1TLV;
      j1TLV.SetPtEtaPhiE(jetC->pt(), jetC->eta(), jetC->phi(), jetC->e());

    } else if (jetC->pt() > j2TLV.Pt()) {

      j2TLV.SetPtEtaPhiE(jetC->pt(), jetC->eta(), jetC->phi(), jetC->e());
    }
    
    if(filledFirstJet){
      //The first time through, this is always false so it won't be entered
      TLorentzVector e_tmp; //setup a temporary 4-vector
      e_tmp.SetPtEtaPhiE(jetC->pt(), jetC->eta(), jetC->phi(), jetC->e());
      //compute variables
      float tmpDEta = fabs(jFirst.Eta()-e_tmp.Eta());
      float tmpJetMass = (jFirst+e_tmp).M();
      float tmpJetDphi = fabs(jFirst.DeltaPhi(e_tmp));

      if(tmpDEta > e_JetsDEta) e_JetsDEta = tmpDEta;
      if(tmpJetMass > e_DiJetMass) e_DiJetMass = tmpJetMass;
      if(tmpJetDphi < e_JetsDPhi) e_JetsDPhi = tmpJetDphi;

    }

    if(!filledFirstJet){
      jFirst.SetPtEtaPhiE(jetC->pt(), jetC->eta(), jetC->phi(), jetC->e());
      filledFirstJet=true;//Now we filled the jet, so say so
    }

    delete jetC;
  }

  // save this for this code.
  if (j2TLV.Pt() > m_minJetPt) {
    e_passPreselect = true;
    n_passPreselect++;
    return true;
  }

  e_passPreselect = false;
  return false;

}

bool DerivationFramework::SkimmingToolEXOT14::SubcutJetPts() const {

  // ATH_MSG_INFO("j1_pt=" << j1TLV.Pt() << "  min=" << m_leadingJetPt);
  // ATH_MSG_INFO("j2_pt=" << j2TLV.Pt() << "  min=" << m_subleadingJetPt);

  e_passJetPts =  (!m_leadingJetPt    || j1TLV.Pt() > m_leadingJetPt);
  e_passJetPts &= (!m_subleadingJetPt || j2TLV.Pt() > m_subleadingJetPt);

  if (e_passJetPts) n_passJetPts++;
  return e_passJetPts;

}

bool DerivationFramework::SkimmingToolEXOT14::SubcutJetDEta() const {

  //e_JetsDEta = fabs(j1TLV.Eta() - j2TLV.Eta());
  //ATH_MSG_INFO("deta=" << e_JetsDEta << "  min=" << m_etaSeparation);

  e_passJetsDEta = e_JetsDEta > m_etaSeparation;

  if (e_passJetsDEta) n_passJetsDEta++;
  return e_passJetsDEta;

}

bool DerivationFramework::SkimmingToolEXOT14::SubcutDijetMass() const {

  //e_DiJetMass = (j1TLV + j2TLV).M();
  // ATH_MSG_INFO("mass=" << e_DiJetMass << "  min=" << m_dijetMass);

  e_passDiJetMass = e_DiJetMass > m_dijetMass;

  if (e_passDiJetMass) n_passDiJetMass++;
  return e_passDiJetMass;

}

bool DerivationFramework::SkimmingToolEXOT14::SubcutJetDPhi() const {

  //e_JetsDPhi = fabs(j1TLV.DeltaPhi(j2TLV));
  // ATH_MSG_INFO("dphi=" << e_JetsDPhi << "  max=" << m_jetDPhi);

  e_passJetsDPhi = e_JetsDPhi < m_jetDPhi;

  if (e_passJetsDPhi) n_passJetsDPhi++;
  return e_passJetsDPhi;

}

std::string DerivationFramework::SkimmingToolEXOT14::TriggerVarName(std::string s) const {
  std::replace(s.begin(), s.end(), '-', '_'); return s;
}
