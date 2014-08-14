/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/OccupancyPlots.h"
#include "MuonTrkPhysMonitoring/MuonTrkPhysMonUtils.h"

using namespace MuonTrkPhysMonUtils;

void OccupancyPlots::initializePlots(){	
  Book2D(m_good_cb_trk_eta_phi, "m_good_cb_trk_eta_phi",m_sTrackCollectionName+": Good Combined Track Occupancy;#eta;", 110,-2.75,2.75,80,1.,17.); //Going to have to find the best binning for this
  Book2D(m_good_me_trk_eta_phi, "m_good_me_trk_eta_phi",m_sTrackCollectionName+": Good Extrapolated Track Occupancy;#eta;", 110,-2.75,2.75,80,1.,17.); //Going to have to find the best binning for this
  Book2D(m_good_id_trk_eta_phi, "m_good_id_trk_eta_phi",m_sTrackCollectionName+": Good (MCP) ID Track Occupancy;#eta;", 110,-2.75,2.75,80,1.,17.); //Going to have to find the best binning for this
  Book2D(m_all_trk_eta_phi, "m_all_trk_eta_phi",m_sTrackCollectionName+": All Track Occupancy;#eta;", 110,-2.75,2.75,80,1.,17.); //Going to have to find the best binning for this
                
  Book1D(m_good_cb_trk_pt, "m_good_cb_trk_pt",m_sTrackCollectionName+": Good Combined Track p_{T};p_{T} [GeV];N_{#mu}", 150,0.,150.);
  Book1D(m_good_me_trk_pt, "m_good_me_trk_pt",m_sTrackCollectionName+": Good Extrapolated Track p_{T};p_{T} [GeV];N_{#mu}", 150,0.,150.);
  Book1D(m_good_id_trk_pt, "m_good_id_trk_pt",m_sTrackCollectionName+": (MCP) ID Track p_{T};p_{T} [GeV];N_{#mu}", 150,0.,150.);
  Book1D(m_all_trk_pt, "m_all_trk_pt",m_sTrackCollectionName+": All Track p_{T};p_{T} [GeV];N_{#mu}", 150,0.,150.);

  LabelSectorAxis(m_good_cb_trk_eta_phi->GetYaxis());
  LabelSectorAxis(m_good_me_trk_eta_phi->GetYaxis());
  LabelSectorAxis(m_good_id_trk_eta_phi->GetYaxis());
  LabelSectorAxis(m_all_trk_eta_phi->GetYaxis());
}

void OccupancyPlots::fill( const xAOD::Muon* muon, bool bPassIDTrackQuality, bool bPassMSTrackQuality ){
  FillPlots(m_all_trk_eta_phi, m_all_trk_pt, muon);  
  if (bPassMSTrackQuality) FillPlots(m_good_me_trk_eta_phi, m_good_me_trk_pt, muon);
  if (bPassIDTrackQuality) FillPlots(m_good_id_trk_eta_phi, m_good_id_trk_pt, muon);
  if ( bPassIDTrackQuality && bPassMSTrackQuality ) FillPlots( m_good_cb_trk_eta_phi, m_good_cb_trk_pt, muon);
}

void OccupancyPlots::FillPlots(TH2F* trk_eta_phi, TH1F* trk_pt, const xAOD::Muon* muon){
  trk_eta_phi->Fill(muon->eta(),PhiSector(muon->phi()));
  trk_pt->Fill(muon->pt()*0.001);  
}
