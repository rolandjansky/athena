/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloTransverseBalanceVecMon.cxx
//
// AUTHORS:   FENG TIAN (COLUMBIA UNIVERSITY) 
//
// Modified: L. Sawyer 12/2013 Added new managed histograms.
//********************************************************************
              

#include "CaloTransverseBalanceVecMon.h"

#include "xAODEventInfo/EventInfo.h"

#include "JetEvent/JetCollection.h"

#include "egammaEvent/Photon.h"

#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <functional>
#include <set>
#include <vector>

#define PI 3.14159265
#define GeV 1000

void CaloTransverseBalanceVecMon::findLeadingPhoton(const PhotonContainer* userPhotonContainer,PhotonContainer::const_iterator& leadingPhPr){
  PhotonContainer::const_iterator photonItr;
  float ptmax=-1;
  for(photonItr=userPhotonContainer->begin();photonItr<userPhotonContainer->end();photonItr++){
   if((*photonItr)->pt()>ptmax) {ptmax=(*photonItr)->pt();leadingPhPr= photonItr;}
  }
}
void CaloTransverseBalanceVecMon::findleadingANDsubleadingjets
  (const std::vector<const Jet*>& userJetContainer,
   Jet const* & leadingJetPr,
   Jet const* & subleadingJetPr)
{
  std::vector<const Jet*>::const_iterator jetItr;
  float ptmax=-1;
  float ptsubmax=-2;
  for(jetItr=userJetContainer.begin();jetItr<userJetContainer.end();jetItr++){
   if((*jetItr)->pt()>ptmax){
     ptsubmax=ptmax;
     subleadingJetPr=leadingJetPr;
     ptmax=(*jetItr)->pt();
     leadingJetPr=*jetItr;
   }
   else if((*jetItr)->pt()>ptsubmax) {ptsubmax=(*jetItr)->pt();subleadingJetPr=*jetItr;}
  }
}
void CaloTransverseBalanceVecMon::getmissingEt_phi(float missingEx,float missingEy,float& missingEt_phi_t){
  if(missingEy>0 && missingEx>0){
   missingEt_phi_t = atan(missingEy/missingEx);
  }
  if(missingEy>0 && missingEx<0){
   missingEt_phi_t = PI - atan(fabs(missingEy/missingEx));
  }
  if(missingEy<0 && missingEx>0){
   missingEt_phi_t = atan(missingEy/missingEx);
  }
  if(missingEy<0 && missingEx<0){
   missingEt_phi_t = atan(fabs(missingEy/missingEx)) - PI;
  }
  if(missingEy==0 && missingEx){
   if(missingEx>0) missingEt_phi_t = 0;
   else missingEt_phi_t = -PI;
  }
  if(missingEx==0 && missingEy){
   if(missingEy>0) missingEt_phi_t = PI/2;
   else missingEt_phi_t = -PI/2;
  }
  if(missingEx==0 && missingEy==0) missingEt_phi_t=-999;
}
void CaloTransverseBalanceVecMon::AdjustPhi(float& deltaphi){
 if(deltaphi > PI) deltaphi = 2*PI - deltaphi;
}

CaloTransverseBalanceVecMon::CaloTransverseBalanceVecMon(const std::string& type, const std::string& name,
				     const IInterface* parent) 
  : ManagedMonitorToolBase(type, name, parent),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_userPhotonContainer(nullptr),
    m_pMissing(nullptr),
    m_theTransverseBalance_shift(nullptr),
    m_cutflow_j_1(0),
    m_cutflow_j_2(0),
    m_cutflow_j_3(0),
    m_TriggerCut(0),
    m_h_photon_pt(nullptr),
    m_h_photon_eta(nullptr),
    m_h_photon_phi(nullptr),
    m_h_jet_pt_beforeoverlap(nullptr),
    m_h_jet_eta_beforeoverlap(nullptr),
    m_h_jet_phi_beforeoverlap(nullptr),
    m_h_jet_pt_afteroverlap(nullptr),
    m_h_jet_eta_afteroverlap(nullptr),
    m_h_jet_phi_afteroverlap(nullptr),
    m_h_njet_beforeoverlap(nullptr),
    m_h_njet_afteroverlap(nullptr),
    m_h_leadingJet_pt(nullptr),
    m_h_leadingJet_eta(nullptr),
    m_h_leadingJet_phi(nullptr),
    m_h_deltaPhi(nullptr),
    m_h2_ph_leadingJet_eta(nullptr),
    m_p_ptratioVsPhi_Barrel(nullptr),
    m_p_ptratioVsPhi_ECC(nullptr),
    m_p_ptratioVsPhi_ECA(nullptr),
    m_p_ptratioVsEta(nullptr),
    m_h2_ptratioVsPhi_Barrel(nullptr),
    m_h2_ptratioVsPhi_ECC(nullptr),
    m_h2_ptratioVsPhi_ECA(nullptr),
    m_h2_ptratioVsEta(nullptr),
    m_h_overlapped_ph_jet_ptratio(nullptr),
    m_h_pt_ratio(nullptr),
    m_h_leadingPh_MET_deltaPhi(nullptr),
    m_h2_leadingPh_MET_deltaPhiVsPt(nullptr)
{

  // Time granularity options -  global by default
   declareProperty("TimeGran",m_timeGran="lowStat");
  //switch on/off for algorithm LArNoisyAlg used to remove events containing large noise bursts in EMEC
  declareProperty("Calo_LArNoisyAlg",m_useLArNoisyAlg=false);

  /////////////////////////////////////////////////////////////////////////////////////
  declareProperty("photonPTCut", m_photonPtCut=20*GeV);
  declareProperty("PhotonContainer", m_PhotonContainerName="PhotonCollection");
  declareProperty("JetContainer",m_JetContainerName="Cone4H1TopoJets");
    /// Missing ET options
  declareProperty("MissingEtObject",m_missingEtObjectName = "MET_Calib");
 
 
}

CaloTransverseBalanceVecMon::~CaloTransverseBalanceVecMon() {}


StatusCode CaloTransverseBalanceVecMon::initialize() {
  
  ATH_CHECK(  m_trigDec.retrieve() );

  m_cutflow_j_1 = 0;
  m_cutflow_j_2 = 0;
  m_cutflow_j_3 = 0;
  m_TriggerCut=false; 
  ManagedMonitorToolBase::initialize().ignore();
  return StatusCode::SUCCESS;
}
////////////////////////////////////////////////////////////////////////////
StatusCode CaloTransverseBalanceVecMon::bookHistogramsRecurrent(){
//  Book old-style (unmanaged) histograms. Not used.

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloTransverseBalanceVecMon::bookHistograms(){

  ATH_MSG_DEBUG( "in bookHistograms()"  );

   //-------- Time Granularity---------------

   // Modified 12/2013 LS: New ATHENA monitoring code allow for managed histograms.
   //                      No longer need to check time granularity.
   Interval_t theinterval=run;

   //  use MMTB trigger awareness
   std::string TheTrigger;
   if (m_triggerChainProp == "")  TheTrigger="NoTrigSel";
   else TheTrigger = m_triggerChainProp;

   m_theTransverseBalance_shift= new MonGroup( this, "/CaloMonitoring/TransverseBalance/", theinterval);

   ATH_MSG_DEBUG( "in bookHistograms()"  );

   m_h_photon_pt    = new TH1F("h_photon_pt","h_photon_pt",100,0,100000);
   m_h_photon_pt->GetXaxis()->SetTitle("pt(MeV)" );
   m_h_photon_eta   = new TH1F("h_photon_eta","h_photon_eta",60,-3,3);
   m_h_photon_phi   = new TH1F("h_photon_phi","h_photon_phi",70,-3.5,3.5);
   m_h_jet_pt_beforeoverlap = new TH1F("jet_pt_beforeoverlap","jet_pt",250,0,1000*GeV);
   m_h_jet_pt_beforeoverlap->SetXTitle("jet pt(MeV)"); 
   m_h_jet_eta_beforeoverlap = new TH1F("h_jet_eta_beforeoverlap","jet_eta",60,-3,3);
   m_h_jet_eta_beforeoverlap->SetXTitle("eta");
   m_h_jet_phi_beforeoverlap = new TH1F("h_jet_phi_beforeoverlap","jet_phi",70,-3.5,3.5);
   m_h_jet_phi_beforeoverlap->SetXTitle("phi"); 
   m_h_jet_pt_afteroverlap = new TH1F("jet_pt_afteroverlap","jet_pt",250,0,1000*GeV);
   m_h_jet_pt_afteroverlap->SetXTitle("jet pt(MeV)");
   m_h_jet_eta_afteroverlap = new TH1F("h_jet_eta_afteroverlap","jet_eta",60,-3,3);
   m_h_jet_eta_afteroverlap->SetXTitle("eta");
   m_h_jet_phi_afteroverlap = new TH1F("h_jet_phi_afteroverlap","jet_phi",70,-3.5,3.5);
   m_h_jet_phi_afteroverlap->SetXTitle("phi");
   m_h_njet_beforeoverlap = new TH1I("njet_beforeoverlap","njet before overlap",20,0,20);
   m_h_njet_beforeoverlap->SetXTitle("# of jet");
   m_h_njet_afteroverlap = new TH1I("njet_afteroverlap","njet after overlap",20,0,20);
   m_h_njet_afteroverlap->SetXTitle("# of jet");
   m_h_leadingJet_pt = new TH1F("h_leadingJet_pt","h_leadingJet_pt",200,0,200*GeV);
   m_h_leadingJet_pt->SetXTitle("leading jet pt");
   m_h_leadingJet_eta = new TH1F("h_leadingJet_eta","h_leadingJet_eta",60,-3,3);
   m_h_leadingJet_eta->SetXTitle("leading jet eta");
   m_h_leadingJet_phi = new TH1F("h_leadingJet_phi","h_leadingJet_phi",70,-3.5,3.5);
   m_h_leadingJet_phi->SetXTitle("leading jet phi");

   // photon & jet
   m_h_deltaPhi = new TH1F("h_deltaPhi","deltaPhi between ph and jet",40,0,4);
   m_h_deltaPhi->SetXTitle("delta phi");
   m_h2_ph_leadingJet_eta = new TH2F("h2_ph_leadingJet_eta","h2_ph_leadingJet_eta",80,-4,4,80,-4,4);
   m_p_ptratioVsPhi_Barrel = new TProfile("p_ptratioVsPhi_Barrel","p_ptratioVsPhi_Barrel",30,-1.5,1.5);
   m_p_ptratioVsPhi_ECC = new TProfile("p_ptratioVsPhi_ECC","p_ptratioVsPhi_ECC",20,1,3);
   m_p_ptratioVsPhi_ECA = new TProfile("p_ptratioVsPhi_ECA","p_ptratioVsPhi_ECA",20,-3,-1);
   m_p_ptratioVsEta = new TProfile("p_ptratioVsEta","p_ptratioVsEta",60,-3,3);
   m_h2_ptratioVsPhi_Barrel = new TH2F("h2_ptratioVsPhi_Barrel","h2_ptratioVsPhi_Barrel",30,-1.5,1.5,30,0,3);
   m_h2_ptratioVsPhi_ECA = new TH2F("h2_ptratioVsPhi_ECA","h2_ptratioVsPhi_ECA",20,-3,1,30,0,3);
   m_h2_ptratioVsPhi_ECC = new TH2F("h2_ptratioVsPhi_ECC","h2_ptratioVsPhi_ECC",20,1,3,30,0,3);
   m_h2_ptratioVsEta = new TH2F("h2_ptratioVsEta","h2_ptratioVsEta",60,-3,3,30,0,3);
   m_h_overlapped_ph_jet_ptratio = new TH1F("h_overlapped_ph_jet_ptratio","h_overlapped_ph_jet_ptratio",20,0,2);
   m_h_leadingPh_MET_deltaPhi = new TH1F("h_leadingPh_MET_deltaPhi","leadingPh_MET_deltaPhi",40,0,4);
   m_h2_leadingPh_MET_deltaPhiVsPt = new TH2F("h2_leadingPh_MET_deltaPhiVsPt","leadingPh_MET_deltaPhiVsPt",100,0,200*GeV,40,0,4);
   m_h_pt_ratio = new TH1F("h_pt_ratio","ph_pt/jet_pt",30,0,3);
   m_h_pt_ratio->SetXTitle("pt ratio");

   Plotregister(m_theTransverseBalance_shift,m_h_photon_pt);
   Plotregister(m_theTransverseBalance_shift,m_h_photon_eta);
   Plotregister(m_theTransverseBalance_shift,m_h_photon_phi);
   Plotregister(m_theTransverseBalance_shift,m_h_jet_pt_beforeoverlap);
   Plotregister(m_theTransverseBalance_shift,m_h_jet_eta_beforeoverlap);
   Plotregister(m_theTransverseBalance_shift,m_h_jet_phi_beforeoverlap);
   Plotregister(m_theTransverseBalance_shift,m_h_jet_pt_afteroverlap);
   Plotregister(m_theTransverseBalance_shift,m_h_jet_eta_afteroverlap);
   Plotregister(m_theTransverseBalance_shift,m_h_jet_phi_afteroverlap);
   Plotregister(m_theTransverseBalance_shift,m_h_njet_beforeoverlap);
   Plotregister(m_theTransverseBalance_shift,m_h_njet_afteroverlap);
   Plotregister(m_theTransverseBalance_shift,m_h_leadingJet_pt);
   Plotregister(m_theTransverseBalance_shift,m_h_leadingJet_eta);
   Plotregister(m_theTransverseBalance_shift,m_h_leadingJet_phi);
   Plotregister(m_theTransverseBalance_shift,m_h_deltaPhi);
   Plotregister(m_theTransverseBalance_shift,m_h2_ph_leadingJet_eta);
   Plotregister(m_theTransverseBalance_shift,m_p_ptratioVsPhi_Barrel);
   Plotregister(m_theTransverseBalance_shift,m_p_ptratioVsPhi_ECC);
   Plotregister(m_theTransverseBalance_shift,m_p_ptratioVsPhi_ECA);
   Plotregister(m_theTransverseBalance_shift,m_p_ptratioVsEta);
   Plotregister(m_theTransverseBalance_shift,m_h2_ptratioVsPhi_Barrel);
   Plotregister(m_theTransverseBalance_shift,m_h2_ptratioVsPhi_ECC);
   Plotregister(m_theTransverseBalance_shift,m_h2_ptratioVsPhi_ECA);
   Plotregister(m_theTransverseBalance_shift,m_h2_ptratioVsEta);
   Plotregister(m_theTransverseBalance_shift,m_h_overlapped_ph_jet_ptratio);
   Plotregister(m_theTransverseBalance_shift,m_h_leadingPh_MET_deltaPhi);
   Plotregister(m_theTransverseBalance_shift,m_h2_leadingPh_MET_deltaPhiVsPt);
   Plotregister(m_theTransverseBalance_shift,m_h_pt_ratio);
    
    
   return StatusCode::SUCCESS;
}
////////////////////////////////////////////////////////////////
// Execute - called by the event loop on event by event

StatusCode CaloTransverseBalanceVecMon::fillHistograms() {
  
  ATH_MSG_DEBUG( "fillHistograms()"  );

  const xAOD::EventInfo* thisEventInfo = nullptr;
  ATH_CHECK(  evtStore()->retrieve(thisEventInfo) );

  if(m_useLArNoisyAlg && (thisEventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error)){
    //      ATH_MSG_WARNING( "Event flagged as bursty by LArNoisyRO"  );
    return StatusCode::SUCCESS;
  }

 //EventID* myEventID = eventInfo->event_ID();
 // int lb = myEventID->lumi_block();
  
    /// get the photon AOD container from StoreGate
  const PhotonContainer* photonTES;
  StatusCode sc=evtStore()->retrieve( photonTES,m_PhotonContainerName);
  if( sc.isFailure()  ||  !photonTES ) {
    ATH_MSG_WARNING( "No ESD photon container found in TDS" );
     return StatusCode::SUCCESS;
  }
  
  const JetCollection* jetTES;
  sc = evtStore()->retrieve( jetTES,m_JetContainerName);
  if( sc.isFailure()  ||  !photonTES ) {
    ATH_MSG_WARNING( "No ESD jet collection found in TDS" );
    return StatusCode::SUCCESS;
  }
   /// retrieve the missing Et object from TDS
  sc = evtStore()->retrieve(m_pMissing,m_missingEtObjectName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve MissingET Object"  );
    return StatusCode::SUCCESS;
   }
  float missingEt_phi=0;
  getmissingEt_phi(m_pMissing->etx(),m_pMissing->ety(),missingEt_phi);
  // trigger cut 
  m_TriggerCut = m_trigDec->isPassed("L1_EM14");
  //  ATH_MSG_INFO( "*****************   trigger: " <<m_TriggerCut );
  if(!m_TriggerCut) return StatusCode::SUCCESS;

  m_userPhotonContainer = new ConstDataVector<PhotonContainer>(SG::VIEW_ELEMENTS);
  std::vector<const Jet*> userJetContainer;
  PhotonContainer::const_iterator photonItrB  = photonTES->begin();
  PhotonContainer::const_iterator photonItrE = photonTES->end();
  PhotonContainer::const_iterator photonItr;
  JetCollection::const_iterator jetItrB = jetTES->begin();
  JetCollection::const_iterator jetItrE = jetTES->end();
  JetCollection::const_iterator jetItr;

  bool flag_j_1 =false;
  bool flag_j_2 =false;
  bool flag_j_3 =false;
  int njet_c_beforeoverlap= 0;
  int njet_c_afteroverlap = 0;
  for(photonItr=photonItrB;photonItr!=photonItrE;photonItr++){
    float ph_pt = (*photonItr)->pt();
    float ph_eta = (*photonItr)->eta();
    float ph_phi = (*photonItr)->phi();
    bool  ph_isTight = ((*photonItr)->isem(egammaPIDObs::PhotonTight)==0);
 //   if( ph_pt <  m_photonPtCut)  continue; 
    if( !(fabs(ph_eta)<2.47 && (fabs(ph_eta)>=1.52 || fabs(ph_eta)<=1.37))) continue;
    if( !ph_isTight) continue;
    m_userPhotonContainer->push_back( *photonItr );
    m_h_photon_pt->Fill(ph_pt);
    m_h_photon_eta->Fill(ph_eta);
    m_h_photon_phi->Fill(ph_phi);
  }
  if(m_userPhotonContainer->size()==0) return StatusCode::SUCCESS;
  //find leading photon
  PhotonContainer::const_iterator leadingPhPr;
  if(m_userPhotonContainer->size()>1){
    findLeadingPhoton(m_userPhotonContainer->asDataVector(),leadingPhPr);
  }
  else leadingPhPr=m_userPhotonContainer->begin();

  //****************   jet selection   ***************
  for(jetItr=jetItrB;jetItr<jetItrE;jetItr++){
    float jet_pt = (*jetItr)->pt();
    float jet_eta= (*jetItr)->eta();
    float jet_phi =(*jetItr)->phi();
    bool j_cut1 = (fabs(jet_eta) <2.47 && (fabs(jet_eta)>=1.8 || fabs(jet_eta)<=1.3));
    bool j_cut2 = (jet_pt >15*GeV);
    if(!j_cut1) continue;
    flag_j_1 = true;
    if(!j_cut2) continue;
    flag_j_2=true;
    njet_c_beforeoverlap++;
    m_h_jet_pt_beforeoverlap->Fill(jet_pt);
    m_h_jet_eta_beforeoverlap->Fill(jet_eta);
    m_h_jet_phi_beforeoverlap->Fill(jet_phi);
    bool flag_overlap=false;
    float GptJptRatio = (*leadingPhPr)->pt()/jet_pt;
    float ph_j_deltaR = deltaR(jet_eta,jet_phi,(*leadingPhPr)->eta(),(*leadingPhPr)->phi());
    if(ph_j_deltaR<0.4){
     if(GptJptRatio>0.6 ) flag_overlap = true;
     if(GptJptRatio<0.6) m_userPhotonContainer->erase(m_userPhotonContainer->begin(),m_userPhotonContainer->end());
    }
    if(flag_overlap) continue;
    flag_j_3 = true;
    njet_c_afteroverlap++;
    m_h_jet_pt_afteroverlap->Fill(jet_pt);
    m_h_jet_eta_afteroverlap->Fill(jet_eta);
    m_h_jet_phi_afteroverlap->Fill(jet_phi);
    userJetContainer.push_back(*jetItr);
  }  
  if(flag_j_1) m_cutflow_j_1++;
  if(flag_j_2) m_cutflow_j_2++;
  if(flag_j_3) m_cutflow_j_3++;
  m_h_njet_beforeoverlap->Fill(njet_c_beforeoverlap);
  m_h_njet_afteroverlap->Fill(njet_c_afteroverlap);

  if(userJetContainer.size()==0) return StatusCode::SUCCESS;

  const Jet* leadingJetPr = nullptr;
  const Jet* subleadingJetPr = nullptr;
  if(userJetContainer.size()>1) 
    findleadingANDsubleadingjets(userJetContainer,leadingJetPr,subleadingJetPr);
  else leadingJetPr = userJetContainer.front(); // only 1 jet in userJetContainer
  if (leadingJetPr) {
    m_h_leadingJet_pt ->Fill(leadingJetPr->pt());
    m_h_leadingJet_eta ->Fill(leadingJetPr->eta());
    m_h_leadingJet_phi->Fill(leadingJetPr->phi());
  }
 // leading photon and leading jet study
 if(m_userPhotonContainer->size()>0 && userJetContainer.size()>0 && leadingJetPr){
  float pt_ratio = (*leadingPhPr)->pt() / leadingJetPr->pt(); 
  m_h_pt_ratio ->Fill(pt_ratio);
  float ph_jet_deltaPhi = fabs((*leadingPhPr)->phi() - leadingJetPr->phi());
  AdjustPhi(ph_jet_deltaPhi);
  m_h_deltaPhi ->Fill(ph_jet_deltaPhi);
  m_h2_ph_leadingJet_eta->Fill((*leadingPhPr)->eta(),leadingJetPr->eta());
 if(fabs( (*leadingPhPr)->eta())<1.37){
  m_p_ptratioVsPhi_Barrel->Fill((*leadingPhPr)->phi(),pt_ratio);
  m_h2_ptratioVsPhi_Barrel->Fill((*leadingPhPr)->phi(),pt_ratio);
 }   
 else if((*leadingPhPr)->eta()>1.52){
        m_p_ptratioVsPhi_ECA ->Fill((*leadingPhPr)->phi(),pt_ratio);
        m_h2_ptratioVsPhi_ECA->Fill((*leadingPhPr)->phi(),pt_ratio);
       }
      else {
            m_p_ptratioVsPhi_ECC ->Fill((*leadingPhPr)->phi(),pt_ratio);
            m_h2_ptratioVsPhi_ECC->Fill((*leadingPhPr)->phi(),pt_ratio);
           }
 m_p_ptratioVsEta->Fill((*leadingPhPr)->eta(),pt_ratio); 
 m_h2_ptratioVsEta->Fill((*leadingPhPr)->eta(),pt_ratio);
 float leadingPh_MET_deltaPhi = fabs(missingEt_phi- (*leadingPhPr)->phi());
  AdjustPhi(leadingPh_MET_deltaPhi);
  m_h_leadingPh_MET_deltaPhi->Fill(leadingPh_MET_deltaPhi);
  m_h2_leadingPh_MET_deltaPhiVsPt->Fill((*leadingPhPr)->pt(),leadingPh_MET_deltaPhi);
 }
   
  return sc;
}

StatusCode CaloTransverseBalanceVecMon::procHistograms( ) {
  StatusCode sc = StatusCode::SUCCESS;
  return sc;
}

float CaloTransverseBalanceVecMon::deltaR(float a_eta,float a_phi,float b_eta,float b_phi){
 float result;
 result = sqrt(pow((a_eta-b_eta),2)+pow((a_phi-b_phi),2));
 return result;
}

void CaloTransverseBalanceVecMon::Plotregister(MonGroup* monitorGroup,TH1F* hist){
  monitorGroup->regHist(hist).ignore();
}
void CaloTransverseBalanceVecMon::Plotregister(MonGroup* monitorGroup,TH1I* hist){
  monitorGroup->regHist(hist).ignore();
}   
void CaloTransverseBalanceVecMon::Plotregister(MonGroup* monitorGroup,TH2F* hist){
  monitorGroup->regHist(hist).ignore();
}

void CaloTransverseBalanceVecMon::Plotregister(MonGroup* monitorGroup,TProfile* hist){
  monitorGroup->regHist(hist).ignore();
}
