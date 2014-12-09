/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ================================================================================
   MTPM_CbMSIDAlign.cxx
   ------------------------------
   AUTHORS:     Nektarios Chr.Benekos, Austin Basye , Christina G.Skarpathiotaki (UoP)
   created:     September 2013
   description: Implementation code for the MTPM_CbMSIDAlign
   ============================================================================== */

#include "MuonTrkPhysMonitoring/MuonTrkPhysMonitoring.h"

//using namespace std;

/** bookHistograms */
//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::bookHistograms_CbMSIDAlign(std::string sPath)
{
  ATH_MSG_DEBUG("Booking Histograms for bookHistograms_CombinedMSIDAlignement");
  InitializePlots(m_oCbMSIDAlignPlots, MonGroup(this, sPath, run, ATTRIB_UNMANAGED));
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::fillHistograms_CbMSIDAlign(const xAOD::Muon* Muon, const xAOD::MuonContainer* MuonJar)
{
  ATH_MSG_DEBUG("Filling Histograms for fillHistograms_CombinedMSIDAlignement");

  const xAOD::TrackParticle* MSTrackParticle = Muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if(!MSTrackParticle) return;
  const Trk::Perigee& measPerigeeMS = MSTrackParticle->perigeeParameters();
  
  const xAOD::TrackParticle* InDetTrackParticle = Muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if(!InDetTrackParticle) return;
  const Trk::Perigee& measPerigeeID = InDetTrackParticle->perigeeParameters();
           
  bool muonPassesTrackCriteria = passIDTrackQuality(InDetTrackParticle) && passMSTrackQuality(Muon,2,2,2,8);  
  if (AthenaMonManager::dataType() == AthenaMonManager::heavyIonCollisions || AthenaMonManager::dataType() == AthenaMonManager::cosmics) {
    if(!muonPassesTrackCriteria) return;
    m_oCbMSIDAlignPlots.fill(Muon, &measPerigeeID, &measPerigeeMS);
  }
  else {    
    if(!(muonPassesTrackCriteria && passZSelection(Muon, MuonJar))) return;
    m_oCbMSIDAlignPlots.fill(Muon, &measPerigeeID, &measPerigeeMS);
  }   
}

//---------------------------------------------------------------------------------------
void MuonTrkPhysMonitoring::procHistograms_CbMSIDAlign()
{ 
  ATH_MSG_DEBUG("Processing Histograms for bookHistograms_CombinedMSIDAlignement");
  m_oCbMSIDAlignPlots.finalize();	
}


//---------------------------------------------------------------------------------------
bool MuonTrkPhysMonitoring::passZSelection(const xAOD::Muon *Muon, const xAOD::MuonContainer* MuonJar){
  float etcone20; float ptcone30;
  if (!Muon->isolation(etcone20, xAOD::Iso::ptcone20) || !Muon->isolation(ptcone30, xAOD::Iso::ptcone30)) return false;
  if (etcone20 >= 4000) return false;
  if (ptcone30 >= 2500) return false;
   
   // TODO muon selector tool usage  
   for (const auto muonIt: *MuonJar) {
     const xAOD::TrackParticle* primaryTrackParticle = Muon->trackParticle(xAOD::Muon::Primary);
     if (!primaryTrackParticle) continue;
     if (!passIDTrackQuality(primaryTrackParticle)) continue;        
     if (!muonIt->isolation(etcone20, xAOD::Iso::ptcone20) || !muonIt->isolation(ptcone30, xAOD::Iso::ptcone30)) continue;
     if (etcone20 >= 4000) return false;
     if (ptcone30 >= 2500) return false;
                         
     if ((Muon->p4() + muonIt->p4()).M() > 70000) return true;
   }    
  return false;    
}
