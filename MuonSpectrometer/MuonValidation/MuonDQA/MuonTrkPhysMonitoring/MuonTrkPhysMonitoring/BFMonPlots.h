/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_BFMONPLOTS_H
#define MUONTRKPHYSMONITORING_BFMONPLOTS_H

#include "PlotBase.h"
#include "xAODMuon/Muon.h"

class BFMonPlots: public PlotBase {
  public:
    BFMonPlots(PlotBase *pParent):PlotBase(pParent){;}
    void fill(const xAOD::Muon *Muon, float fSagitta );
    
    TH1F* m_LSag;
    TH1F* m_LSag_weights;
    TH1F* m_LSag_entries;
    TH1F* m_LSag_mean;
    
    TH2F* m_LSag_eta_phi_weights;
    TH2F* m_LSag_eta_phi_entries;
    TH2F* m_LSag_eta_phi_mean;
    	
			
  private:
    void initializePlots();
    void finalizePlots();

};

#endif





