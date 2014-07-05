/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile HLTMuonMonTool.cxx
 *   
 *    authors: Andrea Ventura (ventura@le.infn.it)
 *             Margherita Primavera (margherita.primavera@le.infn.it)                       
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"

#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

//#include "muonEvent/Muon.h"
//#include "muonEvent/MuonContainer.h"
#include "muonEvent/MuonParamDefs.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"

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

#include "TrigMuonMonitoring/HLTMuonMonTool.h"

//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

using namespace std;

StatusCode HLTMuonMonTool::initMuonEFDQA()
{
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::bookMuonEFDQA()
{
  //histograms in each 10LBs 
  if( newRun || newLowStat ){

    addHistogram( new TH2F("EFMS_eta_vs_phi_in_10LBs",           "TrigMuonEF TrackBuilder eta vs phi in 10LBs; #eta ; #phi",           27, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi), histdircoverage );
    addHistogram( new TH2F("EFSA_eta_vs_phi_in_10LBs",           "TrigMuonEF Extrapolator eta vs phi in 10LBs; #eta ; #phi",           27, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi), histdircoverage );
    addHistogram( new TH2F("EFCB_eta_vs_phi_in_10LBs",           "TrigMuonEF Combiner eta vs phi in 10LBs; #eta ; #phi",               27, -2.7, 2.7, 16, -CLHEP::pi, CLHEP::pi), histdircoverage );

  }

  if( newRun ){

    addHistogram( new TH1F("EFMS_pt",    "TrigMuonEF TrackBuilder pT; p_{T}[GeV/c]; Entries",    105, 0.,105.), histdirmuonef );
    addHistogram( new TH1F("EFMS_signed_pt",    "TrigMuonEF TrackBuilder signed pT; signed p_{T}[GeV/c]; Entries",    210, -105.,105.), histdirmuonef );
    addHistogram( new TH1F("EFMS_eta",   "TrigMuonEF TrackBuilder eta; #eta; Entries",   108,-2.7,2.7), histdirmuonef );
    addHistogram( new TH1F("EFMS_phi",   "TrigMuonEF TrackBuilder phi; #phi[rad]; Entries",   96,-CLHEP::pi, CLHEP::pi), histdirmuonef );
    addHistogram( new TH2F("EFMS_eta_vs_phi",           "TrigMuonEF TrackBuilder eta vs phi; #eta ; #phi",           108, -2.7, 2.7, 96, -CLHEP::pi, CLHEP::pi), histdirmuonef );

    addHistogram( new TH1F("EFSA_pt",    "TrigMuonEF Extrapolator pT; p_{T}[GeV/c]; Entries",    105,0.,105.), histdirmuonef );
    addHistogram( new TH1F("EFSA_signed_pt",    "TrigMuonEF Extrapolator signed pT; signed p_{T}[GeV/c]; Entries",    210,-105.,105.), histdirmuonef );
    addHistogram( new TH1F("EFSA_eta",   "TrigMuonEF Extrapolator eta; #eta; Entries",   108,-2.7,2.7), histdirmuonef );
    addHistogram( new TH1F("EFSA_phi",   "TrigMuonEF Extrapolator phi; #phi[rad]; Entries",   100,-3.15,3.15), histdirmuonef );
    addHistogram( new TH2F("EFSA_eta_vs_phi",           "TrigMuonEF Extrapolator eta vs phi; #eta ; #phi",           108, -2.7, 2.7, 96, -CLHEP::pi, CLHEP::pi), histdirmuonef );

    addHistogram( new TH1F("EFCB_pt",    "TrigMuonEF Combiner pT; p_{T}; Entries",    105,0.,105.), histdirmuonef );
    addHistogram( new TH1F("EFCB_signed_pt",    "TrigMuonEF Combiner signed pT; signed p_{T}; Entries",    210, -105.,105.), histdirmuonef );
    addHistogram( new TH1F("EFCB_eta",   "TrigMuonEF Combiner eta; #eta; Entries",   108,-2.7,2.7), histdirmuonef );
    addHistogram( new TH1F("EFCB_phi",   "TrigMuonEF Combiner phi; #phi[rad]; Entries",   96,-CLHEP::pi, CLHEP::pi), histdirmuonef );
    addHistogram( new TH2F("EFCB_eta_vs_phi",           "TrigMuonEF Combiner eta vs phi; #eta ; #phi",               108, -2.7, 2.7, 96, -CLHEP::pi, CLHEP::pi), histdirmuonef );

    addHistogram( new TH1F("RecMS_pt",    "Offline Muon Spectrometer pT; p_{T}[GeV/c]; Entries",    105, 0.,105.), histdirmuonef );
    addHistogram( new TH1F("RecMS_eta",   "Offline Muon Spectrometer eta; #eta; Entries",   108,-2.7,2.7), histdirmuonef );
    addHistogram( new TH1F("RecMS_phi",   "Offline Muon Spectrometer phi; #phi[rad]; Entries",   96,-CLHEP::pi, CLHEP::pi), histdirmuonef );

    addHistogram( new TH1F("RecSA_pt",    "Offline Muon Extrapolator pT; p_{T}[GeV/c]; Entries",    105, 0.,105.), histdirmuonef );
    addHistogram( new TH1F("RecSA_eta",   "Offline Muon Extrapolator eta; #eta; Entries",   108,-2.7,2.7), histdirmuonef );
    addHistogram( new TH1F("RecSA_phi",   "Offline Muon Extrapolator phi; #phi[rad]; Entries",   96,-CLHEP::pi, CLHEP::pi), histdirmuonef );

    addHistogram( new TH1F("RecCB_pt",    "Offline Muon Combiner pT; p_{T}[GeV/c]; Entries",    105,0.,105.), histdirmuonef );
    addHistogram( new TH1F("RecCB_eta",   "Offline Muon Combiner eta; #eta; Entries",   108,-2.7,2.7), histdirmuonef );
    addHistogram( new TH1F("RecCB_phi",   "Offline Muon Combiner phi; #phi[rad]; Entries",   96,-CLHEP::pi, CLHEP::pi), histdirmuonef );


    /*
      addHistogram( new TH1F("RecLPt_pt",    "Offline Muon LowPt pT",    100,0.,100.) );
      addHistogram( new TH1F("RecLPt_eta",   "Offline Muon LowPt eta",   100,-3.2,3.2) );
      addHistogram( new TH1F("RecLPt_phi",   "Offline Muon LowPt phi",   100,-3.15,3.15) );
    */

    // correlation histograms -- EFMS vs RecMSmuon
    addHistogram(new TH2F("EF_MSMuon_RecMSMuon_PtCor", "PtCor_EFMS_Recmuon ; RecMSmuon_Pt [GeV/c]; EFMSmuon_Pt [GeV/c]", 100, 0.,100., 100, 0.,100.), histdirmuonef);
    addHistogram(new TH2F("EF_MSMuon_RecMSMuon_EtaCor", "EtaCor_EFMS_Recmuon ; RecMSmuon_Eta ; EFMSmuon_Eta", 100,-3.2,3.2, 100,-3.2,3.2), histdirmuonef);
    addHistogram(new TH2F("EF_MSMuon_RecMSMuon_PhiCor", "PhiCor_EFMS_Recmuon ; RecMSmuon_Phi [rad]; EFMSmuon_Phi [rad]", 100,-3.15,3.15, 100,-3.15,3.15), histdirmuonef);
    addHistogram(new TH1F("EF_MSMuon_RecMSMuon_dPt", "dPt_EFMS_Recmuon ; dPt [GeV/c]", 100,-10.,10.), histdirmuonef);
    addHistogram(new TH1F("EF_MSMuon_RecMSMuon_dEta", "dEta_EFMS_Recmuon ; dEta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("EF_MSMuon_RecMSMuon_dPhi", "dPhi_EFMS_Recmuon ; dPhi [rad]", 100,-3.15,3.15), histdirmuonef);
    addHistogram(new TH1F("EF_MSMuon_RecMSMuon_dR", "dR_EFMS_Recmuon ; dR", 110,0.,11.), histdirmuonef);

    // MS  aware  histo
    addHistogram(new TH1F("RecAwareMSmuon_pt", "RecMSmuon_pt ; p_{T}[GeV/c]", 100,0.,100.), histdirmuonef);
    addHistogram(new TH1F("RecAwareMSmuon_eta", "RecMSmuon_eta ; #eta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("RecAwareMSmuon_phi", "RecMSmuon_phi ; #phi[rad]", 100,-3.15,3.15), histdirmuonef);

    // correlation histograms -offline/online EFSA vs RecSAmuon
    addHistogram(new TH2F("EF_SAMuon_RecSAMuon_PtCor", "PtCor_EFSA_Recmuon ; RecSAmuon_Pt [GeV/c]; EFSAmuon_Pt [GeV/c]", 100, 0.,100., 100, 0.,100.), histdirmuonef);
    addHistogram(new TH2F("EF_SAMuon_RecSAMuon_EtaCor", "EtaCor_EFSA_Recmuon ; RecSAmuon_Eta ; EFSAmuon_Eta", 100,-3.2,3.2, 100,-3.2,3.2), histdirmuonef);
    addHistogram(new TH2F("EF_SAMuon_RecSAMuon_PhiCor", "PhiCor_EFSA_Recmuon ; RecSAmuon_Phi [rad]; EFSAmuon_Phi [rad]", 100,-3.15,3.15, 100,-3.15,3.15), histdirmuonef);
    addHistogram(new TH1F("EF_SAMuon_RecSAMuon_dPt", "dPt_EFSA_Recmuon ; dPt [GeV/c]", 100,-0.5,0.5), histdirmuonef);
    addHistogram(new TH1F("EF_SAMuon_RecSAMuon_dEta", "dEta_EFSA_Recmuon ; dEta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("EF_SAMuon_RecSAMuon_dPhi", "dPhi_EFSA_Recmuon ; dPhi [rad]", 100,-3.15,3.15), histdirmuonef);
    addHistogram(new TH1F("EF_SAMuon_RecSAMuon_dR", "dR_EFSA_Recmuon ; dR", 110,0.,11.), histdirmuonef);

    // Stand Alone Trigger aware histograms
    addHistogram(new TH1F("RecAwareSAmuon_pt", "RecSAmuon_pt ; p_{T}[GeV/c]", 100,0.,0.5), histdirmuonef);
    addHistogram(new TH1F("RecAwareSAmuon_eta", "RecSAmuon_eta ; #eta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("RecAwareSAmuon_phi", "RecSAmuon_phi ; #phi[rad]", 100,-3.15,3.15), histdirmuonef);

    //  correlation histograms -offline/online EFCB vs RecCBmuon
    addHistogram(new TH2F("EF_CBMuon_RecCBMuon_PtCor", "PtCor_EFCB_Recmuon ; RecCBmuon_Pt [GeV/c]; EFCBmuon_Pt [GeV/c]", 100, 0.,100.,100, 0.,100.), histdirmuonef);
    addHistogram(new TH2F("EF_CBMuon_RecCBMuon_EtaCor", "EtaCor_EFCB_Recmuon ; RecCBmuon_Eta ; EFCBmuon_Eta", 100,-3.2,3.2,100,-3.2,3.2), histdirmuonef);
    addHistogram(new TH2F("EF_CBMuon_RecCBMuon_PhiCor", "PhiCor_EFCB_Recmuon ; RecCBmuon_Phi [rad]; EFCBmuon_Phi [rad]", 100,-3.15,3.15, 100,-3.15,3.15), histdirmuonef);
    addHistogram(new TH1F("EF_CBMuon_RecCBMuon_dPt", "dPt_EFCB_Recmuon ; dPt [GeV/c]", 100,-0.5,0.5), histdirmuonef);
    addHistogram(new TH1F("EF_CBMuon_RecCBMuon_dEta", "dEta_EFCB_Recmuon ; dEta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("EF_CBMuon_RecCBMuon_dPhi", "dPhi_EFCB_Recmuon ; dPhi [rad]", 100,-3.15,3.15), histdirmuonef);
    addHistogram(new TH1F("EF_CBMuon_RecCBMuon_dR", "dR_EFCB_Recmuon ; dR", 110,0.,11.), histdirmuonef);

    // Combined Trigger aware histograms
    addHistogram(new TH1F("RecAwareCBmuon_pt", "RecCBmuon_pt ; p_{T}[GeV/c]", 100,0.,100.), histdirmuonef);
    addHistogram(new TH1F("RecAwareCBmuon_eta", "RecCBmuon_eta ; #eta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("RecAwareCBmuon_phi", "RecCBmuon_phi ; #phi[rad]", 100,-3.15,3.15), histdirmuonef);

    // correlation histograms -- EFMS vs Reco muons which passed LVL2 MuFast
    addHistogram(new TH2F("EF_SAwrtRecL2Aware_PtCor", "PtCor_SAwrtRecMuFastAware ; RecSAmuon_Pt [GeV/c]; EFSAmuon_Pt [GeV/c]", 100, 0.,100., 100, 0.,100.), histdirmuonef);
    addHistogram(new TH2F("EF_SAwrtRecL2Aware_EtaCor", "EtaCor_SAwrtRecMuFastAware ; RecSAmuon_Eta ; EFSAmuon_Eta", 100,-3.2,3.2, 100,-3.2,3.2), histdirmuonef);
    addHistogram(new TH2F("EF_SAwrtRecL2Aware_PhiCor", "PhiCor_SAwrtRecMuFastAware ; RecSAmuon_Phi [rad]; EFSAmuon_Phi [rad]", 100,-3.15,3.15, 100,-3.15,3.15), histdirmuonef);
    addHistogram(new TH1F("EF_SAwrtRecL2Aware_dPt", "dPt_SAwrtRecMuFastAware ; dPt [GeV/c]", 100,-0.5,0.5), histdirmuonef);
    addHistogram(new TH1F("EF_SAwrtRecL2Aware_dEta", "dEta_SAwrtRecMuFastAware ; dEta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("EF_SAwrtRecL2Aware_dPhi", "dPhi_SAwrtRecMuFastAware ; dPhi [rad]", 100,-3.15,3.15), histdirmuonef);

    // Muon EF SA Efficiencies: SA Muons wrt Reco muons which passed LVL2 MuFast
    addHistogram(new TH1F("EffSA_L2Aware_pt", "pt ; p_{T}[GeV/c]", 100,0.,100.), histdirmuonef);
    addHistogram(new TH1F("EffSA_L2Aware_eta", "eta ; #eta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("EffSA_L2Aware_phi", "phi ; #phi[rad]", 100,-3.15,3.15), histdirmuonef);


    // correlation histograms -- EFCB vs Reco muons which passed LVL2 MuComb
    addHistogram(new TH2F("EF_CBwrtRecL2Aware_PtCor", "PtCor_CBwrtRecMuCombAware ; RecCBmuon_Pt [GeV/c]; EFCBmuon_Pt [GeV/c]", 100, 0.,100., 100, 0.,100.), histdirmuonef);
    addHistogram(new TH2F("EF_CBwrtRecL2Aware_EtaCor", "EtaCor_CBwrtRecMuCombAware ; RecCBmuon_Eta ; EFCBmuon_Eta", 100,-3.2,3.2, 100,-3.2,3.2), histdirmuonef);
    addHistogram(new TH2F("EF_CBwrtRecL2Aware_PhiCor", "PhiCor_CBwrtRecMuCombAware ; RecCBmuon_Phi [rad]; EFCBmuon_Phi [rad]", 100,-3.15,3.15, 100,-3.15,3.15), histdirmuonef);
    addHistogram(new TH1F("EF_CBwrtRecL2Aware_dPt", "dPt_CBwrtRecMuCombAware ; dPt [GeV/c]", 100,-0.5,0.5), histdirmuonef);
    addHistogram(new TH1F("EF_CBwrtRecL2Aware_dEta", "dEta_CBwrtRecMuCombAware ; dEta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("EF_CBwrtRecL2Aware_dPhi", "dPhi_CBwrtRecMuCombAware ; dPhi [rad]", 100,-3.15,3.15), histdirmuonef);

    // Muon EF CB Efficiencies: CB Muons wrt Reco muons which passed LVL2 MuComb
    addHistogram(new TH1F("EffCB_L2Aware_pt", "MuCombAware_pt ; p_{T}[GeV/c]", 100,-0.5,0.5), histdirmuonef);
    addHistogram(new TH1F("EffCB_L2Aware_eta", "MuCombAware_eta ; #eta", 200,-5.,5.), histdirmuonef);
    addHistogram(new TH1F("EffCB_L2Aware_phi", "MuCombAware_phi ; #phi[rad]", 100,-3.15,3.15), histdirmuonef);

    // Efficiency of TrrigMuonEF wrt Offline
    addHistogram( new TH1F("EFMS_effi_toOffl_pt", 	     "TrigMuonEF TrackBuilder effi pt (GeV/c); p_{T}[GeV/c]; Efficiency",		  26, 0, 52), histdireff);
    addHistogram( new TH1F("EFMS_effi_toOffl_pt_numer",    "TrigMuonEF TrackBuilder effi pt (GeV/c) numer; p_{T}[GeV/c]; Entries",	     26, 0, 52), histdirmuonef);
    addHistogram( new TH1F("EFMS_effi_toOffl_pt_denom",    "TrigMuonEF TrackBuilder effi pt (GeV/c) denom; p_{T}[GeV/c]; Entries",	     26, 0, 52), histdirmuonef);
    addHistogram( new TH1F("EFMS_effi_toOffl_eta",	     "TrigMuonEF TrackBuilder effi eta; #eta; Efficiency",	    32, -3.2, 3.2), histdireff );
    addHistogram( new TH1F("EFMS_effi_toOffl_eta_numer",   "TrigMuonEF TrackBuilder effi eta numer; #eta; Entries", 32, -3.2, 3.2), histdirmuonef);
    addHistogram( new TH1F("EFMS_effi_toOffl_eta_denom",   "TrigMuonEF TrackBuilder effi eta denom; #eta; Entries", 32, -3.2, 3.2), histdirmuonef);
    addHistogram( new TH1F("EFMS_effi_toOffl_phi",	     "TrigMuonEF TrackBuilder effi phi; #phi[rad]; Efficiency",	   32, -CLHEP::pi, CLHEP::pi), histdireff );
    addHistogram( new TH1F("EFMS_effi_toOffl_phi_numer",   "TrigMuonEF TrackBuilder effi phi numer; #phi; Entries", 32, -CLHEP::pi, CLHEP::pi), histdirmuonef );
    addHistogram( new TH1F("EFMS_effi_toOffl_phi_denom",   "TrigMuonEF TrackBuilder effi phi denom; #phi; Entries", 32, -CLHEP::pi, CLHEP::pi), histdirmuonef );

    addHistogram( new TH1F("EFSA_effi_toOffl_pt",	    "TrigMuonEF Extrapolator effi pt (GeV/c); p_{T}[GeV/c]; Efficiency",		 26, 0, 52), histdireff);
    addHistogram( new TH1F("EFSA_effi_toOffl_pt_numer",   "TrigMuonEF Extrapolator effi pt (GeV/c) numer; p_{T}[GeV/c]; Entries",	    26, 0, 52), histdirmuonef);
    addHistogram( new TH1F("EFSA_effi_toOffl_pt_denom",   "TrigMuonEF Extrapolator effi pt (GeV/c) denom; p_{T}[GeV/c]; Entries",	    26, 0, 52), histdirmuonef);
    //addHistogram( new TH1F("EFSA_effi_toOffl_eta",	    "TrigMuonEF Extrapolator effi eta; #eta; Efficiency",	   32, -3.2, 3.2), histdireff );
    //addHistogram( new TH1F("EFSA_effi_toOffl_eta_numer",  "TrigMuonEF Extrapolator effi eta numer; #eta; Entries", 32, -3.2, 3.2), histdirmuonef);
    //addHistogram( new TH1F("EFSA_effi_toOffl_eta_denom",  "TrigMuonEF Extrapolator effi eta denom; #eta; Entries", 32, -3.2, 3.2), histdirmuonef);
    //addHistogram( new TH1F("EFSA_effi_toOffl_phi",	    "TrigMuonEF Extrapolator effi phi; #phi[rad]; Efficiency",	  32, -CLHEP::pi, CLHEP::pi), histdireff );
    //addHistogram( new TH1F("EFSA_effi_toOffl_phi_numer",  "TrigMuonEF Extrapolator effi phi numer; #phi; Entries", 32, -CLHEP::pi, CLHEP::pi), histdirmuonef );
    //addHistogram( new TH1F("EFSA_effi_toOffl_phi_denom",  "TrigMuonEF Extrapolator effi phi denom; #phi; Entries", 32, -CLHEP::pi, CLHEP::pi), histdirmuonef );

    addHistogram( new TH1F("EFCB_effi_toOffl_pt",	    "TrigMuonEF Combiner effi pt (GeV/c); p_{T}[GeV/c]; Efficiency",		 26, 0, 52), histdireff);
    addHistogram( new TH1F("EFCB_effi_toOffl_pt_numer",   "TrigMuonEF Combiner effi pt (GeV/c) numer; p_{T}[GeV/c]; Entries",	    26, 0, 52), histdirmuonef);
    addHistogram( new TH1F("EFCB_effi_toOffl_pt_denom",   "TrigMuonEF Combiner effi pt (GeV/c) denom; p_{T}[GeV/c]; Entries",	    26, 0, 52), histdirmuonef);
    //addHistogram( new TH1F("EFCB_effi_toOffl_eta",	    "TrigMuonEF Combiner effi eta; #eta; Efficiency",	   32, -3.2, 3.2), histdireff );
    //addHistogram( new TH1F("EFCB_effi_toOffl_eta_numer",  "TrigMuonEF Combiner effi eta numer; #eta; Entries", 32, -3.2, 3.2), histdirmuonef);
    //addHistogram( new TH1F("EFCB_effi_toOffl_eta_denom",  "TrigMuonEF Combiner effi eta denom; #eta; Entries", 32, -3.2, 3.2), histdirmuonef);
    //addHistogram( new TH1F("EFCB_effi_toOffl_phi",	    "TrigMuonEF Combiner effi phi; #phi[rad]; Efficiency",	  32, -CLHEP::pi, CLHEP::pi), histdireff );
    //addHistogram( new TH1F("EFCB_effi_toOffl_phi_numer",  "TrigMuonEF Combiner effi phi numer; #phi; Entries", 32, -CLHEP::pi, CLHEP::pi), histdirmuonef );
    //addHistogram( new TH1F("EFCB_effi_toOffl_phi_denom",  "TrigMuonEF Combiner effi phi denom; #phi; Entries", 32, -CLHEP::pi, CLHEP::pi), histdirmuonef );

    // MuonEF rate
    // pt > 4GeV
    addHistogram( new TH1F("Number_Of_EF_MS_Muons_4GeV_Cut",     "Number_Of_EF_MS_Muons_4GeV_Cut; LB ; Muons",  400, 1., 801.), histdirrate );
    addHistogram( new TH1F("Number_Of_EF_SA_Muons_4GeV_Cut",     "Number_Of_EF_SA_Muons_4GeV_Cut; LB ; Muons",  400, 1., 801.), histdirrate );
    addHistogram( new TH1F("Number_Of_EF_CB_Muons_4GeV_Cut",     "Number_Of_EF_CB_Muons_4GeV_Cut; LB ; Muons",  400, 1., 801.), histdirrate );

    addHistogram( new TH1F("EF_MS_Over_Moore_MS_4GeV_Cut",     "EF_MS_Over_Moore_MS_4GeV_Cut; LB ; Ratio",  400, 1., 801.), histdirrateratio );
    addHistogram( new TH1F("EF_SA_Over_Moore_SA_4GeV_Cut",     "EF_SA_Over_Moore_SA_4GeV_Cut; LB ; Ratio",  400, 1., 801.), histdirrateratio );
    addHistogram( new TH1F("EF_CB_Over_Muid_4GeV_Cut",         "EF_CB_Over_Muid_4GeV_Cut; LB ; Ratio",  400, 1., 801.), histdirrateratio );

    // pt > 10GeV
    addHistogram( new TH1F("Number_Of_EF_MS_Muons_10GeV_Cut",     "Number_Of_EF_MS_Muons_10GeV_Cut; LB ; Muons",  400, 1., 801.), histdirrate );
    addHistogram( new TH1F("Number_Of_EF_SA_Muons_10GeV_Cut",     "Number_Of_EF_SA_Muons_10GeV_Cut; LB ; Muons",  400, 1., 801.), histdirrate );
    addHistogram( new TH1F("Number_Of_EF_CB_Muons_10GeV_Cut",     "Number_Of_EF_CB_Muons_10GeV_Cut; LB ; Muons",  400, 1., 801.), histdirrate );

    addHistogram( new TH1F("EF_MS_Over_Moore_MS_10GeV_Cut",     "EF_MS_Over_Moore_MS_10GeV_Cut; LB ; Ratio",  400, 1., 801.), histdirrateratio );
    addHistogram( new TH1F("EF_SA_Over_Moore_SA_10GeV_Cut",     "EF_SA_Over_Moore_SA_10GeV_Cut; LB ; Ratio",  400, 1., 801.), histdirrateratio );
    addHistogram( new TH1F("EF_CB_Over_Muid_10GeV_Cut",         "EF_CB_Over_Muid_10GeV_Cut; LB ; Ratio",  400, 1., 801.), histdirrateratio );

  }else if( newLumiBlock ){
  }
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::fillMuonEFDQA()
{
  hist("Common_Counter", histdir )->Fill((float)MUONEF);

  const float DR_MATCHED      = 0.5;
  const float ZERO_LIMIT       = 0.00001;

  const float PT_HISTMAX      = 100.;
  const float PT_OVFL         = 101.5;

  const float EFFI_PT_HISTMAX =  50;
  const float EFFI_PT_OVFL    =  51;

  //Clear Vectors
  pt_EFMSmuon.clear();
  eta_EFMSmuon.clear();
  phi_EFMSmuon.clear();
  
  pt_EFSAmuon.clear();
  eta_EFSAmuon.clear();
  phi_EFSAmuon.clear();
  
  pt_EFCBmuon.clear();
  eta_EFCBmuon.clear();
  phi_EFCBmuon.clear();

  pt_RecMSmuon.clear();
  eta_RecMSmuon.clear();
  phi_RecMSmuon.clear();
  charge_RecMSmuon.clear();
  index_EFMS_RecoMS_corr.clear();

  pt_RecSAmuon.clear();
  eta_RecSAmuon.clear();
  phi_RecSAmuon.clear();
  charge_RecSAmuon.clear();
  index_EFSA_RecoSA_corr.clear();

  pt_RecCBmuon.clear();
  eta_RecCBmuon.clear();
  phi_RecCBmuon.clear();
  charge_RecCBmuon.clear();
  index_EFCB_RecoCB_corr.clear();

  pt_LVL2FASTmuon.clear();
  eta_LVL2FASTmuon.clear();
  phi_LVL2FASTmuon.clear();

  pt_LVL2COMBmuon.clear();
  eta_LVL2COMBmuon.clear();
  phi_LVL2COMBmuon.clear();

  pt_lvl2_RecoSA_corr.clear();
  eta_lvl2_RecoSA_corr.clear();
  phi_lvl2_RecoSA_corr.clear();
  index_lvl2_RecoSA_corr.clear();
  index_EFSA_RecoSA_andL2_corr.clear();

  pt_lvl2_RecoCB_corr.clear();
  eta_lvl2_RecoCB_corr.clear();
  phi_lvl2_RecoCB_corr.clear();
  index_lvl2_RecoCB_corr.clear();
  index_EFCB_RecoCB_andL2_corr.clear();


  int nMuonEFiMS=0, nMuonEFiSA=0, nMuonEFiCB=0;
  int nMuonEFMS=0, nMuonEFSA=0, nMuonEFCB=0;
  int nmethod = 0;   

  // Section 1: simple histograms
  // First try from xAOD::Muon
  const xAOD::MuonContainer* muonEFcontainer(0);
  StatusCode sc_muonEFi = m_storeGate->retrieve(muonEFcontainer, "HLT_xAOD__MuonContainer_MuonEFInfo");
  // ATH_MSG_INFO ( "EF muon xAOD: " << sc_muonEFi );
  
  if (!sc_muonEFi.isFailure()) { // xAOD for offline muon found
    for (auto muon : *muonEFcontainer) {
      ATH_MSG_DEBUG( "Muon found, " << muon->pt() );

      const xAOD::Muon::MuonType muontype = muon->muonType();
      if (xAOD::Muon::MuonType::Combined == muontype
	  || xAOD::Muon::MuonType::MuonStandAlone == muontype) {
	float pt         = float(muon->pt()/CLHEP::GeV);
	//float signed_pt  = float(std::abs(muon->pt())/CLHEP::GeV * ((*muon).charge()))  ;
	const xAOD::TrackParticle* trk = muon->primaryTrackParticle();
	float signed_pt  = float(std::abs(muon->pt())/CLHEP::GeV *trk->charge());
	float eta        = float(muon->eta());
	float phi        = float(muon->phi());

	ATH_MSG_DEBUG( "muonType" << muontype << " : raw pt"
		       << pt << " raw charge " << trk->charge() << " signed pt " << signed_pt );

	float pt_hist = pt;
	if( fabs(pt_hist) > PT_HISTMAX ){ pt_hist =  PT_OVFL; }

	float signed_pt_hist = signed_pt;
	if( signed_pt_hist > PT_HISTMAX ) { signed_pt_hist =  PT_OVFL; }
	else if ( signed_pt_hist < -PT_HISTMAX ){ signed_pt_hist = -PT_OVFL; }

	// fill combined muon information
	if (xAOD::Muon::MuonType::Combined == muontype) {
	  nMuonEFCB++;

	  pt_EFCBmuon.push_back(pt);
	  eta_EFCBmuon.push_back(eta);
	  phi_EFCBmuon.push_back(phi);
		
	  hist("EFCB_pt", histdirmuonef) ->Fill(pt_hist);
	  hist("EFCB_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
	  hist("EFCB_eta", histdirmuonef)->Fill(eta);
	  hist("EFCB_phi", histdirmuonef)->Fill(phi);
	  hist2("EFCB_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
	  hist2("EFCB_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
	}

	// fill corresponding MS muon information
	if (muon->muonSpectrometerTrackParticleLink() != 0) {
	  ATH_MSG_DEBUG(  "EF MS muon found" );

	  nMuonEFMS++;

	  const xAOD::TrackParticle* mooreMuon = 0;
	  mooreMuon = *(muon->muonSpectrometerTrackParticleLink());
	  float pt = mooreMuon->pt()/CLHEP::GeV;
	  float signed_pt  = float(std::abs(muon->pt())/CLHEP::GeV * mooreMuon->charge())  ;
	  float eta = mooreMuon->eta();
	  float phi = mooreMuon->phi();
	  float charge = mooreMuon->charge();

	  float pt_hist = pt;
	  if( fabs(pt_hist) > PT_HISTMAX ){ pt_hist =  PT_OVFL; }

	  float signed_pt_hist = signed_pt;
	  if( signed_pt_hist > PT_HISTMAX ) { signed_pt_hist =  PT_OVFL; }
	  else if ( signed_pt_hist < -PT_HISTMAX ){ signed_pt_hist = -PT_OVFL; }

	  pt_EFMSmuon.push_back(pt);
	  eta_EFMSmuon.push_back(eta);
	  phi_EFMSmuon.push_back(phi);
		
	  hist("EFMS_pt", histdirmuonef) ->Fill(pt_hist);
	  hist("EFMS_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
	  hist("EFMS_eta", histdirmuonef)->Fill(eta);
	  hist("EFMS_phi", histdirmuonef)->Fill(phi);
	  hist2("EFMS_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
	  hist2("EFMS_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);

	}

	// attention: there seems no SA link from CB (according to LY code)
	if (xAOD::Muon::MuonType::MuonStandAlone == muontype) {
	  nMuonEFSA++;

	  pt_EFSAmuon.push_back(pt);
	  eta_EFSAmuon.push_back(eta);
	  phi_EFSAmuon.push_back(phi);              
 
	  hist("EFSA_pt", histdirmuonef) ->Fill(pt_hist);
	  hist("EFSA_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
	  hist("EFSA_eta", histdirmuonef)->Fill(eta);
	  hist("EFSA_phi", histdirmuonef)->Fill(phi);
	  hist2("EFSA_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
	  hist2("EFSA_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
	}
	
      }
    }
  } else {
    ATH_MSG_DEBUG( "Failed to retrieve xAOD Muon" );
    // Retrieve Muon from TrigMuonEFInfoContainer
    ATH_MSG_DEBUG( "about to get TrigMuonEFInfo" );
    const DataHandle< TrigMuonEFInfoContainer > trigMuon, lastTrigMuon;
    StatusCode sc_muonEFi = m_storeGate->retrieve(trigMuon,lastTrigMuon);
    if( sc_muonEFi.isFailure() ) {
      ATH_MSG_DEBUG( "Failed to retrieve HLT TrigMuonEFContainer Muon" );
    }
    ATH_MSG_DEBUG( "finished to get TrigMuonEFInfo " );

    if (sc_muonEFi.isFailure()) {
      ATH_MSG_DEBUG( "Failed to retrieve HLT TrigMuonEFInfoContainer Muon" );
      // Failed to retrieve TrigMuonEFInfoContainer: trying TrigMuonEFContainer 
      const DataHandle<TrigMuonEFContainer> teContainerEnd,teContainerIt;
      StatusCode sc_muonEF = m_storeGate->retrieve(teContainerIt,teContainerEnd);
      if( sc_muonEF.isFailure() ) {
	ATH_MSG_DEBUG( "Failed to retrieve HLT TrigMuonEFContainer Muon" );
      } else {
	ATH_MSG_DEBUG( " ====== START HLTMuon TrigMuonEFContainer MonTool ====== " );
	for ( ; teContainerIt != teContainerEnd; teContainerIt++) {
	  for (TrigMuonEFContainer::const_iterator efit = teContainerIt->begin();efit != teContainerIt->end();efit++) {
	    TrigMuonEF* thisMuo = *efit;
	    int muonType=-1;
	    if (thisMuo) {
	      if ((*efit)->MuonCode()==3) muonType=0;
	      else if ((*efit)->MuonCode()==4) muonType=1;
	      else if ((*efit)->MuonCode()==5) muonType=2;
	    }
	    if (muonType>=0) {
	      if ((*efit)->iPt()!= 0.) {
		float pt         = float(std::abs((*efit)->pt()/CLHEP::GeV));
		float signed_pt  = float(std::abs( (*efit)->pt() )/CLHEP::GeV * (*efit)->Charge())  ;
		float eta        = float((*efit)->eta());
		float phi        = float((*efit)->phi());

		ATH_MSG_DEBUG( "muonType" << muonType << " : raw pt" << (*efit)->pt() << " raw charge " << (*efit)->Charge() << " signed pt " << signed_pt );
            
		float pt_hist = pt;
		if( fabs(pt_hist) > PT_HISTMAX ){ pt_hist =  PT_OVFL; }

		float signed_pt_hist = signed_pt;
		if( signed_pt_hist > PT_HISTMAX ) { signed_pt_hist =  PT_OVFL; }
		else if ( signed_pt_hist < -PT_HISTMAX ){ signed_pt_hist = -PT_OVFL; }

		if (muonType==0) {
		  nMuonEFMS++;

		  pt_EFMSmuon.push_back(pt);
		  eta_EFMSmuon.push_back(eta);
		  phi_EFMSmuon.push_back(phi);
		
		  hist("EFMS_pt", histdirmuonef) ->Fill(pt_hist);
		  hist("EFMS_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
		  hist("EFMS_eta", histdirmuonef)->Fill(eta);
		  hist("EFMS_phi", histdirmuonef)->Fill(phi);
		  hist2("EFMS_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
		  hist2("EFMS_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
		}else if (muonType==1) {
		  nMuonEFSA++;

		  pt_EFSAmuon.push_back(pt);
		  eta_EFSAmuon.push_back(eta);
		  phi_EFSAmuon.push_back(phi);              
 
		  hist("EFSA_pt", histdirmuonef) ->Fill(pt_hist);
		  hist("EFSA_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
		  hist("EFSA_eta", histdirmuonef)->Fill(eta);
		  hist("EFSA_phi", histdirmuonef)->Fill(phi);
		  hist2("EFSA_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
		  hist2("EFSA_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
		}else if (muonType==2) {
		  nMuonEFCB++;

		  pt_EFCBmuon.push_back(pt);
		  eta_EFCBmuon.push_back(eta);
		  phi_EFCBmuon.push_back(phi);
		
		  hist("EFCB_pt", histdirmuonef) ->Fill(pt_hist);
		  hist("EFCB_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
		  hist("EFCB_eta", histdirmuonef)->Fill(eta);
		  hist("EFCB_phi", histdirmuonef)->Fill(phi);
		  hist2("EFCB_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
		  hist2("EFCB_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
		}
	      }
	    }
	  }
	}

	if(nMuonEFMS) nmethod=1;
      }

    } else {
      ATH_MSG_DEBUG( " ====== START HLTMuon TrigMuonEFInfoContainer MonTool ====== " );
      //    int nMuonEFiMS=0, nMuonEFiSA=0, nMuonEFiCB=0;
      for (; trigMuon != lastTrigMuon; trigMuon++) {
	ATH_MSG_DEBUG( " about to get iterator for TrigMuonEFInfoContainer " );
	TrigMuonEFInfoContainer::const_iterator MuonItr  = trigMuon->begin();
	TrigMuonEFInfoContainer::const_iterator MuonItrE = trigMuon->end();
	ATH_MSG_DEBUG( " got iterator for TrigMuonEFInfoContainer: filled? " << (MuonItr != MuonItrE) );

	for (; MuonItr != MuonItrE; MuonItr++ ) {
	  ATH_MSG_DEBUG( "loop over MuonEF track iterator");
	  float pt, signed_pt, eta, phi;
	  TrigMuonEFInfo* muonInfo = (*MuonItr);
	  if (!muonInfo){
	    ATH_MSG_DEBUG( "No TrigMuonEFInfo found" );
	    continue;
	  }
	  ATH_MSG_DEBUG( " muonInfo exists " );
	  // YY another to be fixed
	  if (muonInfo->hasTrack()) {
	    TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();
	    for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin() ; TrackItr!=tc->end(); TrackItr++) {  // loop over container content
	      TrigMuonEFInfoTrack* muonInfoTr = (*TrackItr);
	      TrigMuonEFTrack* muonTrack = muonInfoTr->SpectrometerTrack();
	      if (!muonTrack) {
		ATH_MSG_DEBUG( "No MuonEF Spectrometer track found" );
	      } else {
		ATH_MSG_DEBUG( " SpectrometerTrack exists " );
	      
		if (muonTrack->iPt()!=0.) {
		  //   if (muonTrack->iPt()!=0.) pt = fabs(1./muonTrack->iPt())/1000.;
		
		  pt        = float(std::abs(muonTrack->pt()/CLHEP::GeV));
		  signed_pt = float(std::abs(muonTrack->pt())/CLHEP::GeV * muonTrack->Charge());
		  eta       = float(muonTrack->eta());
		  phi       = float(muonTrack->phi());
		
		  ATH_MSG_DEBUG( "MS :  raw pt " << muonTrack->pt() << " raw charge " << muonTrack->Charge() << " signed pt " << signed_pt );
		  ATH_MSG_DEBUG( " SpectrometerTrack: pt " << pt <<" eta "<<eta<<" phi "<<phi );

		  float pt_hist = pt;
		  if( fabs(pt_hist) > PT_HISTMAX ){ pt_hist =  PT_OVFL; }
		
		  float signed_pt_hist = signed_pt;
		  if( signed_pt_hist > PT_HISTMAX ) { signed_pt_hist =  PT_OVFL; }
		  else if ( signed_pt_hist < -PT_HISTMAX ){ signed_pt_hist = -PT_OVFL; }
		
		  nMuonEFiMS++;
		  if( pt > 4 )
		    hist("Number_Of_EF_MS_Muons_4GeV_Cut",histdirrate)->Fill(m_lumiblock);
		  if( pt > 10 )
		    hist("Number_Of_EF_MS_Muons_10GeV_Cut",histdirrate)->Fill(m_lumiblock);
		
		  pt_EFMSmuon.push_back(pt);
		  eta_EFMSmuon.push_back(eta);
		  phi_EFMSmuon.push_back(phi);
		
		  hist("EFMS_pt", histdirmuonef) ->Fill(pt_hist);
		  hist("EFMS_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
		  hist("EFMS_eta", histdirmuonef)->Fill(eta);
		  hist("EFMS_phi", histdirmuonef)->Fill(phi);
		  hist2("EFMS_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
		  hist2("EFMS_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
		}
	      }
	      muonTrack = muonInfoTr->ExtrapolatedTrack();
	      if (!muonTrack){ 
		ATH_MSG_DEBUG( "No MuonEF Extrapolated track found" );
	      }else{
		ATH_MSG_DEBUG( " ExtrapolatedTrack exists " );
		if (muonTrack->iPt()!=0.) {
	      
		  //        if (muonTrack->iPt()!=0.) pt = fabs(1./muonTrack->iPt())/1000.;
		  pt = float(std::abs(muonTrack->pt()/CLHEP::GeV));
		  signed_pt  = float(std::abs(muonTrack->pt())/CLHEP::GeV * muonTrack->Charge() );
		  eta = float(muonTrack->eta());
		  phi = float(muonTrack->phi());
		
		  float pt_hist = pt;
		  if( fabs(pt_hist) > PT_HISTMAX ){ pt_hist =  PT_OVFL; }
		
		  float signed_pt_hist = signed_pt;
		  if( fabs(signed_pt_hist) > PT_HISTMAX ) {
		    if( signed_pt_hist > 0 ) { signed_pt_hist =  PT_OVFL; }
		    else                    { signed_pt_hist = -PT_OVFL; }
		  }
		
		  ATH_MSG_DEBUG( "SA :  raw pt " << muonTrack->pt() << " raw charge " << muonTrack->Charge() << " signed pt " << signed_pt );
		  ATH_MSG_DEBUG( " ExtrapolatedTrack: pt " << pt <<" eta "<<eta<<" phi "<<phi );
		
		  nMuonEFiSA++;
		  if( pt > 4 )
		    hist("Number_Of_EF_SA_Muons_4GeV_Cut",histdirrate)->Fill(m_lumiblock);
		  if( pt > 10 )
		    hist("Number_Of_EF_SA_Muons_10GeV_Cut",histdirrate)->Fill(m_lumiblock);
		
		  pt_EFSAmuon.push_back(pt);
		  eta_EFSAmuon.push_back(eta);
		  phi_EFSAmuon.push_back(phi);
		
		  hist("EFSA_pt", histdirmuonef) ->Fill(pt_hist);
		  hist("EFSA_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
		  hist("EFSA_eta", histdirmuonef)->Fill(eta);
		  hist("EFSA_phi", histdirmuonef)->Fill(phi);
		  hist2("EFSA_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
		  hist2("EFSA_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
		}
	      }
	      TrigMuonEFCbTrack* muonCbTrack = muonInfoTr->CombinedTrack();
	      if (!muonCbTrack){
		ATH_MSG_DEBUG( "No MuonEF Combined track found" );
	      }else{
		ATH_MSG_DEBUG( " CombinedTrack exists " );
		//        pt  = -9999999.;
		//        if (muonCbTrack->iPt()!=0.) pt = fabs(1./muonCbTrack->iPt())/1000.;
		if (muonCbTrack->iPt()!=0.) {
		  pt = float(std::abs(muonCbTrack->pt()/CLHEP::GeV));
		  //signed_pt = float( muonCbTrack->pt()/GeV * muonCbTrack->Charge() );
		  signed_pt = float( std::abs(muonCbTrack->pt())/CLHEP::GeV * muonCbTrack->Charge() );
		  eta = float(muonCbTrack->eta());
		  phi = float(muonCbTrack->phi());
		
		  float pt_hist = pt;
		  if( fabs(pt_hist) > PT_HISTMAX ){ pt_hist =  PT_OVFL; }
		
		  float signed_pt_hist = signed_pt;
		  if( signed_pt_hist > PT_HISTMAX ) { signed_pt_hist =  PT_OVFL; }
		  else if ( signed_pt_hist < -PT_HISTMAX ){ signed_pt_hist = -PT_OVFL; }
		
		  //ATH_MSG_DEBUG( "CB :  raw pt " << muonCbTrack->pt() << " raw 1/ipt " << 1./muonCbTrack->iPt() << " raw charge " << muonCbTrack->Charge() << " signed pt " << signed_pt );
		  //ATH_MSG_DEBUG( " CombinedTrack:     pt " << pt <<" eta "<<eta<<" phi "<<phi );
	      
		  nMuonEFiCB++;
		  if( pt > 4 )
		    hist("Number_Of_EF_CB_Muons_4GeV_Cut",histdirrate)->Fill(m_lumiblock);
		  if( pt > 10 )
		    hist("Number_Of_EF_CB_Muons_10GeV_Cut",histdirrate)->Fill(m_lumiblock);
		
		  pt_EFCBmuon.push_back(pt);
		  eta_EFCBmuon.push_back(eta);
		  phi_EFCBmuon.push_back(phi);
		
		  hist("EFCB_pt", histdirmuonef) ->Fill(pt_hist);
		  hist("EFCB_signed_pt", histdirmuonef) ->Fill(signed_pt_hist);
		  hist("EFCB_eta", histdirmuonef)->Fill(eta);
		  hist("EFCB_phi", histdirmuonef)->Fill(phi);
		  hist2("EFCB_eta_vs_phi", histdirmuonef)->Fill(eta,phi);
		  hist2("EFCB_eta_vs_phi_in_10LBs", histdircoverage)->Fill(eta,phi);
		}
	      }
	    }
	  }
	  if(nMuonEFiMS) nmethod = 2;
	  ATH_MSG_DEBUG(  " looping over MuonItr " );
	}
	ATH_MSG_DEBUG(  " looping over trigMuon " );
      }
    }
  } // else xAOD muon found

  ATH_MSG_DEBUG(  "finish to loop over TrigMuonEFInfo Container " );
  if(nMuonEFiMS)hist("Common_Counter", histdir)->Fill((float)MUONEFMSFOUND);
  if(nMuonEFiSA)hist("Common_Counter", histdir)->Fill((float)MUONEFSAFOUND);
  if(nMuonEFiCB)hist("Common_Counter", histdir)->Fill((float)MUONEFCBFOUND);
  ATH_MSG_DEBUG(  "filled numbers of MuonEF muons" );

  //  return StatusCode::SUCCESS;
  //}

  //StatusCode HLTMuonMonTool::fillMuonEFOffDQA(){
  int n_RecMSmuon=0;  
  int n_RecSAmuon=0;    
  int n_RecCBmuon=0;

  // Retrieve Muon Offline Reco MuidMuonCollection
  //const Analysis::MuonContainer* muonCont=0;
  const xAOD::MuonContainer* muonCont;

  std::string muonKey = "Muons";
  StatusCode sc = m_storeGate->retrieve( muonCont, muonKey);
  if(sc.isFailure() || !muonCont){
    ATH_MSG_WARNING( "Container of muon particle with key " << muonKey << " not found in Store Gate with size " );
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG(  "Container of muon particle with key found in Store Gate with size " << muonCont->size() );
    xAOD::MuonContainer::const_iterator contItr  = muonCont->begin();
    xAOD::MuonContainer::const_iterator contItrE = muonCont->end();
    
    
    for (; contItr != contItrE; contItr++){

      const xAOD::Muon* recMuon = *contItr;
      float pt  = - 999999.;
      float eta = -999.;
      float phi = -999.;
      float charge = -999.;
 
      ATH_MSG_DEBUG(  "check MS muon " );
      if ((*contItr)->muonSpectrometerTrackParticleLink()!= 0 ){
        //&& 
        //(*contItr)->muonSpectrometerTrackParticle()->iPt() != 0){
        ATH_MSG_DEBUG(  "MS muon found" );

	n_RecMSmuon++;

	const xAOD::TrackParticle* mooreMuon=0;
	mooreMuon = *((*contItr)->muonSpectrometerTrackParticleLink());
	pt = mooreMuon->pt()/CLHEP::GeV;
	eta = mooreMuon->eta();
	phi = mooreMuon->phi();
	charge = mooreMuon->charge();
		  
	pt_RecMSmuon.push_back(pt);
	eta_RecMSmuon.push_back(eta);
	phi_RecMSmuon.push_back(phi);
	charge_RecMSmuon.push_back(charge);
	  
	hist("RecMS_pt", histdirmuonef)->Fill(pt);
	hist("RecMS_eta", histdirmuonef)->Fill(eta);
	hist("RecMS_phi", histdirmuonef)->Fill(phi);

      } 
      ATH_MSG_DEBUG(  "check SA muon " );
	/*  // attention
      if ((*contItr)->muonExtrapolatedTrackParticle()!=0 ){
        ATH_MSG_DEBUG(  "SA muon found" );
	  
	n_RecSAmuon++;
	const Rec::TrackParticle* muidSAMuon = recMuon->muonExtrapolatedTrackParticle();
	pt = muidSAMuon->pt()/CLHEP::GeV;
	eta = muidSAMuon->eta();
	phi = muidSAMuon->phi();
	charge = muidSAMuon->charge();
	  
	pt_RecSAmuon.push_back(pt);
	eta_RecSAmuon.push_back(eta);
	phi_RecSAmuon.push_back(phi);
	charge_RecSAmuon.push_back(charge);
	  
	hist("RecSA_pt", histdirmuonef)->Fill(pt);
	hist("RecSA_eta", histdirmuonef)->Fill(eta);
	hist("RecSA_phi", histdirmuonef)->Fill(phi);
	  	  
      }
	*/
      ATH_MSG_DEBUG(  "check CB muon " );
      if((*contItr)->combinedTrackParticleLink()!=0 ){
        //&&
        //(*contItr)->combinedMuonTrackParticle()->iPt()!=0 ){
        ATH_MSG_DEBUG(  "CB muon found" );
	n_RecCBmuon++;

	const xAOD::TrackParticle* muidCBMuon=0;
	muidCBMuon = *((*contItr)->combinedTrackParticleLink());
	pt = muidCBMuon->pt()/CLHEP::GeV;
	eta = muidCBMuon->eta();
	phi = muidCBMuon->phi();
	charge = muidCBMuon->charge();

	pt_RecCBmuon.push_back(pt);
	eta_RecCBmuon.push_back(eta);
	phi_RecCBmuon.push_back(phi);
	charge_RecCBmuon.push_back(charge);
	  
	hist("RecCB_pt", histdirmuonef)->Fill(pt);
	hist("RecCB_eta", histdirmuonef)->Fill(eta);
	hist("RecCB_phi", histdirmuonef)->Fill(phi);
	  
      }
      // TrigMuonEF TrackBuilder
      for(int i_offl=0;i_offl<(int)pt_RecMSmuon.size();i_offl++) {

        float pt_offl  = pt_RecMSmuon.at(i_offl);
        float eta_offl = eta_RecMSmuon.at(i_offl);
        float phi_offl = phi_RecMSmuon.at(i_offl);
        float charge_offl = charge_RecMSmuon.at(i_offl);
  
        float pt_offl_hist = fabs(pt_offl);
        if( pt_offl_hist > EFFI_PT_HISTMAX )  pt_offl_hist = EFFI_PT_OVFL;
  
        float signed_pt_offl_hist = pt_offl * charge_offl;
        if( signed_pt_offl_hist > EFFI_PT_HISTMAX )  signed_pt_offl_hist = EFFI_PT_OVFL;
        else if( signed_pt_offl_hist < -EFFI_PT_HISTMAX )  signed_pt_offl_hist = -EFFI_PT_OVFL;
  	  
        hist("EFMS_effi_toOffl_pt_denom", histdirmuonef)->Fill(pt_offl_hist);
        hist("EFMS_effi_toOffl_eta_denom", histdirmuonef)->Fill(eta_offl);
        hist("EFMS_effi_toOffl_phi_denom", histdirmuonef)->Fill(phi_offl);
  
        // check whether matching TrigMuonEF TrackBuilder track is there
        float dRmin  = 1000.;
        float pt_efms  = 0.; 
        float eta_efms = 0.; 
        float phi_efms = 0.; 

	// YY: need to convert to xAOD
        const DataHandle< TrigMuonEFInfoContainer > trigMuon, lastTrigMuon;
        StatusCode sc_muonEFiiTB = m_storeGate->retrieve(trigMuon,lastTrigMuon);

        if (!sc_muonEFiiTB.isFailure()) {
        for (; trigMuon != lastTrigMuon; trigMuon++) {
          TrigMuonEFInfoContainer::const_iterator MuonItr  = trigMuon->begin();
          TrigMuonEFInfoContainer::const_iterator MuonItrE = trigMuon->end();
          for (; MuonItr != MuonItrE; MuonItr++ ) {
            TrigMuonEFInfo* muonInfo = (*MuonItr);
	    if (muonInfo->hasTrack()) {
	      TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();  // get the TrackContainer
              for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin()  ; TrackItr!=tc->end() ; TrackItr++) { 
		TrigMuonEFInfoTrack* muonInfoTr = (*TrackItr);
		TrigMuonEFTrack* muonTrack =  muonInfoTr->SpectrometerTrack();
		if (muonTrack && muonTrack->iPt() !=0) {
		  float pt = float(muonTrack->pt()/CLHEP::GeV);
		  if( fabs(pt) < ZERO_LIMIT )  continue;
		  float eta  = muonTrack->eta();
		  float phi  = muonTrack->phi();        
		  float dR = calc_dR(eta, phi, eta_offl, phi_offl);        
		  if( dR < dRmin ) {
		    dRmin = dR;
		    pt_efms = pt;
		    eta_efms = eta;
		    phi_efms = phi;
		  }
		}
	      }
	    }
          }
        }
        }
 
        ///hist("EFMS_dR_toOffl", histdirmuonef)->Fill(dRmin);
        if( dRmin > DR_MATCHED ) continue; // not matched to TrigMuonEF Combiner
  
        hist("EFMS_effi_toOffl_pt_numer", histdirmuonef)->Fill(pt_offl_hist);
        hist("EFMS_effi_toOffl_eta_numer", histdirmuonef)->Fill(eta_offl);
        hist("EFMS_effi_toOffl_phi_numer", histdirmuonef)->Fill(phi_offl);
  
      } // end of offline muon loop for TrigMuonEF TrackBuilder

      // TrigMuonEF Extrapolator
      for(int i_offl=0;i_offl<(int)pt_RecSAmuon.size();i_offl++) {

        float pt_offl     = pt_RecSAmuon.at(i_offl);
        float eta_offl    = eta_RecSAmuon.at(i_offl);
        float phi_offl    = phi_RecSAmuon.at(i_offl);
        float charge_offl = charge_RecSAmuon.at(i_offl);
  
        float pt_offl_hist = fabs(pt_offl);
        if( pt_offl_hist > EFFI_PT_HISTMAX )  pt_offl_hist = EFFI_PT_OVFL;
  
        float signed_pt_offl_hist = pt_offl * charge_offl;
        if( signed_pt_offl_hist > EFFI_PT_HISTMAX )  signed_pt_offl_hist = EFFI_PT_OVFL;
        else if( signed_pt_offl_hist < -EFFI_PT_HISTMAX )  signed_pt_offl_hist = -EFFI_PT_OVFL;
  	  
        hist("EFSA_effi_toOffl_pt_denom", histdirmuonef)->Fill(pt_offl_hist);
        //hist("EFSA_effi_toOffl_eta_denom", histdirmuonef)->Fill(eta_offl);
        //hist("EFSA_effi_toOffl_phi_denom", histdirmuonef)->Fill(phi_offl);
  
        // check whether matching TrigMuonEF Combiner track is there
        float dRmin  = 1000.;
        float pt_efsa  = 0.; 
        float eta_efsa = 0.; 
        float phi_efsa = 0.; 

	// YY: need to convert to xAOD
        const DataHandle< TrigMuonEFInfoContainer > trigMuon, lastTrigMuon;
        StatusCode sc_muonEFiiSA = m_storeGate->retrieve(trigMuon,lastTrigMuon);

        if (!sc_muonEFiiSA.isFailure()) {
        for (; trigMuon != lastTrigMuon; trigMuon++) {
          TrigMuonEFInfoContainer::const_iterator MuonItr  = trigMuon->begin();
          TrigMuonEFInfoContainer::const_iterator MuonItrE = trigMuon->end();
          for (; MuonItr != MuonItrE; MuonItr++ ) {
            TrigMuonEFInfo* muonInfo = (*MuonItr);
	    if (muonInfo->hasTrack()) {
	      TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();  // get the TrackContainer
              for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin(); TrackItr!=tc->end() ; TrackItr++) {  // loop over container content
                TrigMuonEFInfoTrack* muonInfoTr = (*TrackItr);
		
		TrigMuonEFTrack* muonTrack = muonInfoTr->ExtrapolatedTrack();
		if (muonTrack && muonTrack->iPt() !=0) {
		  float pt = float(muonTrack->pt()/CLHEP::GeV);
		  if( fabs(pt) < ZERO_LIMIT )  continue;
		  float eta  = muonTrack->eta();
		  float phi  = muonTrack->phi();
		  float dR = calc_dR(eta, phi, eta_offl, phi_offl);
		  if( dR < dRmin ) {
		    dRmin = dR;
		    pt_efsa = pt;
		    eta_efsa = eta;
		    phi_efsa = phi;
		  }
		}
              }
            }
          }
        }
        }
	
        ///hist("EFSA_dR_toOffl", histdirmuonef)->Fill(dRmin);
        if( dRmin > DR_MATCHED ) continue; // not matched to TrigMuonEF Combiner
  
        hist("EFSA_effi_toOffl_pt_numer", histdirmuonef)->Fill(pt_offl_hist);
        //hist("EFSA_effi_toOffl_eta_numer", histdirmuonef)->Fill(eta_offl);
        //hist("EFSA_effi_toOffl_phi_numer", histdirmuonef)->Fill(phi_offl);
  
      } // end of offline muon loop for TrigMuonEF Extrapolator

      // TrigMuonEF Combiner
      for(int i_offl=0;i_offl<(int)pt_RecCBmuon.size();i_offl++) {

        float pt_offl  = pt_RecCBmuon.at(i_offl);
        float eta_offl = eta_RecCBmuon.at(i_offl);
        float phi_offl = phi_RecCBmuon.at(i_offl);
        float charge_offl = charge_RecCBmuon.at(i_offl);
        
        float pt_offl_hist = fabs(pt_offl);
        if( pt_offl_hist > EFFI_PT_HISTMAX )  pt_offl_hist = EFFI_PT_OVFL;
  
        float signed_pt_offl_hist = pt_offl * charge_offl;
        if( signed_pt_offl_hist > EFFI_PT_HISTMAX )  signed_pt_offl_hist = EFFI_PT_OVFL;
        else if( signed_pt_offl_hist < -EFFI_PT_HISTMAX )  signed_pt_offl_hist = -EFFI_PT_OVFL;
  	  
        hist("EFCB_effi_toOffl_pt_denom", histdirmuonef)->Fill(pt_offl_hist);
        //hist("EFCB_effi_toOffl_eta_denom", histdirmuonef)->Fill(eta_offl);
        //hist("EFCB_effi_toOffl_phi_denom", histdirmuonef)->Fill(phi_offl);
  
        // check whether matching TrigMuonEF Combiner track is there
        float dRmin  = 1000.;
        float pt_efcb  = 0.; 
        float eta_efcb = 0.; 
        float phi_efcb = 0.; 

	// YY: need to convert to xAOD
        const DataHandle< TrigMuonEFInfoContainer > trigMuon, lastTrigMuon;
        StatusCode sc_muonEFiiCB = m_storeGate->retrieve(trigMuon,lastTrigMuon);

        if (!sc_muonEFiiCB.isFailure()) {
        for (; trigMuon != lastTrigMuon; trigMuon++) {
          TrigMuonEFInfoContainer::const_iterator MuonItr  = trigMuon->begin();
          TrigMuonEFInfoContainer::const_iterator MuonItrE = trigMuon->end();
          for (; MuonItr != MuonItrE; MuonItr++ ) {
            TrigMuonEFInfo* muonInfo = (*MuonItr);
	    
	    if (muonInfo->hasTrack()) {
              TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();  // get the TrackContainer
              for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = tc->begin() ; TrackItr!=tc->end(); TrackItr++) {  // loop over container content
                TrigMuonEFInfoTrack* muonInfoTr = (*TrackItr);
		TrigMuonEFCbTrack* muonTrack = muonInfoTr->CombinedTrack();
		if (muonTrack && muonTrack->iPt() != 0.) {
		  float pt = float(muonTrack->pt()/CLHEP::GeV);
		  if( fabs(pt) < ZERO_LIMIT )  continue;
		  float eta  = muonTrack->eta();
		  float phi  = muonTrack->phi();
		  float dR = calc_dR(eta, phi, eta_offl, phi_offl);
		  if( dR < dRmin ) {
		    dRmin = dR;
		    pt_efcb = pt;
		    eta_efcb = eta;
		    phi_efcb = phi;
		  }
		}
              }
            }
          }
        }
        }
 
        ///hist("EFCB_dR_toOffl", histdirmuonef)->Fill(dRmin);

        if( dRmin > DR_MATCHED ) continue; // not matched to TrigMuonEF Combiner
  
        hist("EFCB_effi_toOffl_pt_numer", histdirmuonef)->Fill(pt_offl_hist);
        //hist("EFCB_effi_toOffl_eta_numer", histdirmuonef)->Fill(eta_offl);
        //hist("EFCB_effi_toOffl_phi_numer", histdirmuonef)->Fill(phi_offl);
  
      } // end of offline muon loop for TrigMuonEF Combiner

      // Just in case of tagging muonalgorithms
      /******************************************
	// if((*contItr)->isLowPtReconstructedMuon()){
	if((*contItr)->isSegmentTaggedMuon() && (*contItr)->inDetTrackParticle()!=0)){
          hist("RecLPt_pt")->Fill(pt);
          hist("RecLPt_eta")->Fill(eta);
          hist("RecLPt_phi")->Fill(phi);
        }
      *****************************************/
    }

  }

  const float FAILED_PT_THRES = 0.00001;

  // Retrieve MuonFeatureContainer
  const DataHandle<MuonFeatureContainer> mfContainer;
  const DataHandle<MuonFeatureContainer> lastmfContainer;
  StatusCode sc_mf = m_storeGate->retrieve(mfContainer,lastmfContainer);
  if ( sc_mf.isFailure() ) {
    ATH_MSG_DEBUG( "Failed to retrieve HLT muFast container" );
    return StatusCode::SUCCESS;    
  }

  // Loop over muonFeatureContainer
  int nMuFast=0;
  for(; mfContainer != lastmfContainer; mfContainer++) {

    MuonFeatureContainer::const_iterator mf     = mfContainer->begin();
    MuonFeatureContainer::const_iterator lastmf = mfContainer->end();

    for(; mf != lastmf; mf++) {
 
      if( (*mf)==0 ) continue;

      float pt     = (*mf)->pt();
      float eta    = (*mf)->eta();
      float phi    = (*mf)->phi();
     
      if( fabs(pt)>FAILED_PT_THRES ){
	nMuFast++;
	pt_LVL2FASTmuon.push_back(pt);
	eta_LVL2FASTmuon.push_back(eta);
	phi_LVL2FASTmuon.push_back(phi);
      } 
       
    } // loop over muonFeature
  } // loop over muonFeatureContainer


  // SELECTION OF RECO  MUON OBJECTS WICH PASSED LVL2 MuFast

  int n_Recmuons_PassesdLVL2MuFast = 0;

  // Safe condition in case of MuComb container is empty
  if(nMuFast != 0 && n_RecSAmuon != 0){

    for (int j=0; j<nMuFast; j++) {

      double deltaR = 999.;
      double dR = 0, dphi = 0., deta = 0.;
      float pt_ToUPDATE = -999999., eta_ToUPDATE = -999., phi_ToUPDATE=-999.;
      int kmin = -1; 

      for (int k=0; k<n_RecSAmuon; k++) {

	deta = eta_RecSAmuon.at(k)-eta_LVL2FASTmuon.at(j);
	dphi = phi_RecSAmuon.at(k)-phi_LVL2FASTmuon.at(j);

	if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
	if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);
	dR = sqrt( deta*deta + dphi*dphi ) ;
       
	if (dR < deltaR) {
	  deltaR = dR;
	  kmin = k; 
	}  
      } // close loop on k

 
      if(kmin>-1 && deltaR < m_deltaRCut ) {
	pt_ToUPDATE = pt_RecSAmuon.at(kmin);
	eta_ToUPDATE = eta_RecSAmuon.at(kmin);
	phi_ToUPDATE = phi_RecSAmuon.at(kmin);
       
	n_Recmuons_PassesdLVL2MuFast++;
      }     
       
      pt_lvl2_RecoSA_corr.push_back(pt_ToUPDATE);
      eta_lvl2_RecoSA_corr.push_back(eta_ToUPDATE);
      phi_lvl2_RecoSA_corr.push_back(phi_ToUPDATE);
      index_lvl2_RecoSA_corr.push_back(kmin);
    }//end of loop on j
  }

  // **************************************     Efficiency of EF wrt Reco muons which passed LVL2 MuFast  ***************************************
  // Efficiency of EF wrt Reco muons which passed LVL2 MuFast

  // (YY commented for suppressing warning) int n_L2AwareMuons_passedEFSA = 0;
 
  int nMuonEFSAtoLoop = 0;

  if(nmethod == 2) {
    nMuonEFSAtoLoop = nMuonEFiSA;
  }else if (nmethod == 1){
    nMuonEFSAtoLoop = nMuonEFSA;
  }
  // Safe condition 
  if(n_Recmuons_PassesdLVL2MuFast != 0 && nMuonEFSAtoLoop != 0){

    for (int j=0; j<nMuFast; j++) {

      if (index_lvl2_RecoSA_corr.at(j) < 0)
	continue;

      double deltaR = 999.;
      double dR = 0;
      double pt_EFSA_RecoSA_corr = -999999.;
      double eta_EFSA_RecoSA_corr = -999.;
      double phi_EFSA_RecoSA_corr = -999.;
      int kmin= -1;
     

      for (int k=0; k<nMuonEFSAtoLoop; k++) {

	double deta = -999.;
	double dphi = -999.;

	deta = eta_lvl2_RecoSA_corr.at(j)-eta_EFSAmuon.at(k);
	dphi = phi_lvl2_RecoSA_corr.at(j)-phi_EFSAmuon.at(k);

	if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
	if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);

	dR = sqrt( deta*deta + dphi*dphi ) ;
      
	if (dR < deltaR) {
	  deltaR = dR;
	  kmin = k; 
	}  
      } // close loop on k

      // correlation with Recmuon
      if(kmin > -1 && deltaR < m_deltaRCut){
  
	hist2("EF_SAwrtRecL2Aware_PtCor", histdirmuonef)->Fill(pt_lvl2_RecoSA_corr.at(j),pt_EFSA_RecoSA_corr);
	hist2("EF_SAwrtRecL2Aware_EtaCor", histdirmuonef)->Fill(eta_lvl2_RecoSA_corr.at(j),eta_EFSA_RecoSA_corr);
	hist2("EF_SAwrtRecL2Aware_PhiCor", histdirmuonef)->Fill(phi_lvl2_RecoSA_corr.at(j),phi_EFSA_RecoSA_corr);
	hist("EF_SAwrtRecL2Aware_dPt", histdirmuonef)->Fill(pt_EFSA_RecoSA_corr-pt_lvl2_RecoSA_corr.at(j));
	hist("EF_SAwrtRecL2Aware_dEta", histdirmuonef)->Fill(eta_EFSA_RecoSA_corr-eta_lvl2_RecoSA_corr.at(j));
	hist("EF_SAwrtRecL2Aware_dPhi", histdirmuonef)->Fill(phi_EFSA_RecoSA_corr-phi_lvl2_RecoSA_corr.at(j));
     
       
	hist("EffSA_L2Aware_pt", histdirmuonef)->Fill(pt_EFSA_RecoSA_corr);
	hist("EffSA_L2Aware_eta", histdirmuonef)->Fill(eta_EFSA_RecoSA_corr);
	hist("EffSA_L2Aware_phi", histdirmuonef)->Fill(phi_EFSA_RecoSA_corr);
      }

   
    }
  }


  // **************************************     Efficiency of EF wrt Reco muons which passed LVL2 MuFast  ***************************************

  // Retrieve CombinedMuonFeatureContainer
  const DataHandle<CombinedMuonFeatureContainer> combContainer;
  const DataHandle<CombinedMuonFeatureContainer> lastcombContainer;
  StatusCode sc_comb = m_storeGate->retrieve(combContainer,lastcombContainer);
  if ( sc_comb.isFailure() ) {
    ATH_MSG_DEBUG( "Failed to retrieve HLT muComb container" );
    return StatusCode::SUCCESS;    
  }
 
 
  // Loop over CombinedMuonFeatureContainer
  int nMuComb=0;
  for(; combContainer != lastcombContainer; combContainer++) {
   
    CombinedMuonFeatureContainer::const_iterator comb     = combContainer->begin();
    CombinedMuonFeatureContainer::const_iterator lastcomb = combContainer->end();
   
    for(; comb != lastcomb; comb++) {
     
      if( (*comb)==0 ) continue;
     
      float pt  = (*comb)->pt() / 1000.;  // convert to GeV
      float eta = (*comb)->eta();
      float phi = (*comb)->phi();

      if( fabs(pt)>FAILED_PT_THRES ){
	nMuComb++;
	pt_LVL2COMBmuon.push_back(pt);
	eta_LVL2COMBmuon.push_back(eta);
	phi_LVL2COMBmuon.push_back(phi);
      }
     
    }
  }


  int n_Recmuons_PassesdLVL2MuComb = 0;

  // Safe condition in case of MuComb container is empty
  if(nMuComb != 0 && n_RecCBmuon != 0){

    for (int j=0; j<nMuComb; j++) {

      double deltaR = 999.;
      double dR = 0, dphi = 0., deta = 0.;
      float pt_ToUPDATE = -999999., eta_ToUPDATE = -999., phi_ToUPDATE=-999.;
      int kmin = -1; 

      for (int k=0; k<n_RecCBmuon; k++) {
       
	deta = eta_RecCBmuon.at(k)-eta_LVL2COMBmuon.at(j);
	dphi = phi_RecCBmuon.at(k)-phi_LVL2COMBmuon.at(j);

	if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
	if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);
	dR = sqrt( deta*deta + dphi*dphi ) ;
       
	if (dR < deltaR) {
	  deltaR = dR;
	  kmin = k; 
	}  
      } // close loop on k
	 	
      if(kmin>-1 && deltaR < m_deltaRCut ) {
	pt_ToUPDATE = pt_RecCBmuon.at(kmin);
	eta_ToUPDATE = eta_RecCBmuon.at(kmin);
	phi_ToUPDATE = phi_RecCBmuon.at(kmin);
       
	n_Recmuons_PassesdLVL2MuComb++;
     
      }     
       
      pt_lvl2_RecoCB_corr.push_back(pt_ToUPDATE);
      eta_lvl2_RecoCB_corr.push_back(eta_ToUPDATE);
      phi_lvl2_RecoCB_corr.push_back(phi_ToUPDATE);
      index_lvl2_RecoCB_corr.push_back(kmin);
    }//end of loop on j
  }
   



  // Efficiency of EF wrt Reco muons which passed LVL2 MuComb



  // (YY commented for suppressing warning) int n_L2AwareMuons_passedEFCB = 0;
 
  int nMuonEFCBtoLoop = 0;

  if(nmethod == 2) {
    nMuonEFCBtoLoop = nMuonEFiCB;
  }else if (nmethod == 1){
    nMuonEFCBtoLoop = nMuonEFCB;
  }

  // Safe condition 
  if(n_Recmuons_PassesdLVL2MuComb != 0 && nMuonEFCBtoLoop != 0){

    for (int j=0; j<nMuComb; j++) {

      if (index_lvl2_RecoCB_corr.at(j) < 0)
	continue;

      double deltaR = 999.;
      double dR = 0;
      double pt_EFCB_RecoCB_corr = -999999.;
      double eta_EFCB_RecoCB_corr = -999.;
      double phi_EFCB_RecoCB_corr = -999.;
      int kmin= -1;
     

      for (int k=0; k<nMuonEFCBtoLoop; k++) {
 
	double deta = -999.;
	double dphi = -999.;

	deta = eta_lvl2_RecoCB_corr.at(j)-eta_EFCBmuon.at(k);
	dphi = phi_lvl2_RecoCB_corr.at(j)-phi_EFCBmuon.at(k);

	if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
	if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);
       
	dR = sqrt( deta*deta + dphi*dphi ) ;
      
	if (dR < deltaR) {
	  deltaR = dR;
	  kmin = k; 
	}  

      } // close loop on k

      // correlation with Recmuon
      if(kmin > -1 && deltaR < m_deltaRCut){
  
	hist2("EF_CBwrtRecL2Aware_PtCor", histdirmuonef)->Fill(pt_lvl2_RecoCB_corr.at(j),pt_EFCB_RecoCB_corr);
	hist2("EF_CBwrtRecL2Aware_EtaCor", histdirmuonef)->Fill(eta_lvl2_RecoCB_corr.at(j),eta_EFCB_RecoCB_corr);
	hist2("EF_CBwrtRecL2Aware_PhiCor", histdirmuonef)->Fill(phi_lvl2_RecoCB_corr.at(j),phi_EFCB_RecoCB_corr);
	hist("EF_CBwrtRecL2Aware_dPt", histdirmuonef)->Fill(pt_EFCB_RecoCB_corr-pt_lvl2_RecoCB_corr.at(j));
	hist("EF_CBwrtRecL2Aware_dEta", histdirmuonef)->Fill(eta_EFCB_RecoCB_corr-eta_lvl2_RecoCB_corr.at(j));
	hist("EF_CBwrtRecL2Aware_dPhi", histdirmuonef)->Fill(phi_EFCB_RecoCB_corr-phi_lvl2_RecoCB_corr.at(j));
     
       
	hist("EffCB_L2Aware_pt", histdirmuonef)->Fill(pt_EFCB_RecoCB_corr);
	hist("EffCB_L2Aware_eta", histdirmuonef)->Fill(eta_EFCB_RecoCB_corr);
	hist("EffCB_L2Aware_phi", histdirmuonef)->Fill(phi_EFCB_RecoCB_corr);
      }

   
    }
  }

  //StatusCode HLTMuonMonTool::fillSAMuonEFOnOffDQA(int n_RecSAmuon, int nMuonEFSA){


  // Beginning of Muon Spectrometer Correlations

  int n_Recmuons_passedEFMS = 0;
  int nMuonEFMStoLoop = 0;
 
  if(nmethod == 2) {
    nMuonEFMStoLoop = nMuonEFiMS;
  }else if (nmethod == 1){
    nMuonEFMStoLoop = nMuonEFMS;
  }


  // Safe condition in case of MuonEF container is empty
  if(nMuonEFMStoLoop != 0 && n_RecMSmuon != 0){

    for (int j=0; j<nMuonEFMStoLoop; j++) {
   
      double deltaR = 999., dR = 0, dphi = -999., deta = -999.;
      float pt_MScorrelation = -999999., eta_MScorrelation = -999., phi_MScorrelation=-999.;
      int kmin = -1; 
   
  
      for (int k=0; k<n_RecMSmuon; k++) {
       
	deta = eta_RecMSmuon.at(k)-eta_EFMSmuon.at(j);
	dphi = phi_RecMSmuon.at(k)-phi_EFMSmuon.at(j);

	if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
	if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);
       
	dR = sqrt( deta*deta + dphi*dphi ) ;
      
	if (dR < deltaR) {
	  deltaR = dR;
	  kmin = k;
	}  

      } // close loop on k

      // correlation with RecMSmuon
      if(kmin > -1 && deltaR < m_deltaRCut){
        
        pt_MScorrelation =  pt_RecMSmuon.at(kmin);
        eta_MScorrelation = eta_RecMSmuon.at(kmin);
        phi_MScorrelation = phi_RecMSmuon.at(kmin);
        index_EFMS_RecoMS_corr.push_back(kmin);
        n_Recmuons_passedEFMS++;
	
	hist2("EF_MSMuon_RecMSMuon_PtCor", histdirmuonef)->Fill(pt_MScorrelation,pt_EFMSmuon.at(j));
	hist2("EF_MSMuon_RecMSMuon_EtaCor", histdirmuonef)->Fill(eta_MScorrelation,eta_EFMSmuon.at(j));
	hist2("EF_MSMuon_RecMSMuon_PhiCor", histdirmuonef)->Fill(phi_MScorrelation,phi_EFMSmuon.at(j));
	hist("EF_MSMuon_RecMSMuon_dPt", histdirmuonef)->Fill(pt_MScorrelation-pt_EFMSmuon.at(j));
	hist("EF_MSMuon_RecMSMuon_dEta", histdirmuonef)->Fill(eta_MScorrelation-eta_EFMSmuon.at(j));
	hist("EF_MSMuon_RecMSMuon_dPhi", histdirmuonef)->Fill(phi_MScorrelation-phi_EFMSmuon.at(j));

	hist("RecAwareMSmuon_pt", histdirmuonef)->Fill(pt_MScorrelation);
	hist("RecAwareMSmuon_eta", histdirmuonef)->Fill(eta_MScorrelation);
	hist("RecAwareMSmuon_phi", histdirmuonef)->Fill(phi_MScorrelation);

	hist("EF_MSMuon_RecMSMuon_dR", histdirmuonef)->Fill(deltaR);

      }

    } // close loop on j
  }
 
 
  //  return StatusCode::SUCCESS;
  //}




  // Beginning of Muon Stand Alone Correlations

 
  int n_Recmuons_passedEFSA = 0;
  nMuonEFSAtoLoop = 0;
 
  if(nmethod == 2) {
    nMuonEFSAtoLoop = nMuonEFiSA;
  }else if (nmethod == 1){
    nMuonEFSAtoLoop = nMuonEFSA;
  }


  // Safe condition in case of MuonEF container is empty
  if(nMuonEFSAtoLoop != 0 && n_RecSAmuon != 0){

    for (int j=0; j<nMuonEFSAtoLoop; j++) {
   
      double deltaR = 999., dR = 0, dphi = -999., deta = -999.;
      float pt_SAcorrelation = -999999., eta_SAcorrelation = -999., phi_SAcorrelation=-999.;
      int kmin = -1; 
   
  
      for (int k=0; k<n_RecSAmuon; k++) {
       
	deta = eta_RecSAmuon.at(k)-eta_EFSAmuon.at(j);
	dphi = phi_RecSAmuon.at(k)-phi_EFSAmuon.at(j);

	if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
	if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);
              
	dR = sqrt( deta*deta + dphi*dphi ) ;
      
	if (dR < deltaR) {
	  deltaR = dR;
	  kmin = k; 
	}  

      } // close loop on k

      // correlation with RecSAmuon
      if(kmin > -1 && deltaR < m_deltaRCut){

        pt_SAcorrelation =  pt_RecSAmuon.at(kmin);
        eta_SAcorrelation = eta_RecSAmuon.at(kmin);
        phi_SAcorrelation = phi_RecSAmuon.at(kmin);
        index_EFSA_RecoSA_corr.push_back(kmin);

        n_Recmuons_passedEFSA++;

	hist2("EF_SAMuon_RecSAMuon_PtCor", histdirmuonef)->Fill(pt_SAcorrelation,pt_EFSAmuon.at(j));
	hist2("EF_SAMuon_RecSAMuon_EtaCor", histdirmuonef)->Fill(eta_SAcorrelation,eta_EFSAmuon.at(j));
	hist2("EF_SAMuon_RecSAMuon_PhiCor", histdirmuonef)->Fill(phi_SAcorrelation,phi_EFSAmuon.at(j));
	hist("EF_SAMuon_RecSAMuon_dPt", histdirmuonef)->Fill(pt_SAcorrelation-pt_EFSAmuon.at(j));
	hist("EF_SAMuon_RecSAMuon_dEta", histdirmuonef)->Fill(eta_SAcorrelation-eta_EFSAmuon.at(j));
	hist("EF_SAMuon_RecSAMuon_dPhi", histdirmuonef)->Fill(phi_SAcorrelation-phi_EFSAmuon.at(j));

	hist("RecAwareSAmuon_pt", histdirmuonef)->Fill(pt_SAcorrelation);
	hist("RecAwareSAmuon_eta", histdirmuonef)->Fill(eta_SAcorrelation);
	hist("RecAwareSAmuon_phi", histdirmuonef)->Fill(phi_SAcorrelation);

	hist("EF_SAMuon_RecSAMuon_dR", histdirmuonef)->Fill(deltaR);
      }

    } // close loop on j
  }
 
 
  //  return StatusCode::SUCCESS;
  //}





  // Beginning of Muon Combined Correlation

 
  int n_Recmuons_passedEFCB = 0;
  nMuonEFCBtoLoop = 0;


  if(nmethod == 2) {
    nMuonEFCBtoLoop = nMuonEFiCB;
  }else if (nmethod == 1){
    nMuonEFCBtoLoop = nMuonEFCB;
  }


  // Safe condition in case of MuonEF container is empty
  if(nMuonEFCBtoLoop != 0 && n_RecCBmuon != 0){

    for (int j=0; j<nMuonEFCBtoLoop; j++) {
   
      double deltaR = 999., dR = 0, dphi = -999., deta = -999.;
      float pt_CBcorrelation = -999999., eta_CBcorrelation = -999., phi_CBcorrelation=-999.;
      int kmin = -1; 
   
  
      for (int k=0; k<n_RecCBmuon; k++) {
       
	deta = eta_RecCBmuon.at(k)-eta_EFCBmuon.at(j);
	dphi = phi_RecCBmuon.at(k)-phi_EFCBmuon.at(j);

	if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
	if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);
       
	dR = sqrt( deta*deta + dphi*dphi ) ;
      
	if (dR < deltaR) {
	  deltaR = dR;
	  kmin = k; 
	}  


      } // close loop on k

      // correlation with RecCBmuon
      if(kmin > -1 && deltaR < m_deltaRCut){

        pt_CBcorrelation =  pt_RecCBmuon.at(kmin);
        eta_CBcorrelation = eta_RecCBmuon.at(kmin);
        phi_CBcorrelation = phi_RecCBmuon.at(kmin);
        index_EFCB_RecoCB_corr.push_back(kmin);

        n_Recmuons_passedEFCB++;

	hist2("EF_CBMuon_RecCBMuon_PtCor", histdirmuonef)->Fill(pt_CBcorrelation,pt_EFCBmuon.at(j));
	hist2("EF_CBMuon_RecCBMuon_EtaCor", histdirmuonef)->Fill(eta_CBcorrelation,eta_EFCBmuon.at(j));
	hist2("EF_CBMuon_RecCBMuon_PhiCor", histdirmuonef)->Fill(phi_CBcorrelation,phi_EFCBmuon.at(j));
	hist("EF_CBMuon_RecCBMuon_dPt", histdirmuonef)->Fill(pt_CBcorrelation-pt_EFCBmuon.at(j));
	hist("EF_CBMuon_RecCBMuon_dEta", histdirmuonef)->Fill(eta_CBcorrelation-eta_EFCBmuon.at(j));
	hist("EF_CBMuon_RecCBMuon_dPhi", histdirmuonef)->Fill(phi_CBcorrelation-phi_EFCBmuon.at(j));

	hist("RecAwareCBmuon_pt", histdirmuonef)->Fill(pt_CBcorrelation);
	hist("RecAwareCBmuon_eta", histdirmuonef)->Fill(eta_CBcorrelation);
	hist("RecAwareCBmuon_phi", histdirmuonef)->Fill(phi_CBcorrelation);

	hist("EF_CBMuon_RecCBMuon_dR", histdirmuonef)->Fill(deltaR);
      }

    } // close loop on j
  }
 
  return StatusCode::SUCCESS;
}


StatusCode HLTMuonMonTool::procMuonEFDQA()
{
  if( endOfRun ){

    ATH_MSG_DEBUG("procMuonEFDQA");

    //EF rate pT > 4GeV
    hist("EF_MS_Over_Moore_MS_4GeV_Cut", histdirrateratio)->Sumw2();
    hist("EF_MS_Over_Moore_MS_4GeV_Cut", histdirrateratio)->Divide( hist("Number_Of_EF_MS_Muons_4GeV_Cut", histdirrate), hist("Number_Of_Moore_MS_Muons_4GeV_Cut", histdirrate), 1, 1, "B" );
    
    hist("EF_SA_Over_Moore_SA_4GeV_Cut", histdirrateratio)->Sumw2();
    hist("EF_SA_Over_Moore_SA_4GeV_Cut", histdirrateratio)->Divide( hist("Number_Of_EF_SA_Muons_4GeV_Cut", histdirrate), hist("Number_Of_Moore_SA_Muons_4GeV_Cut", histdirrate), 1, 1, "B" );
    
    hist("EF_CB_Over_Muid_4GeV_Cut", histdirrateratio)    ->Sumw2();
    hist("EF_CB_Over_Muid_4GeV_Cut", histdirrateratio)    ->Divide( hist("Number_Of_EF_CB_Muons_4GeV_Cut", histdirrate), hist("Number_Of_Muid_Muons_4GeV_Cut", histdirrate), 1, 1, "B" );

    //EF rate pT > 10GeV
    hist("EF_MS_Over_Moore_MS_10GeV_Cut", histdirrateratio)->Sumw2();
    hist("EF_MS_Over_Moore_MS_10GeV_Cut", histdirrateratio)->Divide( hist("Number_Of_EF_MS_Muons_10GeV_Cut", histdirrate), hist("Number_Of_Moore_MS_Muons_10GeV_Cut", histdirrate), 1, 1, "B" );
    
    hist("EF_SA_Over_Moore_SA_10GeV_Cut", histdirrateratio)->Sumw2();
    hist("EF_SA_Over_Moore_SA_10GeV_Cut", histdirrateratio)->Divide( hist("Number_Of_EF_SA_Muons_10GeV_Cut", histdirrate), hist("Number_Of_Moore_SA_Muons_10GeV_Cut", histdirrate), 1, 1, "B" );
    
    hist("EF_CB_Over_Muid_10GeV_Cut", histdirrateratio)    ->Sumw2();
    hist("EF_CB_Over_Muid_10GeV_Cut", histdirrateratio)    ->Divide( hist("Number_Of_EF_CB_Muons_10GeV_Cut", histdirrate), hist("Number_Of_Muid_Muons_10GeV_Cut", histdirrate), 1, 1, "B" );

    //efficiency
    hist("EFMS_effi_toOffl_pt", histdireff)->Sumw2();
    hist("EFMS_effi_toOffl_pt", histdireff)->Divide( hist("EFMS_effi_toOffl_pt_numer", histdirmuonef), hist("EFMS_effi_toOffl_pt_denom", histdirmuonef), 1, 1, "B" );
    hist("EFMS_effi_toOffl_eta", histdireff)->Sumw2();
    hist("EFMS_effi_toOffl_eta", histdireff)->Divide( hist("EFMS_effi_toOffl_eta_numer", histdirmuonef), hist("EFMS_effi_toOffl_eta_denom", histdirmuonef), 1, 1, "B" );
    hist("EFMS_effi_toOffl_phi", histdireff)->Sumw2();
    hist("EFMS_effi_toOffl_phi", histdireff)->Divide( hist("EFMS_effi_toOffl_phi_numer", histdirmuonef), hist("EFMS_effi_toOffl_phi_denom", histdirmuonef), 1, 1, "B" );

    hist("EFSA_effi_toOffl_pt", histdireff)->Sumw2();
    hist("EFSA_effi_toOffl_pt", histdireff)->Divide( hist("EFSA_effi_toOffl_pt_numer", histdirmuonef), hist("EFSA_effi_toOffl_pt_denom", histdirmuonef), 1, 1, "B" );
    //hist("EFSA_effi_toOffl_eta", histdireff)->Sumw2();
    //hist("EFSA_effi_toOffl_eta", histdireff)->Divide( hist("EFSA_effi_toOffl_eta_numer", histdirmuonef), hist("EFSA_effi_toOffl_eta_denom", histdirmuonef), 1, 1, "B" );
    //hist("EFSA_effi_toOffl_phi", histdireff)->Sumw2();
    //hist("EFSA_effi_toOffl_phi", histdireff)->Divide( hist("EFSA_effi_toOffl_phi_numer", histdirmuonef), hist("EFSA_effi_toOffl_phi_denom", histdirmuonef), 1, 1, "B" );

    hist("EFCB_effi_toOffl_pt", histdireff)->Sumw2();
    hist("EFCB_effi_toOffl_pt", histdireff)->Divide( hist("EFCB_effi_toOffl_pt_numer", histdirmuonef), hist("EFCB_effi_toOffl_pt_denom", histdirmuonef), 1, 1, "B" );
    //hist("EFCB_effi_toOffl_eta", histdireff)->Sumw2();
    //hist("EFCB_effi_toOffl_eta", histdireff)->Divide( hist("EFCB_effi_toOffl_eta_numer", histdirmuonef), hist("EFCB_effi_toOffl_eta_denom", histdirmuonef), 1, 1, "B" );
    //hist("EFCB_effi_toOffl_phi", histdireff)->Sumw2();
    //hist("EFCB_effi_toOffl_phi", histdireff)->Divide( hist("EFCB_effi_toOffl_phi_numer", histdirmuonef), hist("EFCB_effi_toOffl_phi_denom", histdirmuonef), 1, 1, "B" );


    

  }else if( endOfLumiBlock ){
  }
  return StatusCode::SUCCESS;
}
