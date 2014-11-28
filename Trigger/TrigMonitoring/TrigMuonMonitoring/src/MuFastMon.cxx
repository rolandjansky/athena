/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile MuFastMon.cxx
 *   
 *    authors: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)             
 *             Kunihiro Nagano (nagano@mail.cern.ch)
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"


#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigMuonMonitoring/HLTMuonMonTool.h"

//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

using namespace std;

StatusCode HLTMuonMonTool::initMuFastDQA()
{
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::bookMuFastDQA()
{

  //histograms in each 10LBs 
  if( newRun || newLowStat){

    addHistogram( new TH2F("muFast_eta_vs_phi_in_10LBs",           "muFast eta vs phi in 10LBs; #eta ; #phi",           27, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi), histdircoverage );

  }

  if( newRun ){

    // basic EDM variables
    addHistogram( new TH1F("muFast_pt_all",        "muFast pt (GeV/c); p_{T} [GeV/c]; Entries",           210, -105., 105.), histdirmufast );
    addHistogram( new TH1F("muFast_pt_all_barrel", "muFast pt barrel (GeV/c); p_{T} [GeV/c]; Entries",    210, -105., 105.), histdirmufast );
    addHistogram( new TH1F("muFast_pt_all_endcap", "muFast pt endcap (GeV/c); p_{T} [GeV/c]; Entires",    210, -105., 105.), histdirmufast );
    addHistogram( new TH1F("muFast_pt",            "muFast pt (GeV/c); p_{T} [GeV/c]; Entries",           210, -105., 105.), histdirmufast );
    addHistogram( new TH1F("muFast_pt_barrel",     "muFast pt barrel (GeV/c); p_{T} [GeV/c]; Entries",    210, -105., 105.), histdirmufast );
    addHistogram( new TH1F("muFast_pt_endcap",     "muFast pt endcap (GeV/c); p_{T} [GeV/c]; Entires",    210, -105., 105.), histdirmufast );
    addHistogram( new TH1F("muFast_eta",           "muFast eta; #eta; Entries",                108, -2.7,  2.7), histdirmufast );
    addHistogram( new TH1F("muFast_phi",           "muFast phi; #phi[rad]; Entries",                96, -CLHEP::pi, CLHEP::pi), histdirmufast );
    addHistogram( new TH1F("muFast_phi_barrel",    "muFast phi barrel; #phi[rad]; Entries",         96, -CLHEP::pi, CLHEP::pi), histdirmufast );
    addHistogram( new TH1F("muFast_phi_endcap",    "muFast phi endcap; #phi[rad]; Entries",         96, -CLHEP::pi, CLHEP::pi), histdirmufast );
    addHistogram( new TH2F("muFast_eta_vs_phi",    "muFast eta vs phi; #eta ; #phi",           108, -2.7,  2.7, 96, -CLHEP::pi, CLHEP::pi), histdirmufast );
    
    // position and superpoint
    addHistogram( new TH1F("muFast_saddr",             "muFast station address; address; Entries",             6, -1, 5), histdirmufast );
    addHistogram( new TH2F("muFast_MDTpoints_z_vs_r",  "muFast MDT superpoint Z vs R (mm); Z[mm]; R[mm]",  200, -24000, 24000, 200, -14000, 14000), histdirmufast );

    // RoI ID in case mF failed
    addHistogram( new TH1F("muFast_failed_roiId",   "muFast roiId in case of failure; RoIID; Entries",  100, 0, 100000), histdirmufast ); 

    // RoI descriptor
    addHistogram( new TH1F("muFast_forID_dR_toRecMuonCB_mu6",  "muFast forID RoI, dR to offline CB; dR(forID vs offl CB); Entries",  100, 0, 0.5), histdirmufast ); 
    addHistogram( new TH2F("muFast_forID_deta_vs_eta_toRecMuonCB_mu6",  "muFast forID RoI wrt Offline CB muon, d#eta ; offl CB #eta; d#eta(forID vs offl CB)",
			   54, -2.7, 2.7, 60, -0.3, 0.3),  histdirmufast ); 
    addHistogram( new TH2F("muFast_forID_dphi_vs_eta_toRecMuonCB_mu6",  "muFast forID RoI wrt Offline CB muon, d#phi ; offl CB #eta; d#phi(forID vs offl CB)",
			   54, -2.7, 2.7, 44, -0.02, 0.2), histdirmufast ); 

    // MuonFeatureDetails
    addHistogram( new TH1F("muFast_proc_flow",   "muFast process flow;; Entries",  6, 1, 7), histdirmufast ); 
    hist("muFast_proc_flow", histdirmufast)->GetXaxis()->SetBinLabel(1,"input");
    hist("muFast_proc_flow", histdirmufast)->GetXaxis()->SetBinLabel(2,"n L1 hits > 0");
    hist("muFast_proc_flow", histdirmufast)->GetXaxis()->SetBinLabel(3,"L1 emu ok at trigger layer");
    hist("muFast_proc_flow", histdirmufast)->GetXaxis()->SetBinLabel(4,"n MDT hits > 0 at middle layer");
    hist("muFast_proc_flow", histdirmufast)->GetXaxis()->SetBinLabel(5,"MDT fit ok at middle layer");
    hist("muFast_proc_flow", histdirmufast)->GetXaxis()->SetBinLabel(6,"MDT fit ok at >= 2 layers");
    
    addHistogram( new TH1F("muFast_RPC_Pad_N",   "muFast RPC number of hits; RPC number of hits; Entries",  20, 0, 20), histdirmufast ); 

    addHistogram( new TH1F("muFast_TGC_Mid_rho_chi2",   "muFast TGC big wheel rho fit chi2; chi2; Entries",  100, 0, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_TGC_Mid_phi_chi2",   "muFast TGC big wheel phi fit chi2; chi2; Entries",  100, 0, 10), histdirmufast ); 

    addHistogram( new TH1F("muFast_TGC_Mid_rho_N",   "muFast TGC big wheel number of hits in rho; TGC BW rho nhits; Entries",  20, 0, 20), histdirmufast ); 
    addHistogram( new TH1F("muFast_TGC_Mid_phi_N",   "muFast TGC big wheel number of hits in eta; TGC BW phi nhtis; Entries",  20, 0, 20), histdirmufast ); 
    
    addHistogram( new TH1F("muFast_TGC_Inn_rho_N",   "muFast TGC small wheel number of hits in rho; TGC SW rho nhits; Entries",  10, 0, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_TGC_Inn_phi_N",   "muFast TGC small wheel number of hits in eta; TGC SW phi nhtis; Entries",  10, 0, 10), histdirmufast ); 
    
    addHistogram( new TH1F("muFast_MDT_Inn_fit_chi2_barrel",   "muFast barrel MDT Inner station fit chi2; chi2; Entries",  100, 0, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Mid_fit_chi2_barrel",   "muFast barrel MDT Middle station fit chi2; chi2; Entries",  100, 0, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Out_fit_chi2_barrel",   "muFast barrel MDT Outer station fit chi2; chi2; Entries",  100, 0, 10), histdirmufast ); 

    addHistogram( new TH1F("muFast_MDT_Inn_fit_chi2_endcap",   "muFast endcap MDT Inner station fit chi2; chi2; Entries",  100, 0, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Mid_fit_chi2_endcap",   "muFast endcap MDT Middle station fit chi2; chi2; Entries",  100, 0, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Out_fit_chi2_endcap",   "muFast endcap MDT Outer station fit chi2; chi2; Entries",  100, 0, 10), histdirmufast ); 

    addHistogram( new TH1F("muFast_MDT_N_barrel",   "muFast barrel MDT number of hits; MDT nhits; Entries",  40, 0, 40), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_N_endcap",   "muFast endcap MDT number of hits; MDT nhits; Entries",  40, 0, 40), histdirmufast ); 

    addHistogram( new TH1F("muFast_MDT_Inn_residual_barrel",   "muFast barrel MDT Inner  station residual; MDT Inner barrel residual [cm]; Entries",  100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Mid_residual_barrel",   "muFast barrel MDT Middle station residual; MDT Middle barrel residual [cm]; Entries", 100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Out_residual_barrel",   "muFast barrel MDT Outer  station residual; MDT Outer barrel residual [cm]; Entries",  100, -10, 10), histdirmufast ); 
	 addHistogram( new TH1F("muFast_MDT_Inn_residual_barrel_OffMatch",   "muFast barrel MDT Inner  station residual matched with Offline; MDT Inner barrel residual [cm]; Entries", 100, -10, 10), histdirmufast ); 
	 addHistogram( new TH1F("muFast_MDT_Mid_residual_barrel_OffMatch",   "muFast barrel MDT Middle station residual matched with Offline; MDT Middle barrel residual [cm]; Entries", 100, -10, 10), histdirmufast ); 
	 addHistogram( new TH1F("muFast_MDT_Out_residual_barrel_OffMatch",   "muFast barrel MDT Outer  station residual matched with Offline; MDT Outer barrel residual [cm]; Entries", 100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Inn_N_barrel",   "muFast barrel MDT Inner  number of hits; MDT nhits Inner; Entries",  40, 0, 40), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Mid_N_barrel",   "muFast barrel MDT Middle number of hits; MDT nhits Middle; Entries",  40, 0, 40), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Out_N_barrel",   "muFast barrel MDT Outer  number of hits; MDT nhits Outer; Entries",  40, 0, 40), histdirmufast ); 

    addHistogram( new TH1F("muFast_MDT_Inn_residual_endcap",   "muFast endcap MDT Inner  station residual; MDT Inner endcap residual [cm]; Entries",  100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Mid_residual_endcap",   "muFast endcap MDT Middle station residual; MDT Middle endcap residual [cm]; Entries", 100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Out_residual_endcap",   "muFast endcap MDT Outer  station residual; MDT Outer endcap residual [cm]; Entries",  100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Inn_residual_endcap_OffMatch",   "muFast endcap MDT Inner  station residual matched with Offline; MDT Inner endcap residual [cm]; Entries", 100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Mid_residual_endcap_OffMatch",   "muFast endcap MDT Middle station residual matched with Offline; MDT Middle endcap residual [cm]; Entries", 100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Out_residual_endcap_OffMatch",   "muFast endcap MDT Outer  station residual matched with Offline; MDT Outer endcap residual [cm]; Entries", 100, -10, 10), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Inn_N_endcap",   "muFast endcap MDT Inner  number of hits; MDT nhits Inner; Entries",  40, 0, 40), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Mid_N_endcap",   "muFast endcap MDT Middle number of hits; MDT nhits Middle; Entries",  40, 0, 40), histdirmufast ); 
    addHistogram( new TH1F("muFast_MDT_Out_N_endcap",   "muFast endcap MDT Outer  number of hits; MDT nhits Outer; Entries",  40, 0, 40), histdirmufast ); 

    // Comparison to Offline
    addHistogram( new TH1F("muFast_dR_toRecMuonCB",          "dR between muFast and Offline; #DeltaR; Entries",           100,  0,  2), histdirmufast );
    addHistogram( new TH1F("muFast_ptresol_toRecMuonCB",     "muFast pT resolution wrt Offline; p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos",     "muFast pT resolution wrt Offline positive muons; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg",     "muFast pT resolution wrt Offline negative muons; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_ptresol_toRecMuonCB_BR",  "muFast pT resolution wrt Offline Barrel; p_{T} resol; Entries", 100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_ptresol_toRecMuonCB_EC1", "muFast pT resolution wrt Offline EndCap1; p_{T} resol; Entries",100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_ptresol_toRecMuonCB_EC2", "muFast pT resolution wrt Offline EndCap2; p_{T} resol; Entries",100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_ptresol_toRecMuonCB_EC3", "muFast pT resolution wrt Offline EndCap3; p_{T} resol; Entries",100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos_BR_A",     "muFast pT resolution wrt Offline pos muons Barrel A-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg_BR_A",     "muFast pT resolution wrt Offline neg muons Barrel A-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos_EC1_A",     "muFast pT resolution wrt Offline pos muons EndCap1 A-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg_EC1_A",     "muFast pT resolution wrt Offline neg muons EndCap1 A-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos_EC2_A",     "muFast pT resolution wrt Offline pos muons EndCap2 A-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg_EC2_A",     "muFast pT resolution wrt Offline neg muons EndCap2 A-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos_EC3_A",     "muFast pT resolution wrt Offline pos muons EndCap3 A-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg_EC3_A",     "muFast pT resolution wrt Offline neg muons EndCap3 A-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos_BR_C",     "muFast pT resolution wrt Offline pos muons Barrel C-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg_BR_C",     "muFast pT resolution wrt Offline neg muons Barrel C-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos_EC1_C",     "muFast pT resolution wrt Offline pos muons EndCap1 C-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg_EC1_C",     "muFast pT resolution wrt Offline neg muons EndCap1 C-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos_EC2_C",     "muFast pT resolution wrt Offline pos muons EndCap2 C-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg_EC2_C",     "muFast pT resolution wrt Offline neg muons EndCap2 C-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_pos_EC3_C",     "muFast pT resolution wrt Offline pos muons EndCap3 C-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    addHistogram( new TH1F("muFast_invptresol_toRecMuonCB_neg_EC3_C",     "muFast pT resolution wrt Offline neg muons EndCap3 C-side; 1/p_{T} resol; Entries",        100, -2., 2.), histdirmufast );
    
	 addHistogram( new TH2F("muFast_ptresol_toRecMuonCB_eta",       "muFast pT resolution wrt Offline in eta; #eta; p_{T} resol", 27, -2.7, 2.7, 100, -2, 2), histdirmufast );
    addHistogram( new TH2F("muFast_ptresol_toRecMuonCB_eta_pT4_6", "muFast pT resolution wrt Offline in eta (pT4GeV-6GeV); #eta; p_{T} resol", 27, -2.7, 2.7, 100, -2, 2), histdirmufast );
    addHistogram( new TH2F("muFast_ptresol_toRecMuonCB_eta_pT6_8", "muFast pT resolution wrt Offline in eta (pT6GeV-8GeV); #eta; p_{T} resol", 27, -2.7, 2.7, 100, -2, 2), histdirmufast );
    addHistogram( new TH2F("muFast_ptresol_toRecMuonCB_eta_pT8_x", "muFast pT resolution wrt Offline in eta (pT over 8GeV); #eta; p_{T} resol", 27, -2.7, 2.7, 100, -2, 2), histdirmufast );
    
	 
	 addHistogram( new TH2F("muFast_ptresol_toRecMuonCB_pt_barrel_A", "muFast barrel A pT resolution wrt Offline in pt; p_{T} [GeV/c]; p_{T} resol", 104, -52., 52., 100, -2, 2), histdirmufast );
    addHistogram( new TH2F("muFast_ptresol_toRecMuonCB_pt_barrel_C", "muFast barrel C pT resolution wrt Offline in pt; p_{T} [GeV/c]; p_{T} resol", 104, -52., 52., 100, -2, 2), histdirmufast );
    addHistogram( new TH2F("muFast_ptresol_toRecMuonCB_pt_endcap_A", "muFast endcap A pT resolution wrt Offline in pt; p_{T} [GeV/c]; p_{T} resol", 104, -52., 52., 100, -2, 2), histdirmufast );
    addHistogram( new TH2F("muFast_ptresol_toRecMuonCB_pt_endcap_C", "muFast endcap C pT resolution wrt Offline in pt; p_{T} [GeV/c]; p_{T} resol", 104, -52., 52., 100, -2, 2), histdirmufast );

    // Efficiency wrt Offline
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt",                "muFast effi pt (GeV/c); p_{T}[GeV/c]; Efficiency",              26, 0, 52), histdireff);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt_numer",          "muFast effi pt (GeV/c) numer; p_{T}[GeV/c]; Entries",        26, 0, 52), histdirmufast);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt_denom",          "muFast effi pt (GeV/c) denom; p_{T}[GeV/c]; Entries",        26, 0, 52), histdirmufast);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt_barrel",         "muFast effi pt (GeV/c) barrel; p_{T}[GeV/c]; Efficiency",       26, 0, 52), histdireff);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt_barrel_numer",   "muFast effi pt (GeV/c) barrel numer; p_{T}[GeV/c]; Entries", 26, 0, 52), histdirmufast);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt_barrel_denom",   "muFast effi pt (GeV/c) barrel denom; p_{T}[GeV/c]; Entries", 26, 0, 52), histdirmufast);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt_endcap",         "muFast effi pt (GeV/c) endcap; p_{T}[GeV/c]; Efficiency",       26, 0, 52), histdireff);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt_endcap_numer",   "muFast effi pt (GeV/c) endcap numer; p_{T}[GeV/c]; Entries", 26, 0, 52), histdirmufast);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_pt_endcap_denom",   "muFast effi pt (GeV/c) endcap denom; p_{T}[GeV/c]; Entries", 26, 0, 52), histdirmufast);

    addHistogram( new TH1F("muFast_effi_toRecMuonCB_eta",           "muFast effi eta; #eta; Efficiency",       32, -3.2, 3.2), histdireff );
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_eta_numer",     "muFast effi eta numer; #eta; Entries", 32, -3.2, 3.2), histdirmufast);
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_eta_denom",     "muFast effi eta denom; #eta; Entries", 32, -3.2, 3.2), histdirmufast);

    addHistogram( new TH1F("muFast_effi_toRecMuonCB_phi",           "muFast effi phi; #phi[rad]; Efficiency",       32, -CLHEP::pi, CLHEP::pi), histdireff );
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_phi_numer",     "muFast effi phi numer; #phi; Entries", 32, -CLHEP::pi, CLHEP::pi), histdirmufast );
    addHistogram( new TH1F("muFast_effi_toRecMuonCB_phi_denom",     "muFast effi phi denom; #phi; Entries", 32, -CLHEP::pi, CLHEP::pi), histdirmufast );


  }else if( newLumiBlock ){
  }

  return StatusCode::SUCCESS;
}

StatusCode HLTMuonMonTool::fillMuFastDQA()
{
  hist("Common_Counter", histdir )->Fill((float)MUFAST);

  const float DR_MATCHED      = 0.5;
  const float ETA_OF_BARREL   = 1.05;
  const float ETA_OF_ENDCAP1   = 1.5;
  const float ETA_OF_ENDCAP2   = 2.0;
  const float ETA_OF_ENDCAP3   = 2.5;
  const float ZERO_LIMIT      = 0.00001;

  const float PT_HISTMAX      = 100.;
  const float PT_OVFL         = 101.5;

  const float EFFI_PT_HISTMAX =  50;
  const float EFFI_PT_OVFL    =  51;

  const float PTRESOL_HISTMAX =  1.9;
  const float PTRESOL_OVFL    =  1.95;

  // -----------------------------
  // Retrieve MuonFeatureContainer
  // -----------------------------

  //const DataHandle<MuonFeatureContainer> mfContainer;  //attention
  //const DataHandle<MuonFeatureContainer> lastmfContainer;  //attention
  //
  const DataHandle<xAOD::L2StandAloneMuonContainer> mfContainer;
  const DataHandle<xAOD::L2StandAloneMuonContainer> lastmfContainer;
  StatusCode sc_mf = m_storeGate->retrieve(mfContainer,lastmfContainer);
  if ( sc_mf.isFailure() ) {
    ATH_MSG_VERBOSE( "Failed to retrieve HLT muFast container" );
    return StatusCode::SUCCESS;    
  }

  ATH_MSG_DEBUG( " ====== START HLTMuon muFast MonTool ====== " );

  // -----------------------------
  // Dump muonFeature info
  // -----------------------------

  std::vector<const xAOD::L2StandAloneMuon*> vec_muonFeatures;

  for(; mfContainer != lastmfContainer; mfContainer++) {
    xAOD::L2StandAloneMuonContainer::const_iterator mf     = mfContainer->begin();
    xAOD::L2StandAloneMuonContainer::const_iterator lastmf = mfContainer->end();
    for(; mf != lastmf; mf++) {
      if( (*mf) == 0 ) continue;
      vec_muonFeatures.push_back( *mf );
    }
  }

  int nMuFast = vec_muonFeatures.size();

  std::vector<const xAOD::L2StandAloneMuon*>::const_iterator itMf;

  //int nmf=0;
  for(itMf=vec_muonFeatures.begin(); itMf != vec_muonFeatures.end(); itMf++) {
    //nmf++;
    int id = (*itMf)->algoId();
    // if( id != 0 ) continue;//muFast_900GeV
    // if( id != 1 ) continue;//muFast_Muon
    float pt     = (*itMf)->pt();
    float eta    = (*itMf)->eta();
    float phi    = (*itMf)->phi();
    int   saddr  = (*itMf)->sAddress();
    int   roiId  = (*itMf)->roiId();
    //ATH_MSG_DEBUG( " nmf  " << nmf );
    ATH_MSG_DEBUG( " id  " << id );
    ATH_MSG_DEBUG( " pt  " << pt );
    ATH_MSG_DEBUG( " eta " << eta );
    ATH_MSG_DEBUG( " phi " << phi );
    ATH_MSG_DEBUG( " RoiID  " << roiId );
    ATH_MSG_DEBUG( " SAddress  " << saddr );

    //std::cout<< "nMF " << nmf << " id " << id << " pt " << pt << " eta " << eta << " phi " << phi << std::endl;

    bool isEndcap = false;
    if( saddr == - 1 )  isEndcap = true;

    bool mf_success = true;
    if( fabs(pt) < ZERO_LIMIT )  mf_success = false;
     
    float pt_hist = pt;
    if( fabs(pt_hist) > PT_HISTMAX ) {
      if( fabs(pt_hist) > 0 ) { pt_hist =  PT_OVFL; }
      else                    { pt_hist = -PT_OVFL; }
    }

    hist("muFast_pt_all", histdirmufast)->Fill(pt_hist);
    if( isEndcap ) { hist("muFast_pt_all_endcap", histdirmufast)->Fill(pt_hist); }
    else           { hist("muFast_pt_all_barrel", histdirmufast)->Fill(pt_hist); }

    if( ! mf_success ) {
      hist("muFast_failed_roiId", histdirmufast)->Fill(roiId+0.01);
      continue;
    }

    // basic histos
    hist("muFast_pt",  histdirmufast)->Fill(pt_hist);
    hist("muFast_eta", histdirmufast)->Fill(eta);
    hist("muFast_phi", histdirmufast)->Fill(phi);
    if( mf_success ){
      hist2("muFast_eta_vs_phi", histdirmufast)->Fill(eta,phi);
      hist2("muFast_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
    }
    if( isEndcap ) {
      hist("muFast_pt_endcap", histdirmufast)->Fill(pt_hist);
      hist("muFast_phi_endcap", histdirmufast)->Fill(phi);
    }
    else {
      hist("muFast_pt_barrel", histdirmufast)->Fill(pt_hist);
      hist("muFast_phi_barrel", histdirmufast)->Fill(phi);
    }
     
    // position, superpoints
    hist("muFast_saddr", histdirmufast )->Fill(saddr+0.01);
    std::vector<float> sp_r;
    std::vector<float> sp_z;
    if( fabs((*itMf)->superPointR(1)) > ZERO_LIMIT ) {
      if( isEndcap ) { sp_r.push_back( (*itMf)->superPointR(1) );    sp_z.push_back( (*itMf)->superPointZ(1) ); }
      else           { sp_r.push_back( (*itMf)->superPointR(1)*10 ); sp_z.push_back( (*itMf)->superPointZ(1)*10 ); }
    }
    if( fabs((*itMf)->superPointR(2)) > ZERO_LIMIT ) {
      if( isEndcap ) { sp_r.push_back( (*itMf)->superPointR(2) );    sp_z.push_back( (*itMf)->superPointZ(2) ); }
      else           { sp_r.push_back( (*itMf)->superPointR(2)*10 ); sp_z.push_back( (*itMf)->superPointZ(2)*10 ); }
    }
    if( fabs((*itMf)->superPointR(3)) > ZERO_LIMIT ) {
      if( isEndcap ) { sp_r.push_back( (*itMf)->superPointR(3) );    sp_z.push_back( (*itMf)->superPointZ(3) ); }
      else           { sp_r.push_back( (*itMf)->superPointR(3)*10 ); sp_z.push_back( (*itMf)->superPointZ(3)*10 ); }
    }
    float sign = 1;
    if( phi < 0 ) sign = -1;
    for(int i=0; i<(int)sp_r.size(); i++) {
      hist2("muFast_MDTpoints_z_vs_r", histdirmufast)->Fill( sp_z[i], sign*sp_r[i] );
    }

  } // loop over vecMuonFeatures

  // ------------------------------------
  // Retrieve MuonFeatureDetailsContainer
  // ------------------------------------


  const DataHandle<MuonFeatureDetailsContainer> mfdContainer;
  const DataHandle<MuonFeatureDetailsContainer> lastmfdContainer;
  StatusCode sc_mfd = m_storeGate->retrieve(mfdContainer,lastmfdContainer);
  if ( sc_mfd.isFailure() ) {
    //ATH_MSG_VERBOSE( "Failed to retrieve HLT muFast details container" );  // attention
    ATH_MSG_INFO( "Failed to retrieve HLT muFast details container" );  // attention
    return StatusCode::SUCCESS;    
  }
  
  if ( !mfdContainer ) {
    ATH_MSG_INFO( "MuonFeatureDetailsContainer not found. Truncated?" );
  }

  ATH_MSG_DEBUG( " ====== START HLTMuon muFast details MonTool ====== " );

  // -----------------------------
  // Dump muonFeatureDetails info
  // -----------------------------

  std::vector<const MuonFeatureDetails*> vec_muonFeatureDetails;

  for(; mfdContainer != lastmfdContainer; mfdContainer++) {
    MuonFeatureDetailsContainer::const_iterator mfd     = mfdContainer->begin();
    MuonFeatureDetailsContainer::const_iterator lastmfd = mfdContainer->end();
    for(; mfd != lastmfd; mfd++) {
      if( (*mfd) == 0 ) continue;
      vec_muonFeatureDetails.push_back( *mfd );
    }
  }

  int nMuFastD = vec_muonFeatureDetails.size();

  std::vector<const MuonFeatureDetails*>::const_iterator itMfd;

  //int nmfd=0;
  //int nmfd10=0;
  const float DR_CUT = 0.4;
  for(itMfd=vec_muonFeatureDetails.begin(); itMfd != vec_muonFeatureDetails.end(); itMfd++) {
    bool off_match = false;
    //nmfd++;
    int id = (*itMfd)->id();
    if( id != 0 ) continue;//muFast_900GeV    
    //if( id != 1 ) continue;//muFast_Muon   
    //if( id < 10 ) nmfd10++; 
    int systemID = (*itMfd)->roi_system();//0:barrel,1:endcap,2:forward
    // (YY commented out for suppressing warning) float pt = (*itMfd)->Pt();
    float eta = (*itMfd)->Eta();
    float phi = (*itMfd)->Phi();

    //std::cout<< "nMFD " << nmfd << " id " << id << " pt " << pt << " eta " << eta << " phi " << phi << std::endl;
    for(int i_rec=0;i_rec<(int)m_RecMuonCB_pt.size();i_rec++){
      // (YY commented out for suppressing warning) float rec_pt = m_RecMuonCB_pt[i_rec];
      float rec_eta = m_RecMuonCB_eta[i_rec];
      float rec_phi = m_RecMuonCB_phi[i_rec];
      float dr = calc_dR(rec_eta,rec_phi,eta,phi);
      // (YY commented out for suppressing warning) float dpt = rec_pt -pt;
      if(dr < DR_CUT){
        off_match = true;
        break;
      }
    }

    // flow
    bool isL1hitThere               = false;
    bool isL1emuOkForTriggerPlane   = false;

    int nRPC = (*itMfd)->pad_hit_onlineId().size();

    int nTGCMidRho = (*itMfd)->tgc_Mid_rho_N();
    int nTGCMidPhi = (*itMfd)->tgc_Mid_phi_N();
    int nTGCInnRho = (*itMfd)->tgc_Inn_rho_N();
    int nTGCInnPhi = (*itMfd)->tgc_Inn_phi_N();
    float TGCMidRhoChi2 = (*itMfd)->tgc_Mid_rho_chi2();
    float TGCMidPhiChi2 = (*itMfd)->tgc_Mid_phi_chi2();

    if( systemID==0 ) { // RPC
      float rpc1_z = (*itMfd)->rpc1_z();
      float rpc2_z = (*itMfd)->rpc2_z();
      const float NO_VALUE = 99999;
      if( nRPC!=0 ) isL1hitThere = true;
      if( fabs(rpc1_z-NO_VALUE) > ZERO_LIMIT && fabs(rpc1_z) > ZERO_LIMIT && fabs(rpc2_z-NO_VALUE) > ZERO_LIMIT && fabs(rpc2_z) > ZERO_LIMIT ) isL1emuOkForTriggerPlane = true;
      ATH_MSG_DEBUG("RPC nHits=" << nRPC);
      ATH_MSG_DEBUG("RPC 1/2 Z=" << rpc1_z << " / " << rpc2_z);
    }
    else { // TGC
      const float NO_VALUE = -99999;
      if( nTGCMidRho!=0 && nTGCMidPhi!=0 ) isL1hitThere = true;
      if( fabs(TGCMidRhoChi2 - NO_VALUE) > ZERO_LIMIT && fabs(TGCMidPhiChi2 - NO_VALUE) > ZERO_LIMIT ) isL1emuOkForTriggerPlane = true;
      ATH_MSG_DEBUG("TGC Mid nHits: Rho/Phi=" << nTGCMidRho << " / " << nTGCMidPhi);
      ATH_MSG_DEBUG("TGC Mid Chi2 : Rho/Phi=" << TGCMidRhoChi2 << " / " << TGCMidPhiChi2);
    }


    // loop each MDT tube
    std::vector<float> mdt_tube_residual = (*itMfd)->mdt_tube_residual();
    std::vector<float> mdt_tube_r        = (*itMfd)->mdt_tube_r();
    std::vector<float> mdt_tube_z        = (*itMfd)->mdt_tube_z();

    int n_mdt_hits_inner  = 0;
    int n_mdt_hits_middle = 0;
    int n_mdt_hits_outer  = 0;

    // if muFast succeeded segment reconstruction.
    //std::cout << "L1_MBTS_2 " << getTDT()->isPassed( "L1_MBTS_2" ) << " L1_MU0 " << getTDT()->isPassed( "L1_MU0" ) << std::endl;;
      if( systemID==0 ){//barrel
        for(int i_tube=0; i_tube<(int)mdt_tube_residual.size(); i_tube++) {
          
          float res = mdt_tube_residual[i_tube];
          float r   = mdt_tube_r[i_tube];
          int imr = 2;
          if     ( r < 650 ) { imr=0; }
          else if( r < 850 ) { imr=1; }

	  /*
	  float z   = mdt_tube_z[i_tube];
          if( abs(res) < 0.001 || res == 1. ){
            std::cout << " MDT barrel resitual imr " << imr << ", r " << r << ", z " << z << ", res " << res << ", ntube " << mdt_tube_residual.size() << ", itube " << i_tube << ", pt " << (*itMfd)->Pt() << std::endl;
            std::cout << " MDT segments chi2  inn " << (*itMfd)->mdt_Inner_fit_chi() << ", mid " << (*itMfd)->mdt_Middle_fit_chi() << ", out " << (*itMfd)->mdt_Outer_fit_chi() << std::endl;
          }
	  */

          if     ( imr == 0 ) { 
            n_mdt_hits_inner++;
            hist("muFast_MDT_Inn_residual_barrel", histdirmufast)->Fill(res);
				if(off_match){
				   hist("muFast_MDT_Inn_residual_barrel_OffMatch", histdirmufast)->Fill(res);
				}
          }
          else if( imr == 1 ) {
            n_mdt_hits_middle++;
            hist("muFast_MDT_Mid_residual_barrel", histdirmufast)->Fill(res);
				if(off_match){
				   hist("muFast_MDT_Mid_residual_barrel_OffMatch", histdirmufast)->Fill(res);
				}
          }
          else if( imr == 2 ) {
            n_mdt_hits_outer++;
            hist("muFast_MDT_Out_residual_barrel", histdirmufast)->Fill(res);
				if(off_match){
				   hist("muFast_MDT_Out_residual_barrel_OffMatch", histdirmufast)->Fill(res);
				}
          }
        }
      } 
      else{ //endcap
        for(int i_tube=0; i_tube<(int)mdt_tube_residual.size(); i_tube++) {

          float res = mdt_tube_residual[i_tube] / 10 ; // to cm
          float z   = mdt_tube_z[i_tube];
          int imr = 2;
          if     ( fabs(z) < 10000 ) { imr=0; }
          else if( fabs(z) < 15000 ) { imr=1; }

          /*
          if( res == 0. || res == 1. ){
            std::cout << " MDT endcap resitual imr " << imr << " z " << z << " res " << res << " ntube " << mdt_tube_residual.size() << " itube " << i_tube << " pt " << (*itMfd)->Pt() << std::endl;
            std::cout << " MDT segments chi2  inn " << (*itMfd)->mdt_Inner_fit_chi() << " mid " << (*itMfd)->mdt_Middle_fit_chi() << " out " << (*itMfd)->mdt_Outer_fit_chi() << std::endl;
          }
          */

          if     ( imr == 0 ) {
            n_mdt_hits_inner++;
            hist("muFast_MDT_Inn_residual_endcap", histdirmufast)->Fill(res);
				if(off_match){
				   hist("muFast_MDT_Inn_residual_endcap_OffMatch", histdirmufast)->Fill(res);
				}
          }
          else if( imr == 1 ) {
            n_mdt_hits_middle++;
            hist("muFast_MDT_Mid_residual_endcap", histdirmufast)->Fill(res);
				if(off_match){
				   hist("muFast_MDT_Mid_residual_endcap_OffMatch", histdirmufast)->Fill(res);
				}
          }
          else if( imr == 2 ) {
            n_mdt_hits_outer++;
            hist("muFast_MDT_Out_residual_endcap", histdirmufast)->Fill(res);
				if(off_match){
				   hist("muFast_MDT_Out_residual_endcap_OffMatch", histdirmufast)->Fill(res);
				}
          }
        }
      }

    //MDT flow
    bool isMDThitThereForTriggerPlane = false;
    bool isMDTFitOkForTriggerPlane    = false;
    bool isMDTFitOkFor2Plane          = false;

    int nMDT = (*itMfd)->mdt_onlineId().size();

    float MDTInnChi2 = (*itMfd)->mdt_Inner_fit_chi();
    float MDTMidChi2 = (*itMfd)->mdt_Middle_fit_chi();
    float MDTOutChi2 = (*itMfd)->mdt_Outer_fit_chi();

    const float MDT_CHI2_NO_VALUE = -99999;

    if( n_mdt_hits_middle != 0 ) isMDThitThereForTriggerPlane = true;
    if( fabs(MDTMidChi2-MDT_CHI2_NO_VALUE) > ZERO_LIMIT ) isMDTFitOkForTriggerPlane = true;
    if( isMDTFitOkForTriggerPlane && (fabs(MDTInnChi2 - MDT_CHI2_NO_VALUE) > ZERO_LIMIT || fabs(MDTOutChi2 - MDT_CHI2_NO_VALUE) > ZERO_LIMIT) ) isMDTFitOkFor2Plane  = true;

    ATH_MSG_DEBUG("isL1hitThere/emuOkForTriggerPlane=" << isL1hitThere << " / " << isL1emuOkForTriggerPlane);

    ATH_MSG_DEBUG("isMDThitThere/FitOkForTriggerPlane/FitOkFor2Plane=" << isMDThitThereForTriggerPlane << " / "
                  << isMDTFitOkForTriggerPlane << " / " << isMDTFitOkFor2Plane);

    hist("muFast_proc_flow",histdirmufast)->Fill(1+0.01);
    if( isL1hitThere )                 hist("muFast_proc_flow",histdirmufast)->Fill(2+0.01);
    if( isL1emuOkForTriggerPlane )     hist("muFast_proc_flow",histdirmufast)->Fill(3+0.01);
    if( isMDThitThereForTriggerPlane ) hist("muFast_proc_flow",histdirmufast)->Fill(4+0.01);
    if( isMDTFitOkForTriggerPlane )    hist("muFast_proc_flow",histdirmufast)->Fill(5+0.01);
    if( isMDTFitOkFor2Plane )          hist("muFast_proc_flow",histdirmufast)->Fill(6+0.01);

    // fill

    if( systemID==0 ){//barrel
      hist("muFast_RPC_Pad_N", histdirmufast)->Fill(nRPC);

      hist("muFast_MDT_Inn_fit_chi2_barrel", histdirmufast)->Fill(MDTInnChi2);
      hist("muFast_MDT_Mid_fit_chi2_barrel", histdirmufast)->Fill(MDTMidChi2);
      hist("muFast_MDT_Out_fit_chi2_barrel", histdirmufast)->Fill(MDTOutChi2);

      hist("muFast_MDT_N_barrel", histdirmufast)->Fill(nMDT+0.01);

      hist("muFast_MDT_Inn_N_barrel", histdirmufast)->Fill(n_mdt_hits_inner+0.01);
      hist("muFast_MDT_Mid_N_barrel", histdirmufast)->Fill(n_mdt_hits_middle+0.01);
      hist("muFast_MDT_Out_N_barrel", histdirmufast)->Fill(n_mdt_hits_outer+0.01);

    }else{//endcap
      hist("muFast_TGC_Mid_rho_N", histdirmufast)->Fill(nTGCMidRho+0.01);
      hist("muFast_TGC_Mid_phi_N", histdirmufast)->Fill(nTGCMidPhi+0.01);
      hist("muFast_TGC_Inn_rho_N", histdirmufast)->Fill(nTGCInnRho+0.01);
      hist("muFast_TGC_Inn_phi_N", histdirmufast)->Fill(nTGCInnPhi+0.01);

      hist("muFast_TGC_Mid_rho_chi2", histdirmufast)->Fill(TGCMidRhoChi2);
      hist("muFast_TGC_Mid_phi_chi2", histdirmufast)->Fill(TGCMidPhiChi2);

      hist("muFast_MDT_Inn_fit_chi2_endcap", histdirmufast)->Fill(MDTInnChi2);
      hist("muFast_MDT_Mid_fit_chi2_endcap", histdirmufast)->Fill(MDTMidChi2);
      hist("muFast_MDT_Out_fit_chi2_endcap", histdirmufast)->Fill(MDTOutChi2);

      hist("muFast_MDT_N_endcap", histdirmufast)->Fill(nMDT);
      hist("muFast_MDT_Inn_N_endcap", histdirmufast)->Fill(n_mdt_hits_inner+0.01);
      hist("muFast_MDT_Mid_N_endcap", histdirmufast)->Fill(n_mdt_hits_middle+0.01);
      hist("muFast_MDT_Out_N_endcap", histdirmufast)->Fill(n_mdt_hits_outer+0.01);
    }
    
  } // end of loop MuonFetureDetails

  //std:: cout << " nMF " << nmf << " nMFD "<< nmfd << " nMFD10 " << nmfd10 << " matching nMF==nMFD10 " << (nmf==nmfd10) << std::endl;

  
  // ---------------------------------
  // Get mu6 and mu10 chains using TDT
  // ---------------------------------

  Trig::FeatureContainer fc_mu6 = getTDT()->features("HLT_mu6");
  std::vector<Trig::Combination> combs_mu6 = fc_mu6.getCombinations();
  std::vector<Trig::Combination>::const_iterator p_comb_mu6;

  ATH_MSG_DEBUG("isPassed mu6="  << getTDT()->isPassed("HLT_mu6",  TrigDefs::Physics));

  // -----------------------------
  // Comparison wrt Offline
  // -----------------------------

  for(int i_offl=0;i_offl<(int)m_RecMuonCB_pt.size();i_offl++) {

    float pt_offl  = m_RecMuonCB_pt[i_offl];
    float eta_offl = m_RecMuonCB_eta[i_offl];
    float phi_offl = m_RecMuonCB_phi[i_offl];
    float charge_offl = m_RecMuonCB_charge[i_offl];

    float pt_offl_hist = fabs(pt_offl);
    if( pt_offl_hist > EFFI_PT_HISTMAX )  pt_offl_hist = EFFI_PT_OVFL;

    float signed_pt_offl_hist = pt_offl * charge_offl;
    if( signed_pt_offl_hist > EFFI_PT_HISTMAX )  signed_pt_offl_hist = EFFI_PT_OVFL;
    else if( signed_pt_offl_hist < -EFFI_PT_HISTMAX )  signed_pt_offl_hist = -EFFI_PT_OVFL;
    
    hist("muFast_effi_toRecMuonCB_pt_denom",  histdirmufast)->Fill(pt_offl_hist);
    hist("muFast_effi_toRecMuonCB_eta_denom", histdirmufast)->Fill(eta_offl);
    hist("muFast_effi_toRecMuonCB_phi_denom", histdirmufast)->Fill(phi_offl);

    if( fabs(eta_offl) < ETA_OF_BARREL ) {
      hist("muFast_effi_toRecMuonCB_pt_barrel_denom", histdirmufast)->Fill(pt_offl_hist);
    }
    else {
      hist("muFast_effi_toRecMuonCB_pt_endcap_denom", histdirmufast)->Fill(pt_offl_hist);
    }

    // check whether matching muonFeature is there
    float dRmin  = 1000.;
    float pt_mf  = 0.; 

    for(itMf=vec_muonFeatures.begin(); itMf != vec_muonFeatures.end(); itMf++) {
       float pt   = (*itMf)->pt();
       if( fabs(pt) < ZERO_LIMIT )  continue;
       float eta  = (*itMf)->eta();
       float phi  = (*itMf)->phi();
       float dR = calc_dR(eta, phi, eta_offl, phi_offl);
       if( dR < dRmin ) {
	  dRmin = dR;
	  pt_mf = (*itMf)->pt();
       }
    }

 
    hist("muFast_dR_toRecMuonCB", histdirmufast)->Fill(dRmin);
    if( dRmin > DR_MATCHED ) continue; // not matched to muFast

    // efficiencies, resolutions
    float ptresol = 0;
    float ptresol_hist = 0;
    float invptresol_signed = 0;
    if (pt_offl != 0 && pt_mf != 0) {
      ptresol = fabs(pt_mf)/fabs(pt_offl) - 1;
      ptresol_hist = ptresol;
      invptresol_signed = (1./(pt_offl * charge_offl) - 1./pt_mf) / (1./(pt_offl * charge_offl));
    }
      
    if( fabs(ptresol) > PTRESOL_HISTMAX ) ptresol_hist = ptresol_hist / fabs(ptresol_hist) * PTRESOL_OVFL;
    hist("muFast_ptresol_toRecMuonCB", histdirmufast)->Fill(ptresol_hist);
    if (charge_offl > 0) {
      hist("muFast_invptresol_toRecMuonCB_pos", histdirmufast)->Fill(invptresol_signed);
    }
    if (charge_offl < 0) {
      hist("muFast_invptresol_toRecMuonCB_neg", histdirmufast)->Fill(invptresol_signed);
    }
    hist2("muFast_ptresol_toRecMuonCB_eta", histdirmufast)->Fill(eta_offl, ptresol_hist);
    hist("muFast_effi_toRecMuonCB_pt_numer", histdirmufast)->Fill(pt_offl_hist);
    hist("muFast_effi_toRecMuonCB_eta_numer", histdirmufast)->Fill(eta_offl);
    hist("muFast_effi_toRecMuonCB_phi_numer", histdirmufast)->Fill(phi_offl);
    if( fabs(eta_offl) < ETA_OF_BARREL ) {
      hist("muFast_effi_toRecMuonCB_pt_barrel_numer", histdirmufast)->Fill(pt_offl_hist);
      if(eta_offl>0) hist2("muFast_ptresol_toRecMuonCB_pt_barrel_A", histdirmufast)->Fill(signed_pt_offl_hist, ptresol_hist);
      else           hist2("muFast_ptresol_toRecMuonCB_pt_barrel_C", histdirmufast)->Fill(signed_pt_offl_hist, ptresol_hist);
    }
    else {
      hist("muFast_effi_toRecMuonCB_pt_endcap_numer", histdirmufast)->Fill(pt_offl_hist);
      if(eta_offl>0) hist2("muFast_ptresol_toRecMuonCB_pt_endcap_A", histdirmufast)->Fill(signed_pt_offl_hist, ptresol_hist);
      else           hist2("muFast_ptresol_toRecMuonCB_pt_endcap_C", histdirmufast)->Fill(signed_pt_offl_hist, ptresol_hist);
    }

    if (eta_offl >= 0) {
      if( fabs(eta_offl) < ETA_OF_BARREL ) {
	hist("muFast_ptresol_toRecMuonCB_BR", histdirmufast)->Fill(ptresol_hist);
	if (charge_offl > 0) {
	  hist("muFast_invptresol_toRecMuonCB_pos_BR_A", histdirmufast)->Fill(invptresol_signed);
	}
	if (charge_offl < 0) {
	  hist("muFast_invptresol_toRecMuonCB_neg_BR_A", histdirmufast)->Fill(invptresol_signed);
	}
      }
      else if( fabs(eta_offl) < ETA_OF_ENDCAP1){
	hist("muFast_ptresol_toRecMuonCB_EC1", histdirmufast)->Fill(ptresol_hist);
	if (charge_offl > 0) {
	  hist("muFast_invptresol_toRecMuonCB_pos_EC1_A", histdirmufast)->Fill(invptresol_signed);
	}
	if (charge_offl < 0) {
	  hist("muFast_invptresol_toRecMuonCB_neg_EC1_A", histdirmufast)->Fill(invptresol_signed);
	}
      }
      else if( fabs(eta_offl) < ETA_OF_ENDCAP2){
	hist("muFast_ptresol_toRecMuonCB_EC2", histdirmufast)->Fill(ptresol_hist);
	if (charge_offl > 0) {
	  hist("muFast_invptresol_toRecMuonCB_pos_EC2_A", histdirmufast)->Fill(invptresol_signed);
	}
	if (charge_offl < 0) {
	  hist("muFast_invptresol_toRecMuonCB_neg_EC2_A", histdirmufast)->Fill(invptresol_signed);
	}
      }
      else if( fabs(eta_offl) < ETA_OF_ENDCAP3){
	hist("muFast_ptresol_toRecMuonCB_EC3", histdirmufast)->Fill(ptresol_hist);
	if (charge_offl > 0) {
	  hist("muFast_invptresol_toRecMuonCB_pos_EC3_A", histdirmufast)->Fill(invptresol_signed);
	}
	if (charge_offl < 0) {
	  hist("muFast_invptresol_toRecMuonCB_neg_EC3_A", histdirmufast)->Fill(invptresol_signed);
	}
      }
    } else {
      if( fabs(eta_offl) < ETA_OF_BARREL ) {
	hist("muFast_ptresol_toRecMuonCB_BR", histdirmufast)->Fill(ptresol_hist);
	if (charge_offl > 0) {
	  hist("muFast_invptresol_toRecMuonCB_pos_BR_C", histdirmufast)->Fill(invptresol_signed);
	}
	if (charge_offl < 0) {
	  hist("muFast_invptresol_toRecMuonCB_neg_BR_C", histdirmufast)->Fill(invptresol_signed);
	}
      }
      else if( fabs(eta_offl) < ETA_OF_ENDCAP1){
	hist("muFast_ptresol_toRecMuonCB_EC1", histdirmufast)->Fill(ptresol_hist);
	if (charge_offl > 0) {
	  hist("muFast_invptresol_toRecMuonCB_pos_EC1_C", histdirmufast)->Fill(invptresol_signed);
	}
	if (charge_offl < 0) {
	  hist("muFast_invptresol_toRecMuonCB_neg_EC1_C", histdirmufast)->Fill(invptresol_signed);
	}
      }
      else if( fabs(eta_offl) < ETA_OF_ENDCAP2){
	hist("muFast_ptresol_toRecMuonCB_EC2", histdirmufast)->Fill(ptresol_hist);
	if (charge_offl > 0) {
	  hist("muFast_invptresol_toRecMuonCB_pos_EC2_C", histdirmufast)->Fill(invptresol_signed);
	}
	if (charge_offl < 0) {
	  hist("muFast_invptresol_toRecMuonCB_neg_EC2_C", histdirmufast)->Fill(invptresol_signed);
	}
      }
      else if( fabs(eta_offl) < ETA_OF_ENDCAP3){
	hist("muFast_ptresol_toRecMuonCB_EC3", histdirmufast)->Fill(ptresol_hist);
	if (charge_offl > 0) {
	  hist("muFast_invptresol_toRecMuonCB_pos_EC3_C", histdirmufast)->Fill(invptresol_signed);
	}
	if (charge_offl < 0) {
	  hist("muFast_invptresol_toRecMuonCB_neg_EC3_C", histdirmufast)->Fill(invptresol_signed);
	}
      }
    }
    
    if( fabs(pt_offl) > 4 ) {
      if( fabs(pt_offl) < 6 ){
	hist2("muFast_ptresol_toRecMuonCB_eta_pT4_6", histdirmufast)->Fill(eta_offl, ptresol_hist);
      }
      else if( fabs(pt_offl) < 8 ){
	hist2("muFast_ptresol_toRecMuonCB_eta_pT6_8", histdirmufast)->Fill(eta_offl, ptresol_hist);
      }
      else{
	hist2("muFast_ptresol_toRecMuonCB_eta_pT8_x", histdirmufast)->Fill(eta_offl, ptresol_hist);
      }
    }
    
    //Check TrigRoiDescriptor(for mu6 chains)
    float dRmin_mu6 = 1000;
    float forIDroi_eta_mu6 = 0;
    float forIDroi_phi_mu6 = 0;
	 for(p_comb_mu6=combs_mu6.begin();p_comb_mu6!=combs_mu6.end();++p_comb_mu6) {
       std::vector<Trig::Feature<xAOD::L2StandAloneMuonContainer> > fs_MF = 
	      (*p_comb_mu6).get<xAOD::L2StandAloneMuonContainer>("MuonL2SAInfo",TrigDefs::alsoDeactivateTEs);	
       if(!fs_MF.size()) continue;
       const xAOD::L2StandAloneMuonContainer* mf_cont = fs_MF[0];
       float eta_mf = mf_cont->at(0)->eta();
       float phi_mf = mf_cont->at(0)->phi();
       float dR = calc_dR(eta_mf, phi_mf, eta_offl, phi_offl);
       if( dR < dRmin_mu6 ) {
	      dRmin_mu6 = dR;
	      std::vector<Trig::Feature<TrigRoiDescriptor> > fs_roi = 
	        p_comb_mu6->get<TrigRoiDescriptor>("forID",TrigDefs::alsoDeactivateTEs);	
       if(!fs_roi.size()) continue;
	      forIDroi_eta_mu6 = fs_roi.at(0).cptr()->eta();
	      forIDroi_phi_mu6 = fs_roi.at(0).cptr()->phi();
       }
    }
    if( dRmin_mu6 < DR_MATCHED ) {
       float dr   = calc_dR(eta_offl,phi_offl,forIDroi_eta_mu6,forIDroi_phi_mu6);
       float deta = forIDroi_eta_mu6-eta_offl;
       float dphi = calc_dphi(phi_offl,forIDroi_phi_mu6);
       ATH_MSG_DEBUG("mu6(MF): dR matched=" << dRmin_mu6);
       ATH_MSG_DEBUG("       : forID RoI eta/phi=" << forIDroi_eta_mu6 << " / " << forIDroi_phi_mu6);
       ATH_MSG_DEBUG("       : forID RoI dR/deta/dphi wrt Offl CB=" << dr << " / " << deta << " / " << dphi);
       hist("muFast_forID_dR_toRecMuonCB_mu6", histdirmufast)->Fill(dr);
       hist2("muFast_forID_deta_vs_eta_toRecMuonCB_mu6", histdirmufast)->Fill(eta_offl,deta);
       hist2("muFast_forID_dphi_vs_eta_toRecMuonCB_mu6", histdirmufast)->Fill(eta_offl,dphi);
    }

  } // end of offline muon loop
   
  // -----------------------------
  // end
  // -----------------------------

  if(nMuFast)hist("Common_Counter", histdir)->Fill((float)MUFASTFOUND);
  if(nMuFastD)hist("Common_Counter", histdir)->Fill((float)MUFASTDFOUND);

  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::procMuFastDQA()
{
  if( endOfRun ){

    // efficiency histograms
    hist("muFast_effi_toRecMuonCB_pt", histdireff)->Sumw2();
    hist("muFast_effi_toRecMuonCB_pt", histdireff)->Divide( hist("muFast_effi_toRecMuonCB_pt_numer", histdirmufast), hist("muFast_effi_toRecMuonCB_pt_denom", histdirmufast), 1, 1, "B" );

    hist("muFast_effi_toRecMuonCB_pt_barrel", histdireff)->Sumw2();
    hist("muFast_effi_toRecMuonCB_pt_barrel", histdireff)->Divide( hist("muFast_effi_toRecMuonCB_pt_barrel_numer", histdirmufast), hist("muFast_effi_toRecMuonCB_pt_barrel_denom", histdirmufast), 1, 1, "B" );

    hist("muFast_effi_toRecMuonCB_pt_endcap", histdireff)->Sumw2();
    hist("muFast_effi_toRecMuonCB_pt_endcap", histdireff)->Divide( hist("muFast_effi_toRecMuonCB_pt_endcap_numer", histdirmufast), hist("muFast_effi_toRecMuonCB_pt_endcap_denom", histdirmufast), 1, 1, "B" );

    hist("muFast_effi_toRecMuonCB_eta", histdireff)->Sumw2();
    hist("muFast_effi_toRecMuonCB_eta", histdireff)->Divide( hist("muFast_effi_toRecMuonCB_eta_numer", histdirmufast), hist("muFast_effi_toRecMuonCB_eta_denom", histdirmufast), 1, 1, "B" );

    hist("muFast_effi_toRecMuonCB_phi", histdireff)->Sumw2();
    hist("muFast_effi_toRecMuonCB_phi", histdireff)->Divide( hist("muFast_effi_toRecMuonCB_phi_numer", histdirmufast), hist("muFast_effi_toRecMuonCB_phi_denom", histdirmufast), 1, 1, "B" );

  }else if( endOfLumiBlock ){
  }
  return StatusCode::SUCCESS;
}
