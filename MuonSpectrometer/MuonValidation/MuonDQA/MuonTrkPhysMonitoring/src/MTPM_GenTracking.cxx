/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ================================================================================
   MTPM_GenTracking.cxx
   ------------------------------
   AUTHORS:     Nektarios Chr.Benekos, Austin Basye , Christina G.Skarpathiotaki (UoP)
   created:     September 2013
   description: Implementation code for the MTPM_GenTracking
   ============================================================================== */

#include "MuonTrkPhysMonitoring/MuonTrkPhysMonitoring.h"

using namespace MuonTrkPhysMonUtils;

/** bookHistograms */
//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::bookHistograms_GenTracking(std::string sPath)
{
  ATH_MSG_DEBUG("Booking Histograms for bookHistograms_GenTracking");
  InitializePlots(m_oGenTrackingPlots, MonGroup(this, sPath, run, ATTRIB_UNMANAGED));
}

/** fillHistograms */
//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::fillHistograms_GenTracking(const xAOD::Muon *Muon){

  const xAOD::TrackParticle *MSTrackParticle = Muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if (!MSTrackParticle) return;
  const Trk::Track* pMuExTrk = MSTrackParticle->track();
  if (!pMuExTrk) return;

  // Muon Extrapolated Track Pointer
  const  Trk::MuonTrackSummary* pMuExTrkSummary = pMuExTrk->trackSummary()->muonTrackSummary();
  if(!pMuExTrkSummary){
    ATH_MSG_WARNING(" Muon Extrapolated Track Summary is not avaliable! GenTracking Histograms not filled!");
    return; 
  }
  
  // Get Chamber Hit Summary
  const std::vector<Trk::MuonTrackSummary::ChamberHitSummary> MuExtChamberHitSummary = pMuExTrkSummary->chamberHitSummary();    
  if(!MuExtChamberHitSummary.size()){
    ATH_MSG_WARNING(" Chamber Hit Summary is not empty! GenTracking Histograms not filled!");
    return;
  }
    
  int    nPrecisionHits = RetrieveHitInfo(MSTrackParticle, xAOD::numberOfPrecisionLayers);
  int    nPhiHits       = RetrieveHitInfo(MSTrackParticle, xAOD::numberOfPhiLayers);
  float  MuonEta        = MSTrackParticle->eta();

  m_oGenTrackingPlots.m_N_Precision->Fill(nPrecisionHits); //<-- this will provide us with a sigma(nprecisionHits) for the error calc.
  m_oGenTrackingPlots.m_N_Precision_weights->Fill(EtaRegion(MuonEta),nPrecisionHits);
  m_oGenTrackingPlots.m_N_Precision_entries->Fill(EtaRegion(MuonEta));
    
  m_oGenTrackingPlots.m_N_Phi_Hits->Fill(nPhiHits); //<-- this will provide us with a sigma(nphi) for the error calc.
  m_oGenTrackingPlots.m_N_Phi_Hits_weights->Fill(EtaRegion(MuonEta),nPhiHits);
  m_oGenTrackingPlots.m_N_Phi_Hits_entries->Fill(EtaRegion(MuonEta));
    

  // Loop Over Chamber Hit Locations //TODO: make this work again, weird incomplete type 'const class Trk::TrackSummary' error
  for (unsigned int k = 0; k < ((unsigned int) MuExtChamberHitSummary.size()); k++) {
        
    const Identifier& Id = MuExtChamberHitSummary[k].chamberId();        
    bool isBarrelA=false, isBarrelC=false, isEndcapA=false, isEndcapC=false;
        
    if (!(m_idHelperTool->isEndcap(Id)) && MuonEta > 0.0 )  isBarrelA=true;
    if (!(m_idHelperTool->isEndcap(Id)) && MuonEta < 0.0 )  isBarrelC=true;
    if ( (m_idHelperTool->isEndcap(Id)) && MuonEta > 0.0 )  isEndcapA=true;
    if ( (m_idHelperTool->isEndcap(Id)) && MuonEta < 0.0 )  isEndcapC=true;
        
    int test = int(isBarrelA) + int(isBarrelC) + int(isEndcapA) + int(isEndcapC);
        
    if (test != 1) {
      if(test < 1) ATH_MSG_WARNING("Segment not identified with global geometry location!");
      if(test > 1) ATH_MSG_WARNING("Segment identified with too many global geometry locations!");
    }
  
  
    //Fill Hits Per Track Histograms
    if( m_idHelperTool->isRpc(Id) ){
      if      (isBarrelA) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_RPC_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_RPC_Entries, MTPM_Location_Bins::RPC_A, MuExtChamberHitSummary[k].nholes());
      else if (isBarrelC) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_RPC_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_RPC_Entries, MTPM_Location_Bins::RPC_C, MuExtChamberHitSummary[k].nholes());
    }
    if( m_idHelperTool->isMdt(Id) ){
      if      (isBarrelA) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_MDT_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_MDT_Entries, MTPM_Location_Bins::MDT_BA,MuExtChamberHitSummary[k].nholes());
      else if (isBarrelC) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_MDT_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_MDT_Entries, MTPM_Location_Bins::MDT_BC,MuExtChamberHitSummary[k].nholes());
      else if (isEndcapA) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_MDT_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_MDT_Entries, MTPM_Location_Bins::MDT_EA,MuExtChamberHitSummary[k].nholes());
      else if (isEndcapC) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_MDT_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_MDT_Entries, MTPM_Location_Bins::MDT_EC,MuExtChamberHitSummary[k].nholes());
    }
    if( m_idHelperTool->isTgc(Id) ){
      if      (isEndcapA) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_TGC_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_TGC_Entries, MTPM_Location_Bins::TGC_A,MuExtChamberHitSummary[k].nholes());
      else if (isEndcapC) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_TGC_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_TGC_Entries, MTPM_Location_Bins::TGC_C,MuExtChamberHitSummary[k].nholes());
    }
    if( m_idHelperTool->isCsc(Id) ){
      if      (isEndcapA) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_CSC_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_CSC_Entries, MTPM_Location_Bins::CSC_A,MuExtChamberHitSummary[k].nholes());
      else if (isEndcapC) FillMSPlots(m_oGenTrackingPlots.m_N_HpT_pLB_CSC_Weights, m_oGenTrackingPlots.m_N_HpT_pLB_CSC_Entries, MTPM_Location_Bins::CSC_C,MuExtChamberHitSummary[k].nholes());
    }
        
 }// End Chamber Hit Summary Loop    
  ATH_MSG_DEBUG("End Gen. Tracking Performance Monitoring...");
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::procHistograms_GenTracking()
{
  m_oGenTrackingPlots.finalize();  
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::fillT0Plots(const xAOD::MuonSegmentContainer* MuonSegmentContainer, const xAOD::TrackParticle* MSTrackParticle, const xAOD::Muon* muon, bool extendedPlots ) {
  const Trk::Track* MuTrkME = MSTrackParticle->track();
  if (!MuTrkME) return;
  if( !(MuonSegmentContainer && !MuonSegmentContainer->empty())) return; 
  ATH_MSG_DEBUG("Begin Segment Analysis...");
  
  //Loop over segms
  int nSegments = 0;
  
  ATH_MSG_DEBUG("Inside Segment Loop...");
  for (auto segment: *MuonSegmentContainer){
    if(!matchSegmentTrk(MuTrkME,segment)) continue;                                    
    nSegments++;
                                    
    ATH_MSG_DEBUG(  "MuonSegment chi2, ndof " << (float)segment->chiSquared() << "/" <<  (int)segment->numberDoF() );
    double t0  = segment->t0();
  
//    if( !m_idHelperTool->isMdt(segment->chamberIndex()) ) continue; //TODO how to decide this using xAOD::MuonSegment?
    if( extendedPlots ){
  
      m_oGenTrackingPlots.m_N_Hits_Segment->Fill(segment->nPrecisionHits());
      m_oGenTrackingPlots.m_N_Hits_Segment_weights->Fill(EtaRegion(muon->eta()),segment->nPrecisionHits());
      m_oGenTrackingPlots.m_N_Hits_Segment_entries->Fill(EtaRegion(muon->eta()));
  
      // Muon::MuonStationIndex::StIndex stIndex = segment->sector();  //TODO station information unavailable
    //   FillT02DPlots(m_oGenTrackingPlots.m_t0_eta_phi, m_oGenTrackingPlots.m_t0_eta_phi_weights, m_oGenTrackingPlots.m_t0_eta_phi_entries, muon->eta(), PhiSector(muon->phi()),t0 );
    //   if ( stIndex == Muon::MuonStationIndex::EI || stIndex == Muon::MuonStationIndex::BI ) FillT02DPlots(m_oGenTrackingPlots.m_t0_eta_phi_i, m_oGenTrackingPlots.m_t0_eta_phi_i_weights, m_oGenTrackingPlots.m_t0_eta_phi_i_entries, muon->eta(), PhiSector(muon->phi()),t0 );
    //   if ( stIndex == Muon::MuonStationIndex::EM || stIndex == Muon::MuonStationIndex::BM || 
    //        stIndex == Muon::MuonStationIndex::EE || stIndex == Muon::MuonStationIndex::BE ) FillT02DPlots(m_oGenTrackingPlots.m_t0_eta_phi_m, m_oGenTrackingPlots.m_t0_eta_phi_m_weights, m_oGenTrackingPlots.m_t0_eta_phi_m_entries, muon->eta(), PhiSector(muon->phi()),t0 );
    //   if ( stIndex == Muon::MuonStationIndex::EO || stIndex == Muon::MuonStationIndex::BO ) FillT02DPlots(m_oGenTrackingPlots.m_t0_eta_phi_o, m_oGenTrackingPlots.m_t0_eta_phi_o_weights, m_oGenTrackingPlots.m_t0_eta_phi_o_entries, muon->eta(), PhiSector(muon->phi()),t0 );
                                                                                
      if (EtaRegionSmall(muon->eta()) == 0.5){ m_oGenTrackingPlots.m_t0_LB_EC_weights->Fill(m_lumiblock,t0); m_oGenTrackingPlots.m_t0_LB_EC_entries->Fill(m_lumiblock);}
      if (EtaRegionSmall(muon->eta()) == 1.5){ m_oGenTrackingPlots.m_t0_LB_BC_weights->Fill(m_lumiblock,t0); m_oGenTrackingPlots.m_t0_LB_BC_entries->Fill(m_lumiblock);}
      if (EtaRegionSmall(muon->eta()) == 2.5){ m_oGenTrackingPlots.m_t0_LB_BA_weights->Fill(m_lumiblock,t0); m_oGenTrackingPlots.m_t0_LB_BA_entries->Fill(m_lumiblock);}
      if (EtaRegionSmall(muon->eta()) == 3.5){ m_oGenTrackingPlots.m_t0_LB_EA_weights->Fill(m_lumiblock,t0); m_oGenTrackingPlots.m_t0_LB_EA_entries->Fill(m_lumiblock);}      
    }
                                                
    if (segment->t0error() > 5.0)  continue;
    int region = getDetectorRegion(muon->eta(),muon->phi());
    m_oOnlinePlots.fill(region, t0);
  }
  
  m_oGenTrackingPlots.m_N_Segment_Trk->Fill(nSegments);
  m_oGenTrackingPlots.m_N_Segment_Trk_weights->Fill(EtaRegion(muon->eta()),nSegments);
  m_oGenTrackingPlots.m_N_Segment_Trk_entries->Fill(EtaRegion(muon->eta()));
  
  ATH_MSG_DEBUG("End Segment Analysis...");
}

void MuonTrkPhysMonitoring::FillT02DPlots(TH1F* hist, TH2F* weightshist, TH2F* entrieshist, float xValue, float yValue, float info){
  hist->Fill(info);
  weightshist->Fill(xValue, yValue, info);
  entrieshist->Fill(xValue, yValue);
}

//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::matchSegmentTrk(const Trk::Track* origTrk, const xAOD::MuonSegment* seg) 
{
   // TODO: Prescription was adjusted to xAOD use, but may be wrong, please check!
   const DataVector<const Trk::TrackStateOnSurface>* states = origTrk->trackStateOnSurfaces();    
  
   //Look for matching measurement on the track
   for(DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin(); tsit!=states->end() ; ++tsit)  {
     const Trk::MeasurementBase* measurement = (*tsit)->measurementOnTrack();
     if( (measurement == nullptr) || (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;
           
     Identifier id = m_helperTool->getIdentifier(*measurement);            
     if(!m_idHelperTool->isMdt(id) && !m_idHelperTool->isCsc(id)) continue;
     //Check if the fixed ids from segment and track match
     if(seg->sector() == m_idHelperTool->sector(id) &&
        seg->chamberIndex() == m_idHelperTool->chamberIndex(id) &&
        seg->etaIndex() == m_idHelperTool->regionIndex(id) &&
        seg->technology() == m_idHelperTool->technologyIndex(id)) return true;
  }
  return false;    
}

//---------------------------------------------------------------------------------------
int MuonTrkPhysMonitoring::retrieveLumiBlock(){

  const xAOD::EventInfo* eventInfo = evtStore()->retrieve<const xAOD::EventInfo>("EventInfo");
  if (!eventInfo){
    ATH_MSG_ERROR("Could not retrieve EventInfo. LumiBlock set to -1");
    return -1;
  } 
  return eventInfo->lumiBlock();
}

//--------------------------------------------------------------------------------------

void MuonTrkPhysMonitoring::FillMSPlots(TH2F *hWeights, TH2F *hEntries, int location, float info){
	hWeights->Fill(m_lumiblock,location, info);
	hEntries->Fill(m_lumiblock,location); 
}