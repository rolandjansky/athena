// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/TopoInputEvent.h"
#include <fstream>
#include <pthread.h>

using namespace TCS;

//Current input list size for gFEX tobs and jEM might be not accurate 
TopoInputEvent::TopoInputEvent() :
  TrigConfMessaging("TopoInputEvent"),
  m_clusters("InputClusters",120),
  m_eEms("InputeEms",120),
  m_jEms("InputjEms",60),
  m_eTaus("InputeTaus",120),
  m_jTaus("InputjTaus",60),
  m_cTaus("InputcTaus",120),
  m_taus("InputTaus",120),
  m_jets("InputJets",60),
  m_jLJets("InputjLJets",60),
  m_gLJets("InputgLJets",60),
  m_jJets("InputjJets",60),
  m_gJets("InputgJets",60),
  m_muons("InputMuons",32),
  m_lateMuons("InputLateMuons",32),
  m_muonsNextBC("InputMuonsNextBC",32),
  m_met("InputMet",1),
  m_jxe("InputjXE",1),
  m_jxec("InputjXEC",1),
  m_jxeperf("InputjXEPERF",1),
  m_jte("InputjTE",1),
  m_jtec("InputjTEC",1),
  m_jtefwd("InputjTEFWD",1),
  m_jtefwda("InputjTEFWDA",1),
  m_jtefwdc("InputjTEFWDC",1),
  m_gxejwoj("InputgXEJWOJ",1),
  m_gxenc("InputgXENC",1),
  m_gxerho("InputgXERHO",1),
  m_gmht("InputgMHT",1),
  m_gte("InputgTE",1)
{
}

TopoInputEvent::~TopoInputEvent() {}

StatusCode TopoInputEvent::addCluster(const TCS::ClusterTOB & cluster) {
   m_clusters.push_back(cluster);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addeEm(const TCS::eEmTOB & eEm) {
   m_eEms.push_back(eEm);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addjEm(const TCS::jEmTOB & jEm) {
   m_jEms.push_back(jEm);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addeTau(const TCS::eTauTOB & eTau) {
   m_eTaus.push_back(eTau);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addcTau(const TCS::eTauTOB & eTau) {
   TCS::cTauTOB cTau(eTau.Et(), eTau.eta(), eTau.phi(), TCS::ETAU);
   cTau.setEtDouble( eTau.EtDouble() );
   cTau.setEtaDouble( eTau.etaDouble() );
   cTau.setPhiDouble( eTau.phiDouble() );
   cTau.setRCore( eTau.rCore() );
   cTau.setRHad( eTau.rHad() );
   m_cTaus.push_back(cTau);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addTau(const TCS::ClusterTOB & tau) {
   m_taus.push_back(tau);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addJet(const TCS::JetTOB & jet) {
   m_jets.push_back(jet);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addjTau(const TCS::jTauTOB & tau) {
   m_jTaus.push_back(tau);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addcTau(const TCS::jTauTOB & jTau) {
  TCS::cTauTOB cTau(jTau.Et(), jTau.eta(), jTau.phi(), TCS::JTAU);
   cTau.setEtDouble( jTau.EtDouble() );
   cTau.setEtaDouble( jTau.etaDouble() );
   cTau.setPhiDouble( jTau.phiDouble() );
   cTau.setEtIso( jTau.EtIso() );
   m_cTaus.push_back(cTau);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addjLJet(const TCS::jLJetTOB & jet) {
   m_jLJets.push_back(jet);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addgLJet(const TCS::gLJetTOB & jet) {
   m_gLJets.push_back(jet);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addjJet(const TCS::jJetTOB & jet) {
   m_jJets.push_back(jet);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addgJet(const TCS::gJetTOB & jet) {
   m_gJets.push_back(jet);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addMuon(const TCS::MuonTOB & muon) {
   m_muons.push_back(muon);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addLateMuon(const TCS::LateMuonTOB & muon) {
   m_lateMuons.push_back(muon);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addMuonNextBC(const TCS::MuonNextBCTOB & muon) {
   m_muonsNextBC.push_back(muon);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setMET(const TCS::MetTOB & met) {
   m_met.clear();
   m_met.push_back(met);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setjXE(const TCS::jXETOB & jxe) {
   m_jxe.clear();
   m_jxe.push_back(jxe);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setjTE(const TCS::jTETOB & jte) {
   m_jte.clear();
   m_jte.push_back(jte);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setjTEC(const TCS::jTETOB & jtec) {
   m_jtec.clear();
   m_jtec.push_back(jtec);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setjTEFWD(const TCS::jTETOB & jtefwd) {
   m_jtefwd.clear();
   m_jtefwd.push_back(jtefwd);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setjTEFWDA(const TCS::jTETOB & jtefwda) {
   m_jtefwda.clear();
   m_jtefwda.push_back(jtefwda);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setjTEFWDC(const TCS::jTETOB & jtefwdc) {
   m_jtefwdc.clear();
   m_jtefwdc.push_back(jtefwdc);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setgXEJWOJ(const TCS::gXETOB & gxejwoj) {
   m_gxejwoj.clear();
   m_gxejwoj.push_back(gxejwoj);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setgMHT(const TCS::gXETOB & gmht) {
   m_gmht.clear();
   m_gmht.push_back(gmht);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setgXENC(const TCS::gXETOB & gxenc) {
   m_gxenc.clear();
   m_gxenc.push_back(gxenc);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setgXERHO(const TCS::gXETOB & gxerho) {
   m_gxerho.clear();
   m_gxerho.push_back(gxerho);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setgTE(const TCS::gTETOB & gte) {
   m_gte.clear();
   m_gte.push_back(gte);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::setEventInfo(const uint32_t runNo, const uint32_t evtNo, const uint32_t lumiB, const uint32_t BCID) {
   m_runNo = runNo;
   m_evtNo = evtNo;
   m_lumiB = lumiB;
   m_BCID  = BCID; 
   return StatusCode::SUCCESS;
}

void TopoInputEvent::setOverflowFromEmtauInput (const bool &v)
{
    m_overflowFromEmtauInput = v;
}

void TopoInputEvent::setOverflowFromJetInput   (const bool &v)
{
    m_overflowFromJetInput = v;
}

void TopoInputEvent::setOverflowFromeEmInput   (const bool &v)
{
    m_overflowFromeEmInput = v;
}

void TopoInputEvent::setOverflowFromjEmInput   (const bool &v)
{
    m_overflowFromjEmInput = v;
}

void TopoInputEvent::setOverflowFromeTauInput   (const bool &v)
{
    m_overflowFromeTauInput = v;
}

void TopoInputEvent::setOverflowFromjTauInput   (const bool &v)
{
    m_overflowFromjTauInput = v;
}

void TopoInputEvent::setOverflowFromjJetInput   (const bool &v)
{
    m_overflowFromjJetInput = v;
}

void TopoInputEvent::setOverflowFromgJetInput   (const bool &v)
{
    m_overflowFromgJetInput = v;
}

void TopoInputEvent::setOverflowFromjLJetInput   (const bool &v)
{
    m_overflowFromjLJetInput = v;
}

void TopoInputEvent::setOverflowFromgLJetInput   (const bool &v)
{
    m_overflowFromgLJetInput = v;
}

void TopoInputEvent::setOverflowFromEnergyInput(const bool &v)
{
    m_overflowFromEnergyInput = v;
}

void TopoInputEvent::setOverflowFromjXEInput   (const bool &v)
{
    m_overflowFromjXEInput = v;
}

void TopoInputEvent::setOverflowFromjTEInput   (const bool &v)
{
    m_overflowFromjTEInput = v;
}

void TopoInputEvent::setOverflowFromjTECInput   (const bool &v)
{
    m_overflowFromjTECInput = v;
}

void TopoInputEvent::setOverflowFromjTEFWDInput   (const bool &v)
{
    m_overflowFromjTEFWDInput = v;
}

void TopoInputEvent::setOverflowFromjTEFWDAInput   (const bool &v)
{
    m_overflowFromjTEFWDAInput = v;
}

void TopoInputEvent::setOverflowFromjTEFWDCInput   (const bool &v)
{
    m_overflowFromjTEFWDCInput = v;
}

void TopoInputEvent::setOverflowFromgXEJWOJInput   (const bool &v)
{
    m_overflowFromgXEJWOJInput = v;
}

void TopoInputEvent::setOverflowFromgMHTInput   (const bool &v)
{
    m_overflowFromgMHTInput = v;
}

void TopoInputEvent::setOverflowFromgXENCInput   (const bool &v)
{
    m_overflowFromgXENCInput = v;
}

void TopoInputEvent::setOverflowFromgXERHOInput   (const bool &v)
{
    m_overflowFromgXERHOInput = v;
}

void TopoInputEvent::setOverflowFromgTEInput   (const bool &v)
{
    m_overflowFromgTEInput = v;
}

void TopoInputEvent::setOverflowFromMuonInput  (const bool &v)
{
    m_overflowFromMuonInput = v;
}

// access to data for the steering
const InputTOBArray *
TopoInputEvent::inputTOBs(inputTOBType_t tobType) const {
   switch(tobType) {
   case CLUSTER: return &m_clusters;
   case EEM: return &m_eEms;
   case JEM: return &m_jEms;
   case CTAU: return &m_cTaus;
   case JET: return &m_jets;
   case ETAU: return &m_eTaus;
   case JTAU: return &m_jTaus;
   case JJET: return &m_jJets;
   case GJET: return &m_gJets;
   case JLJET: return &m_jLJets;
   case GLJET: return &m_gLJets;
   case MUON: return &m_muons;
   case LATEMUON: return &m_lateMuons;
   case MUONNEXTBC: return &m_muonsNextBC;
   case TAU: return &m_taus;
   case MET: return &m_met;
   case JXE: return &m_jxe;
   case JXEC: return &m_jxec;
   case JXEPERF: return &m_jxeperf;
   case JTE: return &m_jte;
   case JTEC: return &m_jtec;
   case JTEFWD: return &m_jtefwd;
   case JTEFWDA: return &m_jtefwda;
   case JTEFWDC: return &m_jtefwdc;
   case GXEJWOJ: return &m_gxejwoj;
   case GXENC: return &m_gxenc;
   case GXERHO: return &m_gxerho;
   case GMHT: return &m_gmht;
   case GTE: return &m_gte;
   }
   return 0;
}

bool TopoInputEvent::hasInputOverflow(TCS::inputTOBType_t tobType) const
{
    //Not using or setting the overflow bits in practice. We will need to get back to this in 2022. 
    bool inputOverflow = false;
    switch(tobType) {
    case CLUSTER:    inputOverflow = overflowFromEmtauInput();  break;
    case JET:        inputOverflow = overflowFromJetInput();    break;
    case MUON:       inputOverflow = overflowFromMuonInput();   break;
    case LATEMUON:   inputOverflow = false;                     break; 
    case MUONNEXTBC: inputOverflow = false;                     break; 
    case TAU:        inputOverflow = overflowFromEmtauInput();  break;
    case MET:        inputOverflow = overflowFromEnergyInput(); break;
    case JXE:        inputOverflow = overflowFromjXEInput();    break;
    case JXEC:       inputOverflow = overflowFromjXEInput();    break;
    case JXEPERF:    inputOverflow = overflowFromjXEInput();    break;
    case JTE:        inputOverflow = overflowFromjTEInput();    break;
    case JTEC:       inputOverflow = overflowFromjTECInput();    break;
    case JTEFWD:     inputOverflow = overflowFromjTEFWDInput();    break;
    case JTEFWDA:    inputOverflow = overflowFromjTEFWDAInput();    break;
    case JTEFWDC:    inputOverflow = overflowFromjTEFWDCInput();    break;
    case GXEJWOJ:    inputOverflow = overflowFromgXEJWOJInput();    break;
    case GXENC:      inputOverflow = overflowFromgXEJWOJInput();    break;
    case GXERHO:     inputOverflow = overflowFromgXEJWOJInput();    break;
    case GMHT:       inputOverflow = overflowFromgMHTInput();   break;
    case GTE:        inputOverflow = overflowFromgTEInput();    break;
    default:         inputOverflow = false;
    }
   return inputOverflow;
}

TCS::StatusCode
TCS::TopoInputEvent::clear() {
   // only need to clear the vectors since the objects themselves are
   // collected on the ClusterTOB::heap and reset by the
   // TopoSteering::reset
   m_clusters.clear();
   m_eEms.clear();
   m_jEms.clear();
   m_eTaus.clear();
   m_cTaus.clear();
   m_jets.clear();
   m_jTaus.clear();
   m_jLJets.clear();
   m_gLJets.clear();
   m_jJets.clear();
   m_gJets.clear();
   m_taus.clear();
   m_muons.clear();
   m_lateMuons.clear();
   m_muonsNextBC.clear();
   m_met.clear();
   m_jxe.clear();
   m_jxec.clear();
   m_jxeperf.clear();
   m_jte.clear();
   m_jtec.clear();
   m_jtefwd.clear();
   m_jtefwda.clear();
   m_jtefwdc.clear();
   m_gxejwoj.clear();
   m_gxenc.clear();
   m_gxerho.clear();
   m_gmht.clear();
   m_gte.clear();
   m_runNo = 0;
   m_evtNo = 0;
   m_lumiB = 0;
   m_BCID  = 0;
   setMET(MetTOB(0,0,0));   // default MET
   setjXE(jXETOB(0,0,0));   // default jXE
   setjTE(jTETOB(0));       // default jTE
   setjTEC(jTETOB(0));       // default jTEC
   setjTEFWD(jTETOB(0));       // default jTEFWD
   setjTEFWDA(jTETOB(0));       // default jTEFWDA
   setjTEFWDC(jTETOB(0));       // default jTEFWDC
   setgXEJWOJ(gXETOB(0,0,0));   // default gXEJWOJ
   setgXENC(gXETOB(0,0,0));   // default gXENC
   setgXERHO(gXETOB(0,0,0));   // default gXERHO
   setgMHT(gXETOB(0,0,0)); // default gMHT
   setgTE(gTETOB(0));       // default gTE

   m_overflowFromMuonInput = false;

   return StatusCode::SUCCESS;
}

void
TopoInputEvent::enableInputDump( const std::string& filename, bool enable ) {
   m_dumpEnabled = enable;
   m_inputDumpFile = filename;
}


void
TopoInputEvent::dump() {

   if( ! m_dumpEnabled ) return;

   std::ofstream file(m_inputDumpFile, std::ios_base::out | std::ios_base::app );
   file << "<event>" << std::endl;

   file << "<cluster>" << std::endl;
   for(ClusterTOB* cluster : m_clusters) {
      file << cluster->Et() << "  " << cluster->isolation() << "  " << cluster->eta() << "  " << cluster->phi() << "  " << cluster->etaDouble() << "  " << cluster->phiDouble() << std::endl;
   }
   file << "</cluster>" << std::endl;

   file << "<eEm>" << std::endl;
   for(eEmTOB* em : m_eEms) {
      file << em->Et() << "  " << em->Reta() << "  " << em->Rhad() << "  " << em->Wstot() << "  " << em->eta() << "  " << em->phi() << "  " << em->etaDouble() << "  " << em->phiDouble() << std::endl;
   }
   file << "</eEm>" << std::endl;

   file << "<jEm>" << std::endl;
   for(jEmTOB* em : m_jEms) {
      file << em->Et() << "  " << em->eta() << "  " << em->phi() << "  " << em->etaDouble() << "  " << em->phiDouble() << std::endl;
   }
   file << "</jEm>" << std::endl;

   file << "<eTau>" << std::endl;
   for(eTauTOB* tau : m_eTaus) {
      file << tau->Et() << "  " << tau->rCore() << "  " << tau->rHad() << "  " << tau->eta() << "  " << tau->phi() << "  " << tau->etaDouble() << "  " << tau->phiDouble() << std::endl;
   }
   file << "</eTau>" << std::endl;

   file << "<tau>" << std::endl;
   for(ClusterTOB* tau : m_taus) {
      file << tau->Et() << "  " << tau->isolation() << "  " << tau->eta() << "  " << tau->phi() << "  " << tau->etaDouble() << "  " << tau->phiDouble() << std::endl;
   }
   file << "</tau>" << std::endl;

   file << "<jet>" << std::endl;
   for(JetTOB* jet : m_jets) {
      file << jet->Et1() << "  " << jet->Et2() << "  " << jet->eta() << "  " << jet->phi() << "  " << jet->etaDouble() << "  " << jet->phiDouble() << std::endl;
   }
   file << "</jet>" << std::endl;

   file << "<jTau>" << std::endl;
   for(jTauTOB* tau : m_jTaus) {
      file << tau->Et() << "  " << tau->eta() << "  " << tau->phi() << "  " << tau->etaDouble() << "  " << tau->phiDouble() << std::endl;
   }
   file << "</jTau>" << std::endl;

   file << "<jLJet>" << std::endl;
   for(jLJetTOB* jet : m_jLJets) {
      file << jet->Et() << "  " << jet->eta() << "  " << jet->phi() << "  " << jet->etaDouble() << "  " << jet->phiDouble() << std::endl;
   }
   file << "</jLJet>" << std::endl;

   file << "<gLJet>" << std::endl;
   for(gLJetTOB* jet : m_gLJets) {
      file << jet->Et() << "  " << jet->eta() << "  " << jet->phi() << "  " << jet->etaDouble() << "  " << jet->phiDouble() << std::endl;
   }
   file << "</gLJet>" << std::endl;

   file << "<jJet>" << std::endl;
   for(jJetTOB* jet : m_jJets) {
      file << jet->Et() << "  " << jet->eta() << "  " << jet->phi() << "  " << jet->etaDouble() << "  " << jet->phiDouble() << std::endl;
   }
   file << "</jJet>" << std::endl;

   file << "<gJet>" << std::endl;
   for(gJetTOB* jet : m_gJets) {
      file << jet->Et() << "  " << jet->eta() << "  " << jet->phi() << "  " << jet->etaDouble() << "  " << jet->phiDouble() << std::endl;
   }
   file << "</gJet>" << std::endl;

   file << "<muon>" << std::endl;
   for(MuonTOB* muon : m_muons) {
      file << muon->Et() << "  " << muon->eta() << "  " << muon->phi() << "  " << muon->EtaDouble() << "  " << muon->PhiDouble() << std::endl;
   }
   file << "</muon>" << std::endl;

   file << "<lateMuon>" << std::endl;
   for(LateMuonTOB* lateMuon : m_lateMuons) {
      file << lateMuon->Et() << "  " << lateMuon->eta() << "  " << lateMuon->phi() << "  " << lateMuon->EtaDouble() << "  " << lateMuon->PhiDouble() << std::endl;
   }
   file << "</lateMuon>" << std::endl;

   file << "<muonNextBC>" << std::endl;
   for(MuonNextBCTOB* muonNextBC : m_muonsNextBC) {
      file << muonNextBC->Et() << "  " << muonNextBC->eta() << "  " << muonNextBC->phi() << "  " << muonNextBC->EtaDouble() << "  " << muonNextBC->PhiDouble() << std::endl;
   }
   file << "</muonNextBC>" << std::endl;   

   file << "<jxe>" << std::endl;
   for(const jXETOB* jxe : m_jxe) {
      file << jxe->Ex() << "  " << jxe->Ey() << "  " << jxe->Et() << std::endl;
   }
   file << "</jxe>" << std::endl;

   file << "<jte>" << std::endl;
   for(const jTETOB* jte : m_jte) {
      file << jte->sumEt() << std::endl;
   }
   file << "</jte>" << std::endl;

   file << "<jtec>" << std::endl;
   for(const jTETOB* jtec : m_jtec) {
      file << jtec->sumEt() << std::endl;
   }
   file << "</jtec>" << std::endl;

   file << "<jtefwd>" << std::endl;
   for(const jTETOB* jtefwd : m_jtefwd) {
      file << jtefwd->sumEt() << std::endl;
   }
   file << "</jtefwd>" << std::endl;

   file << "<jtefwda>" << std::endl;
   for(const jTETOB* jtefwda : m_jtefwda) {
      file << jtefwda->sumEt() << std::endl;
   }
   file << "</jtefwda>" << std::endl;

   file << "<jtefwdc>" << std::endl;
   for(const jTETOB* jtefwdc : m_jtefwdc) {
      file << jtefwdc->sumEt() << std::endl;
   }
   file << "</jtefwdc>" << std::endl;

   file << "<gxejwoj>" << std::endl;
   for(const gXETOB* gxejwoj : m_gxejwoj) {
      file << gxejwoj->Ex() << "  " << gxejwoj->Ey() << "  " << gxejwoj->Et() << std::endl;
   }
   file << "</gxejwoj>" << std::endl;

   file << "<gmht>" << std::endl;
   for(const gXETOB* gmht : m_gmht) {
      file << gmht->Ex() << "  " << gmht->Ey() << "  " << gmht->Et() << std::endl;
   }
   file << "</gmht>" << std::endl;

   file << "<gxenc>" << std::endl;
   for(const gXETOB* gxenc : m_gxenc) {
      file << gxenc->Ex() << "  " << gxenc->Ey() << "  " << gxenc->Et() << std::endl;
   }
   file << "</gxenc>" << std::endl;

   file << "<gxerho>" << std::endl;
   for(const gXETOB* gxerho : m_gxerho) {
      file << gxerho->Ex() << "  " << gxerho->Ey() << "  " << gxerho->Et() << std::endl;
   }
   file << "</gxerho>" << std::endl;

   file << "<gte>" << std::endl;
   for(const gTETOB* gte : m_gte) {
      file << gte->sumEt() << std::endl;
   }
   file << "</gte>" << std::endl;

   file << "<met>" << std::endl;
   for(MetTOB* met : m_met) {
      file << met->Ex() << "  " << met->Ey() << "  " << met->Et() << std::endl;
   }
   file << "</met>" << std::endl;
   file << "<info>" << std::endl;
   file << m_runNo << "  " << m_evtNo << "  " << m_lumiB << "  " << m_BCID << std::endl;
   file << "</info>" << std::endl;
   file << "</event>" << std::endl;
   file.close();
}

void
TopoInputEvent::dumpStart() {
   if( ! m_dumpEnabled ) return;
   std::ofstream file(m_inputDumpFile);
   file << "<file>" << std::endl;
   file.close();
}

void
TopoInputEvent::dumpFinish() {
   if( ! m_dumpEnabled ) return;
   std::ofstream file(m_inputDumpFile, std::ios_base::out | std::ios_base::app);
   file << "</file>" << std::endl;
   file.close();
}



namespace TCS {


std::ostream & operator<<(std::ostream &o, const TCS::TopoInputEvent &evt) {
   o << "Event:" << std::endl;
   o << "  #clusters: " << evt.clusters().size() << " (capacity: " << evt.clusters().capacity() << ")" << std::endl;
   o << "  #eEms    : " << evt.eEms().size() << " (capacity: " << evt.eEms().capacity() << ")" << std::endl;
   o << "  #eTaus   : " << evt.eTaus().size() << " (capacity: " << evt.eTaus().capacity() << ")" << std::endl;
   o << "  #taus    : " << evt.taus().size() << " (capacity: " << evt.taus().capacity() << ")" << std::endl;
   o << "  #jets    : " << evt.jets().size() << " (capacity: " << evt.jets().capacity() << ")" << std::endl;
   o << "  #jJets   : " << evt.jJets().size() << " (capacity: " << evt.jJets().capacity() << ")" << std::endl;
   o << "  #muons   : " << evt.muons().size() << " (capacity: " << evt.muons().capacity() << ")" << std::endl;
   o << "  #latemuons   : " << evt.lateMuons().size() << " (capacity: " << evt.lateMuons().capacity() << ")" << std::endl;
   o << "  #muonsNextBC : " << evt.muonsNextBC().size() << " (capacity: " << evt.muonsNextBC().capacity() << ")" << std::endl;
   o << "  #jxe     : " << evt.m_jxe.size() << " (capacity: " << evt.m_jxe.capacity() << ")" << std::endl;
   o << "  #jte     : " << evt.m_jte.size() << " (capacity: " << evt.m_jte.capacity() << ")" << std::endl;
   o << "  #jtec    : " << evt.m_jtec.size() << " (capacity: " << evt.m_jtec.capacity() << ")" << std::endl;
   o << "  #jtefwd  : " << evt.m_jtefwd.size() << " (capacity: " << evt.m_jtefwd.capacity() << ")" << std::endl;
   o << "  #jtefwda : " << evt.m_jtefwda.size() << " (capacity: " << evt.m_jtefwda.capacity() << ")" << std::endl;
   o << "  #jtefwdc : " << evt.m_jtefwdc.size() << " (capacity: " << evt.m_jtefwdc.capacity() << ")" << std::endl;
   o << "  #gxejwoj : " << evt.m_gxejwoj.size() << " (capacity: " << evt.m_gxejwoj.capacity() << ")" << std::endl;
   o << "  #gmht    : " << evt.m_gmht.size() << " (capacity: " << evt.m_gmht.capacity() << ")" << std::endl;
   o << "  #gxenc   : " << evt.m_gxenc.size() << " (capacity: " << evt.m_gxenc.capacity() << ")" << std::endl;
   o << "  #gxerho  : " << evt.m_gxerho.size() << " (capacity: " << evt.m_gxerho.capacity() << ")" << std::endl;
   o << "  #gte     : " << evt.m_gte.size() << " (capacity: " << evt.m_gte.capacity() << ")" << std::endl;
   o << "  #met     : " << evt.m_met.size() << " (capacity: " << evt.m_met.capacity() << ")" << std::endl;
   o << "  #info    : runNo, evtNo, lumiBlock and BCID" << std::endl;
   
   o << "Details:" << std::endl;
   o << "Cluster input vector (" << evt.clusters().name() << "):" << std::endl << evt.clusters();
   o << "eEm input vector (" << evt.eEms().name() << "):" << std::endl << evt.eEms();
   o << "eTau input vector (" << evt.eTaus().name() << "):" << std::endl << evt.eTaus();
   o << "Tau input vector (" << evt.taus().name() << "):" << std::endl << evt.taus();
   o << "Jet input vector (" << evt.jets().name() << "):" << std::endl << evt.jets();
   o << "jJet input vector (" << evt.jJets().name() << "):" << std::endl << evt.jJets();
   o << "Muon input vector (" << evt.muons().name() << "):" << std::endl << evt.muons();
   o << "LateMuon input vector (" << evt.lateMuons().name() << "):" << std::endl << evt.lateMuons();
   o << "MuonNextBC input vector (" << evt.muonsNextBC().name() << "):" << std::endl << evt.muonsNextBC();
   o << "jXE input (" << evt.m_jxe.name() << "):" << std::endl << evt.m_jxe;
   o << "jTE input (" << evt.m_jte.name() << "):" << std::endl << evt.m_jte;
   o << "jTEC input (" << evt.m_jtec.name() << "):" << std::endl << evt.m_jtec;
   o << "jTEFWD input (" << evt.m_jtefwd.name() << "):" << std::endl << evt.m_jtefwd;
   o << "jTEFWDA input (" << evt.m_jtefwda.name() << "):" << std::endl << evt.m_jtefwda;
   o << "jTEFWDC input (" << evt.m_jtefwdc.name() << "):" << std::endl << evt.m_jtefwdc;
   o << "gXEJWOJ input (" << evt.m_gxejwoj.name() << "):" << std::endl << evt.m_gxejwoj;
   o << "gMHT input (" << evt.m_gmht.name() << "):" << std::endl << evt.m_gmht;
   o << "gXENC input (" << evt.m_gxenc.name() << "):" << std::endl << evt.m_gxenc;
   o << "gXERHO input (" << evt.m_gxerho.name() << "):" << std::endl << evt.m_gxerho;
   o << "gTE input (" << evt.m_gte.name() << "):" << std::endl << evt.m_gte;
   o << "MET input (" << evt.m_met.name() << "):" << std::endl << evt.m_met;
   o << "Overflow from:"
     <<" EmtauInput "<<evt.overflowFromEmtauInput()
     <<" JetInput "<<evt.overflowFromJetInput()
     <<" EnergyInput "<<evt.overflowFromEnergyInput()
     <<" MuonInput "<<evt.overflowFromMuonInput()
     << std::endl;
   o << "Event info: " << evt.run_number() << "  " << evt.event_number() << "  " << evt.lumi_block() << "  " << evt.bunch_crossing_id();

   return o;
}

}


void
TopoInputEvent::print() const {
   TRG_MSG_INFO("Event:");
   TRG_MSG_INFO("  #clusters: " << clusters().size() << " (capacity: " << clusters().capacity() << ")");
   TRG_MSG_INFO("  #eEms    : " << eEms().size() << " (capacity: " << eEms().capacity() << ")");
   TRG_MSG_INFO("  #eTaus   : " << eTaus().size() << " (capacity: " << eTaus().capacity() << ")");
   TRG_MSG_INFO("  #taus    : " << taus().size() << " (capacity: " << taus().capacity() << ")");
   TRG_MSG_INFO("  #jets    : " << jets().size() << " (capacity: " << jets().capacity() << ")");
   TRG_MSG_INFO("  #jJets   : " << jJets().size() << " (capacity: " << jJets().capacity() << ")");
   TRG_MSG_INFO("  #muons   : " << muons().size() << " (capacity: " << muons().capacity() << ")");
   TRG_MSG_INFO("  #latemuons  : " << lateMuons().size() << " (capacity: " << lateMuons().capacity() << ")");
   TRG_MSG_INFO("  #muonsNextBC: " << muonsNextBC().size() << " (capacity: " << muonsNextBC().capacity() << ")");
   TRG_MSG_INFO("  #jxe     : " << m_jxe.size() << " (capacity: " << m_jxe.capacity() << ")");
   TRG_MSG_INFO("  #jte     : " << m_jte.size() << " (capacity: " << m_jte.capacity() << ")");
   TRG_MSG_INFO("  #jtec    : " << m_jtec.size() << " (capacity: " << m_jtec.capacity() << ")");
   TRG_MSG_INFO("  #jtefwd  : " << m_jtefwd.size() << " (capacity: " << m_jtefwd.capacity() << ")");
   TRG_MSG_INFO("  #jtefwda : " << m_jtefwda.size() << " (capacity: " << m_jtefwda.capacity() << ")");
   TRG_MSG_INFO("  #jtefwdc : " << m_jtefwdc.size() << " (capacity: " << m_jtefwdc.capacity() << ")");
   TRG_MSG_INFO("  #gxejwoj : " << m_gxejwoj.size() << " (capacity: " << m_gxejwoj.capacity() << ")");
   TRG_MSG_INFO("  #gmht    : " << m_gmht.size() << " (capacity: " << m_gmht.capacity() << ")");
   TRG_MSG_INFO("  #gxenc    : " << m_gxenc.size() << " (capacity: " << m_gxenc.capacity() << ")");
   TRG_MSG_INFO("  #gxerho   : " << m_gxerho.size() << " (capacity: " << m_gxerho.capacity() << ")");
   TRG_MSG_INFO("  #gte     : " << m_gte.size() << " (capacity: " << m_gte.capacity() << ")");
   TRG_MSG_INFO("  #met     : " << m_met.size() << " (capacity: " << m_met.capacity() << ")");
   
   TRG_MSG_DEBUG("Details:");
   TRG_MSG_DEBUG("Cluster input vector (" << clusters().name() << "):");
   for(auto * x : clusters()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("eEm input vector (" << eEms().name() << "):");
   for(auto * x : eEms()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("eTau input vector (" << eTaus().name() << "):");
   for(auto * x : eTaus()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Tau input vector (" << taus().name() << "):");// << std::endl << taus();
   for(auto * x : taus()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Jet input vector (" << jets().name() << "):");// << std::endl << jets();
   for(auto * x : jets()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("jJet input vector (" << jJets().name() << "):");
   for(auto * x : jJets()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Muon input vector (" << muons().name() << "):");// << std::endl << muons();
   for(auto * x : muons()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("LateMuon input vector (" << lateMuons().name() << "):");// << std::endl << latemuons();
   for(auto * x : lateMuons()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("MuonsNextBC input vector (" << muonsNextBC().name() << "):");// << std::endl << muonsNextBC();
   for(auto * x : muonsNextBC()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("jXE input (" << m_jxe.name() << "):");
   for(auto * x : m_jxe) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("jTE input (" << m_jte.name() << "):");
   for(auto * x : m_jte) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("jTEC input (" << m_jtec.name() << "):");
   for(auto * x : m_jtec) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("jTEFWD input (" << m_jtefwd.name() << "):");
   for(auto * x : m_jtefwd) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("jTEFWDA input (" << m_jtefwda.name() << "):");
   for(auto * x : m_jtefwda) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("jTEFWDC input (" << m_jtefwdc.name() << "):");
   for(auto * x : m_jtefwdc) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("gXEJWOJ input (" << m_gxejwoj.name() << "):");
   for(auto * x : m_gxejwoj) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("gMHT input (" << m_gmht.name() << "):");
   for(auto * x : m_gmht) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("gXENC input (" << m_gxenc.name() << "):");
   for(auto * x : m_gxenc) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("gXERHO input (" << m_gxerho.name() << "):");
   for(auto * x : m_gxerho) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("gTE input (" << m_gte.name() << "):");
   for(auto * x : m_gte) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("MET input (" << m_met.name() << "):");// << std::endl << m_met;
   for(auto * x : m_met) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Overflow bits from:"
                 <<" emtau "<<m_overflowFromEmtauInput
                 <<" jet "<<m_overflowFromJetInput
                 <<" jxe "<<m_overflowFromjXEInput
                 <<" energy "<<m_overflowFromEnergyInput
                 <<" muon "<<m_overflowFromMuonInput);
   TRG_MSG_DEBUG("Event info:");
   TRG_MSG_DEBUG("      runNo: " << run_number() << "  evtNo: " << event_number() << "  lumiBlock: " << lumi_block() << "  BCID: " << bunch_crossing_id());
}


