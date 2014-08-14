/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_GENTRACKINGPLOTS_H
#define MUONTRKPHYSMONITORING_GENTRACKINGPLOTS_H

#include "PlotBase.h"
#include "ResidualPlots.h"
#include "TrkParameters/TrackParameters.h"

class GenTrackingPlots: public PlotBase {
  public:
    GenTrackingPlots(PlotBase *pParent);

    //N_MU/lumi v eta
    TH2F* m_N_Mu_LB_per_DRegion;
    
    //N_MU
    TH2F* m_N_HpT_pLB_CSC;
    TH2F* m_N_HpT_pLB_CSC_Weights;
    TH2F* m_N_HpT_pLB_CSC_Entries;
    
    TH2F* m_N_HpT_pLB_TGC;
    TH2F* m_N_HpT_pLB_TGC_Weights;
    TH2F* m_N_HpT_pLB_TGC_Entries;
    
    TH2F* m_N_HpT_pLB_MDT;
    TH2F* m_N_HpT_pLB_MDT_Weights;
    TH2F* m_N_HpT_pLB_MDT_Entries;
    
    TH2F* m_N_HpT_pLB_RPC;
    TH2F* m_N_HpT_pLB_RPC_Weights;
    TH2F* m_N_HpT_pLB_RPC_Entries;
    
    
    //N_Mu/Lumi
    TH1F* m_N_Mu_Lumi;
    TH1F* m_N_Mu_Lumi_Run;
    TH1F* m_N_Mu_Lumi_LB;
	
    //<N Percision>  v eta
    TH1F* m_N_Precision;
    
    TH1F* m_N_Precision_entries;
    TH1F* m_N_Precision_weights;
    TH1F* m_N_Precision_mean;
	
    //<N Phi Hits> v eta
    TH1F* m_N_Phi_Hits;
	
    TH1F* m_N_Phi_Hits_entries;
    TH1F* m_N_Phi_Hits_weights;
    TH1F* m_N_Phi_Hits_mean;
    
    //N_Hit/segment v eta
    TH1F* m_N_Hits_Segment;
	
    TH1F* m_N_Hits_Segment_entries;
    TH1F* m_N_Hits_Segment_weights;
    TH1F* m_N_Hits_Segment_mean;
    
    //N_Segments/Track v eta
    TH1F* m_N_Segment_Trk;
	
    TH1F* m_N_Segment_Trk_entries;
    TH1F* m_N_Segment_Trk_weights;
    TH1F* m_N_Segment_Trk_mean;
        
    //Refitted t0 v eta v phi
    TH1F* m_t0_eta_phi;
	
    TH2F* m_t0_eta_phi_entries;
    TH2F* m_t0_eta_phi_weights;
    TH2F* m_t0_eta_phi_mean;
    
    TH1F* m_t0_eta_phi_i;
	
    TH2F* m_t0_eta_phi_i_entries;
    TH2F* m_t0_eta_phi_i_weights;
    TH2F* m_t0_eta_phi_i_mean;
    
    TH1F* m_t0_eta_phi_m;
	
    TH2F* m_t0_eta_phi_m_entries;
    TH2F* m_t0_eta_phi_m_weights;
    TH2F* m_t0_eta_phi_m_mean;
    
    TH1F* m_t0_eta_phi_o;
	
    TH2F* m_t0_eta_phi_o_entries;
    TH2F* m_t0_eta_phi_o_weights;
    TH2F* m_t0_eta_phi_o_mean;
    
    TH1F* m_Good_t0;
    TH1F* m_Good_t0_EA;
    TH1F* m_Good_t0_BA;
    TH1F* m_Good_t0_BC;
    TH1F* m_Good_t0_EC; 
    
    TH1F* m_t0_LB_EA_entries;    
    TH1F* m_t0_LB_EA_weights;    
    TH1F* m_t0_LB_EA_mean;    
    
    TH1F* m_t0_LB_BA_entries;    
    TH1F* m_t0_LB_BA_weights;    
    TH1F* m_t0_LB_BA_mean;    
    
    TH1F* m_t0_LB_BC_entries;    
    TH1F* m_t0_LB_BC_weights;    
    TH1F* m_t0_LB_BC_mean;    
    
    TH1F* m_t0_LB_EC_entries;    
    TH1F* m_t0_LB_EC_weights;   
    TH1F* m_t0_LB_EC_mean;   
    
    TH1F* m_Buffer_Heap_Protection; //Prevent Heap Corruption Seen Recently 

		
	
  private:
    void FinalizeGoodt0Results(TH1F* hist, int iBin, std::string sLabel);
    void FinalizeT0Plots(TH2F* hist_weights, TH2F* hist_entries, TH2F* hist_mean);
    void FinalizeMeanHistograms(TH1F* hist_overall, TH1F* hist_entries, TH1F* hist_weights, TH1F* hist_mean);
    void SetLabels(TH2F* hist,TH2F* hist_weights,TH2F* hist_entries, std::vector<std::pair<int,std::string>> vLabels);
		void CalculateSectorResults(TH1F* summaryPlot, TH1F* inputPlot, int sector);
    void initializePlots();
		void finalizePlots();

};

#endif
