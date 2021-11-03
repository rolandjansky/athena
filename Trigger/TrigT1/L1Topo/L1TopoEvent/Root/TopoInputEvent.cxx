// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/TopoInputEvent.h"
#include <fstream>
#include <pthread.h>

using namespace TCS;

//Current input list size for gFEX tobs and jEM might be not accurate 
TopoInputEvent::TopoInputEvent() :
  TrigConfMessaging("TopoInputEvent"),
  m_clusters("InputClusters",120),
  m_eems("InputeEms",120),
  m_jems("InputjEms",60),
  m_etaus("InputeTaus",120),
  m_jtaus("InputjTaus",60),
  m_ctaus("InputcTaus",120),
  m_taus("InputTaus",120),
  m_jets("InputJets",60),
  m_jLargeRJets("InputjLargeRJets",60),
  m_gLargeRJets("InputgLargeRJets",60),
  m_jJets("InputjJets",60),
  m_gJets("InputgJets",60),
  m_muons("InputMuons",32),
  m_lateMuons("InputLateMuons",32),
  m_muonsNextBC("InputMuonsNextBC",32),
  m_met("InputMet",1)
{
}

TopoInputEvent::~TopoInputEvent() {}

StatusCode TopoInputEvent::addCluster(const TCS::ClusterTOB & cluster) {
   m_clusters.push_back(cluster);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addeEm(const TCS::eEmTOB & eem) {
   m_eems.push_back(eem);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addjEm(const TCS::jEmTOB & jem) {
   m_jems.push_back(jem);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addeTau(const TCS::eTauTOB & etau) {
   m_etaus.push_back(etau);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addcTau(const TCS::eTauTOB & etau) {
   TCS::cTauTOB ctau(etau.Et(),etau.isolation(),etau.eta(),etau.phi(),TCS::ETAU);
   ctau.setEtDouble( etau.EtDouble() );
   ctau.setEtaDouble( etau.etaDouble() );
   ctau.setPhiDouble( etau.phiDouble() );
   m_ctaus.push_back(ctau);
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
   m_jtaus.push_back(tau);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addcTau(const TCS::jTauTOB & jtau) {
  TCS::cTauTOB ctau(jtau.Et(),jtau.isolation(),jtau.eta(),jtau.phi(),TCS::JTAU);
   ctau.setEtDouble( jtau.EtDouble() );
   ctau.setEtaDouble( jtau.etaDouble() );
   ctau.setPhiDouble( jtau.phiDouble() );
   m_ctaus.push_back(ctau);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addjLargeRJet(const TCS::jLargeRJetTOB & jet) {
   m_jLargeRJets.push_back(jet);
   return StatusCode::SUCCESS;
}

StatusCode TopoInputEvent::addgLargeRJet(const TCS::gLargeRJetTOB & jet) {
   m_gLargeRJets.push_back(jet);
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

void TopoInputEvent::setOverflowFromjLargeRJetInput   (const bool &v)
{
    m_overflowFromjLargeRJetInput = v;
}

void TopoInputEvent::setOverflowFromgLargeRJetInput   (const bool &v)
{
    m_overflowFromgLargeRJetInput = v;
}

void TopoInputEvent::setOverflowFromEnergyInput(const bool &v)
{
    m_overflowFromEnergyInput = v;
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
   case EEM: return &m_eems;
   case JEM: return &m_jems;
   case CTAU: return &m_ctaus;
   case JET: return &m_jets;
   case ETAU: return &m_etaus;
   case JTAU: return &m_jtaus;
   case JJET: return &m_jJets;
   case GJET: return &m_gJets;
   case JLARGERJET: return &m_jLargeRJets;
   case GLARGERJET: return &m_gLargeRJets;
   case MUON: return &m_muons;
   case LATEMUON: return &m_lateMuons;
   case MUONNEXTBC: return &m_muonsNextBC;
   case TAU: return &m_taus;
   case MET: return &m_met;
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
   m_eems.clear();
   m_jems.clear();
   m_etaus.clear();
   m_ctaus.clear();
   m_jets.clear();
   m_jtaus.clear();
   m_jLargeRJets.clear();
   m_gLargeRJets.clear();
   m_jJets.clear();
   m_gJets.clear();
   m_taus.clear();
   m_muons.clear();
   m_lateMuons.clear();
   m_muonsNextBC.clear();
   m_met.clear();
   m_runNo = 0;
   m_evtNo = 0;
   m_lumiB = 0;
   m_BCID  = 0;
   setMET(MetTOB(0,0,0)); // default MET

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

   file << "<eem>" << std::endl;
   for(eEmTOB* eem : m_eems) {
      file << eem->Et() << "  " << eem->isolation() << "  " << eem->eta() << "  " << eem->phi() << "  " << eem->etaDouble() << "  " << eem->phiDouble() << std::endl;
   }
   file << "</eem>" << std::endl;

   file << "<jem>" << std::endl;
   for(jEmTOB* jem : m_jems) {
      file << jem->Et() << "  " << jem->eta() << "  " << jem->phi() << "  " << jem->etaDouble() << "  " << jem->phiDouble() << std::endl;
   }
   file << "</jem>" << std::endl;

   file << "<etau>" << std::endl;
   for(eTauTOB* etau : m_etaus) {
      file << etau->Et() << "  " << etau->isolation() << "  " << etau->eta() << "  " << etau->phi() << "  " << etau->etaDouble() << "  " << etau->phiDouble() << std::endl;
   }
   file << "</etau>" << std::endl;

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
   for(jTauTOB* tau : m_jtaus) {
      file << tau->Et() << "  " << tau->eta() << "  " << tau->phi() << "  " << tau->etaDouble() << "  " << tau->phiDouble() << std::endl;
   }
   file << "</jTau>" << std::endl;

   file << "<jLargeRJet>" << std::endl;
   for(jLargeRJetTOB* jet : m_jLargeRJets) {
      file << jet->Et() << "  " << jet->eta() << "  " << jet->phi() << "  " << jet->etaDouble() << "  " << jet->phiDouble() << std::endl;
   }
   file << "</jLargeRJet>" << std::endl;

   file << "<gLargeRJet>" << std::endl;
   for(gLargeRJetTOB* jet : m_gLargeRJets) {
      file << jet->Et() << "  " << jet->eta() << "  " << jet->phi() << "  " << jet->etaDouble() << "  " << jet->phiDouble() << std::endl;
   }
   file << "</gLargeRJet>" << std::endl;

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
   o << "  #eems    : " << evt.eems().size() << " (capacity: " << evt.eems().capacity() << ")" << std::endl;
   o << "  #etaus    : " << evt.etaus().size() << " (capacity: " << evt.etaus().capacity() << ")" << std::endl;
   o << "  #taus    : " << evt.taus().size() << " (capacity: " << evt.taus().capacity() << ")" << std::endl;
   o << "  #jets    : " << evt.jets().size() << " (capacity: " << evt.jets().capacity() << ")" << std::endl;
   o << "  #muons   : " << evt.muons().size() << " (capacity: " << evt.muons().capacity() << ")" << std::endl;
   o << "  #latemuons   : " << evt.lateMuons().size() << " (capacity: " << evt.lateMuons().capacity() << ")" << std::endl;
   o << "  #muonsNextBC : " << evt.muonsNextBC().size() << " (capacity: " << evt.muonsNextBC().capacity() << ")" << std::endl;
   o << "  #met     : " << evt.m_met.size() << " (capacity: " << evt.m_met.capacity() << ")" << std::endl;
   o << "  #info    : runNo, evtNo, lumiBlock and BCID" << std::endl;
   
   o << "Details:" << std::endl;
   o << "Cluster input vector (" << evt.clusters().name() << "):" << std::endl << evt.clusters();
   o << "eEm input vector (" << evt.eems().name() << "):" << std::endl << evt.eems();
   o << "eTau input vector (" << evt.etaus().name() << "):" << std::endl << evt.etaus();
   o << "Tau input vector (" << evt.taus().name() << "):" << std::endl << evt.taus();
   o << "Jet input vector (" << evt.jets().name() << "):" << std::endl << evt.jets();
   o << "Muon input vector (" << evt.muons().name() << "):" << std::endl << evt.muons();
   o << "LateMuon input vector (" << evt.lateMuons().name() << "):" << std::endl << evt.lateMuons();
   o << "MuonNextBC input vector (" << evt.muonsNextBC().name() << "):" << std::endl << evt.muonsNextBC();
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
   TRG_MSG_INFO("  #eems    : " << eems().size() << " (capacity: " << eems().capacity() << ")");
   TRG_MSG_INFO("  #etaus    : " << etaus().size() << " (capacity: " << etaus().capacity() << ")");
   TRG_MSG_INFO("  #taus    : " << taus().size() << " (capacity: " << taus().capacity() << ")");
   TRG_MSG_INFO("  #jets    : " << jets().size() << " (capacity: " << jets().capacity() << ")");
   TRG_MSG_INFO("  #muons   : " << muons().size() << " (capacity: " << muons().capacity() << ")");
   TRG_MSG_INFO("  #latemuons  : " << lateMuons().size() << " (capacity: " << lateMuons().capacity() << ")");
   TRG_MSG_INFO("  #muonsNextBC: " << muonsNextBC().size() << " (capacity: " << muonsNextBC().capacity() << ")");
   TRG_MSG_INFO("  #met     : " << m_met.size() << " (capacity: " << m_met.capacity() << ")");
   
   TRG_MSG_DEBUG("Details:");
   TRG_MSG_DEBUG("Cluster input vector (" << clusters().name() << "):");
   for(auto * x : clusters()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("eEm input vector (" << eems().name() << "):");
   for(auto * x : eems()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("eTau input vector (" << etaus().name() << "):");
   for(auto * x : etaus()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Tau input vector (" << taus().name() << "):");// << std::endl << taus();
   for(auto * x : taus()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Jet input vector (" << jets().name() << "):");// << std::endl << jets();
   for(auto * x : jets()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Muon input vector (" << muons().name() << "):");// << std::endl << muons();
   for(auto * x : muons()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("LateMuon input vector (" << lateMuons().name() << "):");// << std::endl << latemuons();
   for(auto * x : lateMuons()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("MuonsNextBC input vector (" << muonsNextBC().name() << "):");// << std::endl << muonsNextBC();
   for(auto * x : muonsNextBC()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("MET input (" << m_met.name() << "):");// << std::endl << m_met;
   for(auto * x : m_met) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Overflow bits from:"
                 <<" emtau "<<m_overflowFromEmtauInput
                 <<" jet "<<m_overflowFromJetInput
                 <<" energy "<<m_overflowFromEnergyInput
                 <<" muon "<<m_overflowFromMuonInput);
   TRG_MSG_DEBUG("Event info:");
   TRG_MSG_DEBUG("      runNo: " << run_number() << "  evtNo: " << event_number() << "  lumiBlock: " << lumi_block() << "  BCID: " << bunch_crossing_id());
}


