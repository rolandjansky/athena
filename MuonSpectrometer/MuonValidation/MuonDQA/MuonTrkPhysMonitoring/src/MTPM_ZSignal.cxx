/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ================================================================================
   MTPM_ZSignal.cxx
   ------------------------------
   AUTHORS:     Nektarios Chr.Benekos, Austin Basye, Christina G. Skarpathiotaki (UoP)
   created:     September 2013
   description: Implementation code for the MTPM_ZSignal
   ============================================================================== */

#include "MuonTrkPhysMonitoring/MuonTrkPhysMonitoring.h"

using namespace std;

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::bookHistograms_ZSignal(std::string sPath)
{
  ATH_MSG_DEBUG("Booking Histograms for bookHistograms_ZSignal");
  InitializePlots(m_oZSignalPlots, MonGroup(this, sPath, run, ATTRIB_UNMANAGED));
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::procHistograms_ZSignal()
{    
  ATH_MSG_DEBUG("Processing Histograms for procHistograms_ZSignal");
  m_oZSignalPlots.finalize();    
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::fillTagAndProbe(const xAOD::Muon* Muon, const xAOD::TrackParticle* RecordProbe, const xAOD::MuonContainer* MuonJar)
{
  ATH_MSG_DEBUG("Filling Histograms for fillHistograms_ZSignal");
  double fTagEta= Muon->eta();
  const xAOD::TrackParticle* primaryTP = Muon->trackParticle(xAOD::Muon::Primary);
  double fTagCharge =  primaryTP ? primaryTP->charge() : 0; //TODO: Maybe issue a warning if this fails
  double fTagEtaRegion = EtaRegion(fTagEta);            
  
  double fProbeEta= RecordProbe->eta();
  double fProbePhi= RecordProbe->phi();
  double fProbeCharge = RecordProbe->charge();
  double fProbeEtaRegion = EtaRegion(fProbeEta);
  
  double IVmass = ( RecordProbe->p4() + Muon->p4() ).M()*0.001;
    
   //For efficiency measurement: must see if it's matched to a CB muon
   m_oZSignalPlots.m_Z_occupancy->Fill(fTagEtaRegion);
   m_oZSignalPlots.m_Z_EffDenominator->Fill(fProbeEtaRegion,PhiSector(fProbePhi));
   m_oZSignalPlots.m_Z_EffDenominator_eta->Fill(fProbeEta);
                 
   if(ProbeToMuonMatching(RecordProbe, MuonJar)){
     m_oZSignalPlots.m_Z_occupancy->Fill(fProbeEtaRegion);
     m_oZSignalPlots.m_Z_EffNumerator->Fill(fProbeEtaRegion,PhiSector(fProbePhi));
     m_oZSignalPlots.m_Z_EffNumerator_eta->Fill(fProbeEta);
             
     if (fTagCharge > 0) m_oZSignalPlots.fill(fTagEta, fProbeEta, IVmass);        
     if (fProbeCharge > 0) m_oZSignalPlots.fill(fProbeEta, fTagEta, IVmass);        
   }
}


//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::fillHistograms_ZSignal( const xAOD::Muon* muon, const xAOD::MuonContainer* MuonJar, const xAOD::TrackParticleContainer* IDTrackContainer) {
    
  ATH_MSG_DEBUG("Begin Tag+Probe Z Monitoring...");
  int probe_charge = (m_eventCounter%2) ? 1 : -1;    //Decorrelation of Tag and Probe Charges
                           
  if (!IsTagMuon(muon) ) return;  
  if (!IsZTriggerMatched(muon)) return;
  
  double pT_record     = 0.0001;
  const xAOD::TrackParticle* RecordProbe = 0;
                                 
  //Begin Loop through the ID Track container
  ATH_MSG_DEBUG("Looping over Probes");
  for (const auto probe : *IDTrackContainer) {
   if (probe->charge()*probe_charge != 1)  continue;
   if (!IsProbeParticle(probe, IDTrackContainer)) continue;
   
   //Determine Pair Reconstructed Invariant Mass
   double IVmass      = ( probe->p4() + muon->p4() ).M();
   double DeltaIVmass = fabs(IVmass - 91188);
                                   
   //Determine total mass sigma
   m_oZSignalPlots.m_Z_Mass->Fill(IVmass*0.001); //GeV
                                   
   //Check mass window
   if (DeltaIVmass < m_cut_diMuonMass) continue;
                                   
   //choose Highest Pt Probe
   if (probe->pt() > pT_record) {
     pT_record     = probe->pt();
     RecordProbe    = probe;
   }
  }
  
  ATH_MSG_DEBUG("Suitable Probe found");
  if (RecordProbe) return
  fillTagAndProbe(muon,RecordProbe,MuonJar);
}

//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::IsTagMuon(const xAOD::Muon* muon){
  ATH_MSG_DEBUG("Select Tag...");
  const xAOD::TrackParticle* IDTrackParticle = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if (!IDTrackParticle) return false;
  if ( fabs(IDTrackParticle->eta()) < 0.05 ) return false;
  if (!MCPRecommendationsSatisfied(IDTrackParticle)) return false;                                
  float chi2 = 0;
  //if (!muon->parameter(chi2, xAOD::Muon::msInnerMatchChi2)) return false;
  if ( chi2 > 150. ) return false;
  const xAOD::TrackParticle* MSTrackParticle = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if (!MSTrackParticle) return false;
  float pt_EX = MSTrackParticle->pt();
  float p_EX  = MSTrackParticle->p4().P();
  float p_ID  = IDTrackParticle->p4().P();
  if ( (pt_EX < 50.)  && ( p_EX - p_ID < -0.4 * p_ID )) return false;
  float etcone40 = 1000;
  if (!muon->isolation(etcone40, xAOD::Iso::etcone40)) return false;
  if ( etcone40 > 2000. ) return false;  
  return true;
}

//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::IsZTriggerMatched(const xAOD::Muon* /*muon*/){
  // if (!m_doZTrigMatch) return true;
  // 
  // ATH_MSG_DEBUG("Do Trigger Match...");
  // Trig::FeatureContainer f(m_trigDecTool_b->features(m_tagMuonTrigger));
  // std::vector< Trig::Feature<TrigRoiDescriptor> > ROI_descriptor = f.get<TrigRoiDescriptor>();
  // for (unsigned int j=0; j<ROI_descriptor.size(); j++){
  //   if (fabs(muon.eta() - ROI_descriptor[j].cptr()->eta()) < ROI_descriptor[j].cptr()->etaHalfWidth() &&
  //      fabs(muon.phi() - ROI_descriptor[j].cptr()->phi()) < ROI_descriptor[j].cptr()->phiHalfWidth()){
  //     return true;
  //   }
  // }
  return true;  
}

//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::IsProbeParticle(const xAOD::TrackParticle* probe,  const xAOD::TrackParticleContainer* IDTrackContainer){
  if ( probe->pt() < 20000. ) return false;
  if ( fabs(probe->eta()) < 0.05) return false;
  if (!MCPRecommendationsSatisfied(probe)) return false;
  //if ( probe->fitQuality()->chiSquared() > 150. ) return false; //TODO: Reimplement requirement
  if (!probeIsolation(probe, IDTrackContainer, 0.2)) return false;
  return true;
}

//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::MCPRecommendationsSatisfied(const xAOD::TrackParticle* trk){
   if ( RetrieveHitInfo(trk, xAOD::expectBLayerHit) > 0 && RetrieveHitInfo(trk, xAOD::numberOfBLayerHits) == 0) return false;
   if ( RetrieveHitInfo(trk, xAOD::numberOfPixelHits) + RetrieveHitInfo(trk, xAOD::numberOfPixelDeadSensors) < 2 ) return false;
   if ( RetrieveHitInfo(trk, xAOD::numberOfSCTHits) + RetrieveHitInfo(trk, xAOD::numberOfSCTDeadSensors) < 6 ) return false;
   if ( RetrieveHitInfo(trk, xAOD::numberOfPixelHoles) + RetrieveHitInfo(trk, xAOD::numberOfSCTHoles) > 1 ) return false;
   
   int   nTRTHits       = RetrieveHitInfo(trk, xAOD::numberOfTRTHits);
   int   nTRTOutliers   = RetrieveHitInfo(trk, xAOD::numberOfTRTOutliers);
   if ( fabs(trk->eta()) < 1.9 ) {
     if ( nTRTHits < 6 ) return false;
     if ( nTRTOutliers >= 0.9*(nTRTOutliers + nTRTHits) ) return false;
   }
   else{
     if ( nTRTHits > 5 ) {
       if ( nTRTOutliers >= 0.9*(nTRTOutliers + nTRTHits) ) return false;
     }
   } 
  return true;
}

//---------------------------------------------------------------------------------------
const xAOD::Muon* MuonTrkPhysMonitoring::ProbeToMuonMatching(const xAOD::TrackParticle* RecordProbe, const xAOD::MuonContainer* MuonJar){
   // can this be done with the elementlink only?
   for (const auto muon: *MuonJar ) {
     if(muon->muonType() != xAOD::Muon::Combined) continue;
     const xAOD::TrackParticle* InDetTrackParticle = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
     if (!InDetTrackParticle) continue;
     double dr = DeltaR(RecordProbe->eta(), InDetTrackParticle->eta(), RecordProbe->phi(), InDetTrackParticle->phi());
     if(dr < 0.01) return (muon);
   }
  return nullptr;
}

//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::probeIsolation(const xAOD::TrackParticle *probe, const xAOD::TrackParticleContainer* IDTrackContainer, float cut)
{
  //TODO: Can this be substituted by an already existing algorithm?
  double sumPt=0;
  for( const auto trackParticle : *IDTrackContainer){
   double dR = DeltaR(trackParticle->eta(), probe->eta(), trackParticle->phi(), probe->phi());        
   if ( dR > m_isoConeMin && dR < m_isoConeMax) {
     sumPt+=trackParticle->pt();
   }
  }    
  double iso = sumPt/ probe->pt();
  if (iso > cut ) return false;
  return true;
}
