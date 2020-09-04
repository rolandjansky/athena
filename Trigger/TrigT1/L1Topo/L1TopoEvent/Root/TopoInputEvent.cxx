// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/TopoInputEvent.h"
#include <fstream>
#include <pthread.h>

using namespace std;
using namespace TCS;

TopoInputEvent::TopoInputEvent(const string & clusterName,
                               const string & tauName,
                               const string & jetName,
                               const string & muonName,
                               const string & lateMuonName,
                               const string & muonNextBCName,
                               const string & metName) :
   TrigConfMessaging("TopoInputEvent"),
   m_clusters(clusterName,120),
   m_taus(tauName,120),
   m_jets(jetName,60),
   m_muons(muonName,32),
   m_lateMuons(lateMuonName,32),
   m_muonsNextBC(muonNextBCName,32),
   m_met(metName,1)
{}

TopoInputEvent::~TopoInputEvent() {}

StatusCode TopoInputEvent::addCluster(const TCS::ClusterTOB & cluster) {
   m_clusters.push_back(cluster);
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
   case JET: return &m_jets;
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
    bool inputOverflow = false;
    switch(tobType) {
    case CLUSTER:    inputOverflow = overflowFromEmtauInput();  break;
    case JET:        inputOverflow = overflowFromJetInput();    break;
    case MUON:       inputOverflow = overflowFromMuonInput();   break;
    case LATEMUON:   inputOverflow = false;                     break; // DG not sure
    case MUONNEXTBC: inputOverflow = false;                     break; // DG not sure
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
   m_jets.clear();
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

   ofstream file(m_inputDumpFile, ios_base::out | ios_base::app );
   file << "<event>" << endl;
   file << "<cluster>" << endl;
   for(ClusterTOB* cluster : m_clusters) {
      file << cluster->Et() << "  " << cluster->isolation() << "  " << cluster->eta() << "  " << cluster->phi() << "  " << cluster->etaDouble() << "  " << cluster->phiDouble() << endl;
   }
   file << "</cluster>" << endl;
   file << "<tau>" << endl;
   for(ClusterTOB* tau : m_taus) {
      file << tau->Et() << "  " << tau->isolation() << "  " << tau->eta() << "  " << tau->phi() << "  " << tau->etaDouble() << "  " << tau->phiDouble() << endl;
   }
   file << "</tau>" << endl;
   file << "<jet>" << endl;
   for(JetTOB* jet : m_jets) {
      file << jet->Et1() << "  " << jet->Et2() << "  " << jet->eta() << "  " << jet->phi() << "  " << jet->etaDouble() << "  " << jet->phiDouble() << endl;
   }
   file << "</jet>" << endl;
   file << "<muon>" << endl;
   for(MuonTOB* muon : m_muons) {
      file << muon->Et() << "  " << muon->eta() << "  " << muon->phi() << "  " << muon->EtaDouble() << "  " << muon->PhiDouble() << endl;
   }
   file << "</muon>" << endl;
   file << "<lateMuon>" << endl;
   for(LateMuonTOB* lateMuon : m_lateMuons) {
      file << lateMuon->Et() << "  " << lateMuon->eta() << "  " << lateMuon->phi() << "  " << lateMuon->EtaDouble() << "  " << lateMuon->PhiDouble() << endl;
   }
   file << "</lateMuon>" << endl;
   file << "<muonNextBC>" << endl;
   for(MuonNextBCTOB* muonNextBC : m_muonsNextBC) {
      file << muonNextBC->Et() << "  " << muonNextBC->eta() << "  " << muonNextBC->phi() << "  " << muonNextBC->EtaDouble() << "  " << muonNextBC->PhiDouble() << endl;
   }
   file << "</muonNextBC>" << endl;   
   file << "<met>" << endl;
   for(MetTOB* met : m_met) {
      file << met->Ex() << "  " << met->Ey() << "  " << met->Et() << endl;
   }
   file << "</met>" << endl;
   file << "<info>" << endl;
   file << m_runNo << "  " << m_evtNo << "  " << m_lumiB << "  " << m_BCID << endl;
   file << "</info>" << endl;
   file << "</event>" << endl;
   file.close();
}

void
TopoInputEvent::dumpStart() {
   if( ! m_dumpEnabled ) return;
   ofstream file(m_inputDumpFile);
   file << "<file>" << endl;
   file.close();
}

void
TopoInputEvent::dumpFinish() {
   if( ! m_dumpEnabled ) return;
   ofstream file(m_inputDumpFile, ios_base::out | ios_base::app);
   file << "</file>" << endl;
   file.close();
}



namespace TCS {


std::ostream & operator<<(std::ostream &o, const TCS::TopoInputEvent &evt) {
   o << "Event:" << endl;
   o << "  #clusters: " << evt.clusters().size() << " (capacity: " << evt.clusters().capacity() << ")" << endl;
   o << "  #taus    : " << evt.taus().size() << " (capacity: " << evt.taus().capacity() << ")" << endl;
   o << "  #jets    : " << evt.jets().size() << " (capacity: " << evt.jets().capacity() << ")" << endl;
   o << "  #muons   : " << evt.muons().size() << " (capacity: " << evt.muons().capacity() << ")" << endl;
   o << "  #latemuons   : " << evt.lateMuons().size() << " (capacity: " << evt.lateMuons().capacity() << ")" << endl;
   o << "  #muonsNextBC : " << evt.muonsNextBC().size() << " (capacity: " << evt.muonsNextBC().capacity() << ")" << endl;
   o << "  #met     : " << evt.m_met.size() << " (capacity: " << evt.m_met.capacity() << ")" << endl;
   o << "  #info    : runNo, evtNo, lumiBlock and BCID" << endl;
   
   o << "Details:" << endl;
   o << "Cluster input vector (" << evt.clusters().name() << "):" << endl << evt.clusters();
   o << "Tau input vector (" << evt.taus().name() << "):" << endl << evt.taus();
   o << "Jet input vector (" << evt.jets().name() << "):" << endl << evt.jets();
   o << "Muon input vector (" << evt.muons().name() << "):" << endl << evt.muons();
   o << "LateMuon input vector (" << evt.lateMuons().name() << "):" << endl << evt.lateMuons();
   o << "MuonNextBC input vector (" << evt.muonsNextBC().name() << "):" << endl << evt.muonsNextBC();
   o << "MET input (" << evt.m_met.name() << "):" << endl << evt.m_met;
   o << "Overflow from:"
     <<" EmtauInput "<<evt.overflowFromEmtauInput()
     <<" JetInput "<<evt.overflowFromJetInput()
     <<" EnergyInput "<<evt.overflowFromEnergyInput()
     <<" MuonInput "<<evt.overflowFromMuonInput()
     << endl;
   o << "Event info: " << evt.run_number() << "  " << evt.event_number() << "  " << evt.lumi_block() << "  " << evt.bunch_crossing_id();

   return o;
}

}


void
TopoInputEvent::print() const {
   TRG_MSG_INFO("Event:");
   TRG_MSG_INFO("  #clusters: " << clusters().size() << " (capacity: " << clusters().capacity() << ")");
   TRG_MSG_INFO("  #taus    : " << taus().size() << " (capacity: " << taus().capacity() << ")");
   TRG_MSG_INFO("  #jets    : " << jets().size() << " (capacity: " << jets().capacity() << ")");
   TRG_MSG_INFO("  #muons   : " << muons().size() << " (capacity: " << muons().capacity() << ")");
   TRG_MSG_INFO("  #latemuons  : " << lateMuons().size() << " (capacity: " << lateMuons().capacity() << ")");
   TRG_MSG_INFO("  #muonsNextBC: " << muonsNextBC().size() << " (capacity: " << muonsNextBC().capacity() << ")");
   TRG_MSG_INFO("  #met     : " << m_met.size() << " (capacity: " << m_met.capacity() << ")");
   
   TRG_MSG_DEBUG("Details:");
   TRG_MSG_DEBUG("Cluster input vector (" << clusters().name() << "):");
   for(auto * x : clusters()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Tau input vector (" << taus().name() << "):");// << endl << taus();
   for(auto * x : taus()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Jet input vector (" << jets().name() << "):");// << endl << jets();
   for(auto * x : jets()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Muon input vector (" << muons().name() << "):");// << endl << muons();
   for(auto * x : muons()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("LateMuon input vector (" << lateMuons().name() << "):");// << endl << latemuons();
   for(auto * x : lateMuons()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("MuonsNextBC input vector (" << muonsNextBC().name() << "):");// << endl << muonsNextBC();
   for(auto * x : muonsNextBC()) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("MET input (" << m_met.name() << "):");// << endl << m_met;
   for(auto * x : m_met) TRG_MSG_DEBUG("      " << *x);
   TRG_MSG_DEBUG("Overflow bits from:"
                 <<" emtau "<<m_overflowFromEmtauInput
                 <<" jet "<<m_overflowFromJetInput
                 <<" energy "<<m_overflowFromEnergyInput
                 <<" muon "<<m_overflowFromMuonInput);
   TRG_MSG_DEBUG("Event info:");
   TRG_MSG_DEBUG("      runNo: " << run_number() << "  evtNo: " << event_number() << "  lumiBlock: " << lumi_block() << "  BCID: " << bunch_crossing_id());
}


