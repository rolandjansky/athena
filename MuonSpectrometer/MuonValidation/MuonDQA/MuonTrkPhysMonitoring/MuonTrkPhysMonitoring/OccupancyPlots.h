/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_OCCUPANCYPLOTS_H
#define MUONTRKPHYSMONITORING_OCCUPANCYPLOTS_H

#include "PlotBase.h"
#include "xAODMuon/Muon.h"

class OccupancyPlots: public PlotBase {
  public:
    OccupancyPlots(PlotBase *pParent):PlotBase(pParent){;}
    void fill( const xAOD::Muon* muon, bool bPassIDTrackQuality, bool bPassMSTrackQuality );

    TH2F* m_all_trk_eta_phi;
    TH2F* m_good_id_trk_eta_phi;
    TH2F* m_good_cb_trk_eta_phi;
    TH2F* m_good_me_trk_eta_phi;
    
    TH1F* m_all_trk_pt;
    TH1F* m_good_id_trk_pt;
    TH1F* m_good_cb_trk_pt;
    TH1F* m_good_me_trk_pt;
			
  private:
    void FillPlots(TH2F* trk_eta_phi, TH1F* trk_pt, const xAOD::Muon* muon);
    void initializePlots();
    void finalizePlots(){;}

};

#endif





