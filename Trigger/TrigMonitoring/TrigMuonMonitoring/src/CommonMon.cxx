/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile HLTMuonMonTool.cxx
 *   
 *    authors: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)             
 *             Kunihiro Nagano (nagano@mail.cern.ch)
 */

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/TagInfo.h"

#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"

#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "TrigConfHLTData/HLTUtils.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

//#include "muonEvent/Muon.h"
//#include "muonEvent/MuonContainer.h"
#include "muonEvent/MuonParamDefs.h"

#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/MuonRoI.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/Vertex.h"

#include "xAODPrimitives/IsolationType.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"

#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"

// YY: adding vertex information
#include "VxVertex/Vertex.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"

// YY: adding for muon RoI monitoring

#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "TrigDecisionTool/Conditions.h"

// YY: adding MuonCombinedSelectorTool - not to use for the moment
// #include "MuonCombinedToolInterfaces/IMuonCombinedSelectorTool.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TProfile.h"

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

template <class T> class my_sort{
  public:
    //   bool operator() (std::pair<float, int>& i, std::pair<float, int>& j) {
    bool operator() (const T& i,const T& j) const {
      return (i.first > j.first);
    }
};


float CalculateDeltaR(float off_eta, float off_phi,float on_eta, float on_phi);

StatusCode HLTMuonMonTool::bookCommonDQA()
{

  if( newRunFlag() ){

    ATH_MSG_INFO("start booking Common histograms for newRun");

    addHistogram( new TH1F("HI_PP_Flag", "HI_PP_Flag",4,0,4), m_histdir); 
    addHistogram( new TH1F("Monitoring_Chain", "Monitoring_Chain",15,0,15), m_histdir);
    addHistogram( new TH1F("Common_Counter",           "Event Counter",  30, 0., 30.), m_histdir );
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(EVENT+1,"Event");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUFAST+1,"L2MuonSA");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUFASTFOUND+1,"L2MuonSA found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUFASTDFOUND+1,"L2MuonSA D found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUCOMB+1,"muComb");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUCOMBFOUND+1,"muComb found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUISO+1,"muIso");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUISOFOUND+1,"muIso found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(TILEMU+1,"TileMu");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(TILEMUFOUND+1,"TileMu found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(TILEMUTRACKFOUND+1,"TileMuTrack found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUONEF+1,"MuonEF");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUONEFMSFOUND+1,"MuonEF MS found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUONEFSAFOUND+1,"MuonEF SA found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUONEFCBFOUND+1,"MuonEF CB found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(MUZTP+1,"MuonZTP");

    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(OFLFOUND+1,"Offline found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(OFLMSFOUND+1,"Offline MS found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(OFLSAFOUND+1,"Offline SA found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(OFLCBFOUND+1,"Offline CB found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(OFLCBSELFOUND+1,"selected CB found");
    hist("Common_Counter", m_histdir)->GetXaxis()->SetBinLabel(OFLLPTFOUND+1,"Offline LPT found");

    hist("Common_Counter", m_histdir)->GetXaxis()->LabelsOption("v");

    addHistogram( new TH1F("Number_Of_Events",        "Number_Of_Event; LB ; Events Per 2LBs",  400, 1., 801.), m_histdirrate );
    hist( "Number_Of_Events", m_histdirrate ) -> Sumw2(); 

    //pt > 4GeV
    addHistogram( new TH1F("Number_Of_Moore_MS_Muons_4GeV_Cut", "Number_Of_Moore_MS_Muon_4GeV_Cut; LB ; Moore MS Muons",  400, 1., 801.), m_histdirrate );
    addHistogram( new TH1F("Number_Of_Moore_SA_Muons_4GeV_Cut", "Number_Of_Moore_SA_Muon_4GeV_Cut; LB ; Moore SA Muons",  400, 1., 801.), m_histdirrate );
    addHistogram( new TH1F("Number_Of_Muid_Muons_4GeV_Cut",     "Number_Of_Muid_Muon_4GeV_Cut; LB ; Muid Muons",  400, 1., 801.), m_histdirrate );
    //pt > 10GeV
    addHistogram( new TH1F("Number_Of_Moore_MS_Muons_10GeV_Cut", "Number_Of_Moore_MS_Muon_10GeV_Cut; LB ; Moore MS Muons",  400, 1., 801.), m_histdirrate );
    addHistogram( new TH1F("Number_Of_Moore_SA_Muons_10GeV_Cut", "Number_Of_Moore_SA_Muon_10GeV_Cut; LB ; Moore SA Muons",  400, 1., 801.), m_histdirrate );
    addHistogram( new TH1F("Number_Of_Muid_Muons_10GeV_Cut",     "Number_Of_Muid_Muon_10GeV_Cut; LB ; Muid Muons",  400, 1., 801.), m_histdirrate );

    addHistogram( new TH1F("Offline_Number_Of_Muon",             "Number of Offline Muon; # of Muon; Entries",  11, 0., 11.), m_histdir  );
    addHistogram( new TH1F("Offline_Number_Of_MS_Muon",          "Number of Offline MS Muon; # of Muon; Entries",  11, 0., 11.), m_histdir  );
    addHistogram( new TH1F("Offline_Number_Of_SA_Muon",          "Number of Offline SA Muon; # of Muon; Entries",  11, 0., 11.), m_histdir  );
    addHistogram( new TH1F("Offline_Number_Of_LPT_Muon",         "Number of Offline LPT Muon; # of Muon; Entries",  11, 0., 11.), m_histdir  );
    addHistogram( new TH1F("Offline_Number_Of_CB_Muon",          "Number of Offline CB Muon; # of Muon; Entries",  11, 0., 11.), m_histdir );
    addHistogram( new TH1F("Offline_Number_Of_Selected_CB_Muon", "Number of Offline Selected CB Muon; # of Muon; Entries",  11, 0., 11.), m_histdir  );

    //trigger overlap
    int nbin = m_chainsOverlap.size();
    //std::cout << " m_chainsOverlap.size() " << nbin << std::endl;

    addHistogram( new TH2F("Trigger_Overlap", "Trigger overlap for passed events", nbin , 0., (double)nbin, nbin, 0., (double)nbin ), m_histdir  );
    addHistogram( new TH2F("Trigger_Overlap_Including_Passthrough", "Trigger overlap for passed and pass through events",  nbin, 0., (double)nbin, nbin, 0., (double)nbin ), m_histdir  );

    addHistogram( new TH2F("Trigger_Overlap_Fraction", "Trigger overlap fraction for passed events",  nbin , 0., (double)nbin, nbin, 0., (double)nbin ), m_histdir  );
    addHistogram( new TH2F("Trigger_Overlap_Fraction_Including_Passthrough", "Trigger overlap fraction for passed and pass through events", nbin , 0., (double)nbin, nbin, 0., (double)nbin ), m_histdir  );

    //Trigger aware
    for(int i=0; i < (int)m_chainsOverlap.size(); i++ ){

      TString s = m_chainsOverlap[i];
      s.ReplaceAll("HLT_","");
      hist2( "Trigger_Overlap", m_histdir   )->GetXaxis()->SetBinLabel( i + 1, s );
      hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   )->GetXaxis()->SetBinLabel( i + 1, s );
      hist2( "Trigger_Overlap_Fraction", m_histdir   )->GetXaxis()->SetBinLabel( i + 1, s );
      hist2( "Trigger_Overlap_Fraction_Including_Passthrough", m_histdir   )->GetXaxis()->SetBinLabel( i + 1, s );

      hist2( "Trigger_Overlap", m_histdir   )->GetYaxis()->SetBinLabel( i + 1, s );
      hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   )->GetYaxis()->SetBinLabel( i + 1, s );
      hist2( "Trigger_Overlap_Fraction", m_histdir   )->GetYaxis()->SetBinLabel( i + 1, s );
      hist2( "Trigger_Overlap_Fraction_Including_Passthrough", m_histdir   )->GetYaxis()->SetBinLabel( i + 1, s );

    }

    hist2( "Trigger_Overlap", m_histdir   )->GetXaxis()->LabelsOption("v");
    hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   )->GetXaxis()->LabelsOption("v");
    hist2( "Trigger_Overlap_Fraction", m_histdir   )->GetXaxis()->LabelsOption("v");
    hist2( "Trigger_Overlap_Fraction_Including_Passthrough", m_histdir   )->GetXaxis()->LabelsOption("v");

    hist2( "Trigger_Overlap", m_histdir   )->GetYaxis()->LabelsOption("v");
    hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   )->GetYaxis()->LabelsOption("v");
    hist2( "Trigger_Overlap_Fraction", m_histdir   )->GetYaxis()->LabelsOption("v");
    hist2( "Trigger_Overlap_Fraction_Including_Passthrough", m_histdir   )->GetYaxis()->LabelsOption("v");

    //label size
    hist2( "Trigger_Overlap", m_histdir   )->GetXaxis()->SetLabelSize(0.025);
    hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   )->GetXaxis()->SetLabelSize(0.025);
    hist2( "Trigger_Overlap_Fraction", m_histdir   )->GetXaxis()->SetLabelSize(0.025);
    hist2( "Trigger_Overlap_Fraction_Including_Passthrough", m_histdir   )->GetXaxis()->SetLabelSize(0.025);

    hist2( "Trigger_Overlap", m_histdir   )->GetYaxis()->SetLabelSize(0.025);
    hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   )->GetYaxis()->SetLabelSize(0.025);
    hist2( "Trigger_Overlap_Fraction", m_histdir   )->GetYaxis()->SetLabelSize(0.025);
    hist2( "Trigger_Overlap_Fraction_Including_Passthrough", m_histdir   )->GetYaxis()->SetLabelSize(0.025);

    //marker size (text size = 0.02*padheight*markersize)
    hist2( "Trigger_Overlap", m_histdir   )->SetMarkerSize(0.6);
    hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   )->SetMarkerSize(0.6);
    hist2( "Trigger_Overlap_Fraction", m_histdir   )->SetMarkerSize(0.6);
    hist2( "Trigger_Overlap_Fraction_Including_Passthrough", m_histdir   )->SetMarkerSize(0.6);

    //Trigger aware
    for(std::map<std::string, std::string>::iterator  it=m_ztpmap.begin(); it != m_ztpmap.end() ; it++ ){

      std::string name     = "Number_Of_"+ it->second + "_Passed_Events" ;
      std::string nameaxis = name + "; LB ; Events" ;

      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), 400, 1., 801.),m_histdirrate );
      hist( name, m_histdirrate ) -> Sumw2(); 

      name     = it->second + "_Triggers_Rate" ;
      nameaxis = name + "; LB ; Trigger Rate [Hz]" ;

      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), 400, 1., 801.),m_histdirrate );
      hist( name, m_histdirrate ) -> Sumw2(); 

      name     = it->second + "_Triggers_Per_Event" ;
      nameaxis = name + "; LB ; Triggers per Event" ;

      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), 400, 1., 801.),m_histdirrateratio );
      hist( name, m_histdirrateratio ) -> Sumw2(); 

    }

    //ES trigger counter
    nbin = m_allESchain.size();
    addHistogram( new TH1F("Number_Of_ES_Triggers",     "Number_Of_ES_Triggers;; Entries", nbin , 0., (double)nbin), m_histdir );

    for(int i=0; i < (int)m_allESchain.size(); i++ ){

      TString s = m_allESchain[i];
      s.ReplaceAll("HLT_","");
      hist( "Number_Of_ES_Triggers", m_histdir   )->GetXaxis()->SetBinLabel( i + 1, s );

    }


    std::string name     = "L1_MUB_L1_MUE_match_muon_RoIs";
    std::string nameaxis = name + "; category(1-4 good) ; Events" ;
    addHistogram(new TH1F(name.c_str(), nameaxis.c_str(), 8, 0.5, 8.5), m_histdirrate);

    // 2D eta-phi histograms for L1RoI 
    name     = "L1RoI_etaphi";
    nameaxis = name + "; #eta; #phi";
    int eta_nbins = 54;
    float eta_range = 2.7;
    int phi_bins = 64;
    float phi_range = CLHEP::pi;

    addHistogram( new TH2F( name.c_str(), nameaxis.c_str(),
	  eta_nbins, -eta_range, eta_range, phi_bins, -phi_range, phi_range), m_histdirdist2d);
    hist2(name, m_histdirdist2d)->Sumw2();


    ATH_MSG_INFO("finished booking Common histograms for newRun");

  }else if( newLumiBlockFlag() ){
  }

  return StatusCode::SUCCESS;
}

StatusCode HLTMuonMonTool::bookChainDQA()
{

  StatusCode retval = StatusCode::SUCCESS;

  std::vector<std::string>::const_iterator it;

  for(it=m_histChainMSonly.begin(); it != m_histChainMSonly.end() ; it++ ){
    StatusCode sc = bookChainDQA_MSonly(*it);
    if (sc.isFailure()) {
      ATH_MSG_VERBOSE("bookChainDQA_MSonly failed for chain=" << *it );
      retval = StatusCode::RECOVERABLE;
    }
  }

  for(it=m_histChainEFFS.begin(); it != m_histChainEFFS.end() ; it++ ){
    StatusCode sc = bookChainDQA_standard(*it);
    if (sc.isFailure()) {
      ATH_MSG_VERBOSE("bookChainDQA_standard failed for chain=" << *it ); 
      retval = StatusCode::RECOVERABLE;
    }
  }


  // YY adding Generic monitoring (using only TDT information for L2MuonSA, muComb and an EF algorithm)
  for (it = m_histChainGeneric.begin(); it != m_histChainGeneric.end(); it++) {
    StatusCode sc = bookChainDQA_generic(*it,false);
    if (sc.isFailure()) {
      ATH_MSG_VERBOSE("bookChainDQA_generic: failed for chain=" << *it );
      retval = StatusCode::RECOVERABLE;
    }
  }

  for (it = m_histChainEFiso.begin(); it != m_histChainEFiso.end(); it++) {
    StatusCode sc = bookChainDQA_generic(*it,true);
    if (sc.isFailure()) {
      ATH_MSG_VERBOSE("bookChainDQA_generic: failed for chain=" << *it );
      retval = StatusCode::RECOVERABLE;
    }
  }

  return retval;

}

StatusCode HLTMuonMonTool::bookChainDQA_MSonly(const std::string& cName )
{
  std::string chainName = cName;  // YY modified 26.06.2011

  if( newRunFlag() ) {

    ATH_MSG_DEBUG("bookChainDQA_MSonly for chain=" << chainName );

    std::string name;
    std::string nameaxis;

    // 0.25 for <  8GeV : 32bins
    // 0.33 for < 17GeV : 27bins
    // 0.50 for < 25GeV : 16bins
    // 1.00 for >=25GeV : 25bins
    // 2.50 for >=50GeV : 20bins
    // const int lopt_nbins = 32 + 27 + 16 + 25 + 20;
    int tmp_bin = 0;
    if(!m_HI_pp_mode){
     tmp_bin = 32 + 27 + 16;
    }else{
     tmp_bin = 32 + 27 + 16 + 25 + 20;
    }
    const int lopt_nbins = tmp_bin;
    float lopt_bins[150];
    //float lopt_bins[lopt_nbins+1];
    lopt_bins[0]=0.;
    float boundary=0.;

    for( int bin = 1; bin <= lopt_nbins ; bin ++) {
      if( !m_HI_pp_mode){
	if( bin <= 32)      boundary += 0.25;
	if( bin <= 32 + 27) boundary += 1./3.;
	if( bin <= 32 + 27 +16 ) boundary += 0.5;
	lopt_bins[bin] = boundary;
      }else{
	if( bin <= 32 )               boundary += 0.25;
	else if( bin <= 32+27 )       boundary += 1./3.;
	else if( bin <= 32+27+16 )    boundary += 0.5;
	else if( bin <= 32+27+16+25 ) boundary += 1.0;
	else if( bin <= 32+27+16+25+20 ) boundary += 2.5;
	lopt_bins[bin] = boundary;
      }
    }

    float *pt_bins;

    // initialising pt_bins[] 
    int pt_nbins = lopt_nbins;
    pt_bins = lopt_bins;

    // High-pt binning 0-300 GeV
    // 5 GeV for 0-100 GeV     : 20 bins
    // 10 GeV for 100-150 GeV  : 5 bins
    // 50 GeV for 150-300 GeV  : 3 bins
    const int hipt_nbins = 20 + 5 + 3;
    float hipt_bins[hipt_nbins + 1];
    hipt_bins[0] = 0.;
    boundary = 0.;
    for (int ibin = 1; ibin <= hipt_nbins; ibin++) {
      if (ibin <= 20) { boundary += 5.; }
      else if (ibin <= 25) { boundary += 10.; }
      else if (ibin <= 28) { boundary += 50.; }
      hipt_bins[ibin] = boundary;
    }

    //for( int bin = 1 ; bin <= 32 ; bin++) pt_bins[bin]    = float(bin)*0.25;
    //for( int bin = 1 ; bin <= 27 ; bin++) pt_bins[bin+32] = 32.*0.25 + float(bin)*0.25;
    //for( int bin = 1 ; bin <= 16 ; bin++) pt_bins[bin+32+27] = 32.*0.25 + 27.*0.33 + float(bin)*0.50;
    //for( int bin = 1 ; bin <= 26 ; bin++) pt_bins[bin+32+27+16] = 32.*0.25 + 27.*0.33 + 16.*0.50 + float(bin)*1.00;

    /*
       const float pt_min   =  0;
       const float pt_max   = 50;
       const int   pt_nbins = 100;
       */
    const float dr_min   = 0.0;
    const float dr_max   = 1.0;
    const int   dr_nbins = 100;

    int eta_nbins = 108;
    float eta_range = 2.7;
    int phi_bins = 96;
    float phi_range = CLHEP::pi;

    std::string monalg[3]={"_L2MuonSA", "_MuonEFMS", "_MuonEFSA"};
    std::string bestr[2] = {"_Barrel", "_Endcap"};


    //ES, trigger not-aware
    for (int i = 0; i <= m_maxESbr; i++) {
      if(!m_MS_mon_ESbr[i])continue; 
      name     = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidSA_Denominator";
      nameaxis = name + "; Muid SA pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      // New L1 eff. turn-on curves
      name     = chainName + m_triggerES[i] + "_L1" + "_Turn_On_Curve_wrt_MuidSA";
      nameaxis = name + "; Muid SA pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
      hist(name, m_histdireff)->Sumw2();

      for (int be = 0; be < 2; be++) {
	name     = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidSA" + bestr[be] + "_Denominator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + m_triggerES[i] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + bestr[be] + "_Denominator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + m_triggerES[i] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
	nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();

      }
    }

    for (int ihpt = 0; ihpt < 2; ihpt++) {  // reserving also high-pt and MSb histograms for wrt offline 
      // remove hpt histograms (Li Yuan 27.02.13)
      //trigger not-aware = wrt offline
      if (ihpt == 0) {
	chainName = cName;
	pt_nbins = lopt_nbins;
	pt_bins = lopt_bins;
      }	else if (ihpt == 1) {
	chainName = cName + m_hptName;
	pt_nbins = hipt_nbins;
	pt_bins = hipt_bins;
      }
      if( ihpt > 0 ) continue; 
      name     = chainName + "_Turn_On_Curve_wrt_MuidSA_Denominator";
      nameaxis = name + "; Muid SA pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      // Barrel/endcap (YY 27.05.10) - Monitoring L2MuonSA, MuonEFSA wrt offline
      for (int be = 0; be < 2; be++) {
	name     = chainName + "_Turn_On_Curve_wrt_MuidSA" + bestr[be] + "_Denominator";
	nameaxis = name + "; Muid SA pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();
      }
    }

    chainName = cName;
    pt_bins = lopt_bins;

    std::string wrtalg[3]={"_L1", "_L2MuonSA", "_L2MuonSA"};

    // wrt upstream
    for(int alg=0; alg<3;alg++){

      for (int ihpt = 0; ihpt < 2; ihpt++) {  // reserving also high-pt and MSb histograms for wrt offline 
	// remove hpt histograms (Li Yuan 27.02.13)
	//trigger not-aware = wrt offline
	if (ihpt == 0) {
	  chainName = cName;
	  pt_nbins = lopt_nbins;
	  pt_bins = lopt_bins;
	} else if (ihpt == 1) {
	  chainName = cName + m_hptName;
	  pt_nbins = hipt_nbins;
	  pt_bins = hipt_bins;
	} 
	if( ihpt > 0 ) continue;

	name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	nameaxis = name + "; Muid SA pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	nameaxis = name + "; Muid SA pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidSA";
	nameaxis = name + "; Muid SA pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

	name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	nameaxis = name + "; Muid SA pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
      }

      chainName = cName;
      pt_nbins = lopt_nbins;
      pt_bins = lopt_bins;

      // ES histograms
      for (int i = 0; i <= m_maxESbr; i++) {
	if(!m_MS_mon_ESbr[i])continue; 
	name     = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	nameaxis = name + "; Muid SA pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
	nameaxis = name + "; Muid SA pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt_MuidSA";
	nameaxis = name + "; Muid SA pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      }


      for (int i = 0; i <= m_maxESbr; i++) {
	if(!m_MS_mon_ESbr[i])continue; 
	name     = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	nameaxis = name + "; Muid SA pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      }

      // Barrel/endcap (YY 27.05.10) - Monitoring L2MuonSA, MuonEFSA wrt upstream / offline
      // monalg[3]={"_L2MuonSA", "_MuonEFMS", "_MuonEFSA"}; -> select {0, 2}
      if (0 == alg || 2 == alg) {
	for (int ihpt = 0; ihpt < 2; ihpt++) {  // reserving also high-pt and MSb histograms for wrt offline 
	  // remove hpt histograms (Li Yuan 27.02.13)
	  //trigger not-aware = wrt offline
	  if (ihpt == 0) {
	    chainName = cName;
	    pt_nbins = lopt_nbins;
	    pt_bins = lopt_bins;
	  } else if (ihpt == 1) {
	    chainName = cName + m_hptName;
	    pt_nbins = hipt_nbins;
	    pt_bins = hipt_bins;
	  } 
	  if(ihpt > 0) continue;
	  for (int be = 0; be < 2; be++) {
	    name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
	    nameaxis = name + "; Muid SA pT (GeV); Events";
	    addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	    hist(name, m_histdireffnumdenom)->Sumw2();

	    name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
	    nameaxis = name + "; Muid SA pT (GeV); Efficiency";
	    addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	    hist(name, m_histdireff)->Sumw2();
	    addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

	    name     = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	    nameaxis = name + "; Muid SA pT (GeV); Events";
	    addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	    hist(name, m_histdireffnumdenom)->Sumw2();

	    name     = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
	    nameaxis = name + "; Muid SA pT (GeV); Efficiency";
	    addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	    hist(name, m_histdireff)->Sumw2();
	    addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	  }
	}
      }

      chainName = cName;
      // int pt_nbins = lopt_nbins;
      // pt_bins = lopt_bins;

      // 2D eta-phi histograms for 
      name     = chainName + monalg[alg] + "_etaphi_wrt_MuidCB";
      nameaxis = name + "; #eta; #phi";
      addHistogram( new TH2F( name.c_str(), nameaxis.c_str(),
	    eta_nbins, -eta_range, eta_range, phi_bins, -phi_range, phi_range), m_histdirdist2d);
      hist2(name, m_histdirdist2d)->Sumw2();

      // coarse histograms
      name     = chainName + monalg[alg] + "_etaphi_coarse_wrt_MuidCB";
      nameaxis = name + "; #eta; #phi";
      addHistogram( new TH2F( name.c_str(), nameaxis.c_str(),
	    s_eta_cnbins, m_bins_eta, s_phi_cnbins, m_bins_phi), m_histdirdist2d);
      hist2(name, m_histdirdist2d)->Sumw2();
      name     = chainName + monalg[alg] + "_etaphi_coarse1d_wrt_MuidCB";
      nameaxis = name + "; (i_bin#phi * N_#eta + i_bin#eta); Number of events / area";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(),
	    s_eta_cnbins * s_phi_cnbins, 0., (Double_t)(s_eta_cnbins * s_eta_cnbins)), m_histdirdist2d);
      hist(name, m_histdirdist2d)->Sumw2();

      if( alg==2 )continue;
      name     = chainName + "_deltaR_btw_L1_MuidSA_For" + monalg[alg];
      nameaxis = name + "; deltaR; Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), dr_nbins, dr_min, dr_max ), m_histdireffnumdenom );


    }

    // Booking plateau summary histograms

    for (std::vector<std::string>::const_iterator itr = m_vectkwd.begin(); itr < m_vectkwd.end(); itr++) {
      name = chainName + "_highpt_effsummary_by" + *itr;
      if( !m_HI_pp_mode) {
	nameaxis = name + "; Algorithm ; Efficiency 15-25GeV";
      }else{
	nameaxis = name + "; Algorithm ; Efficiency 60-100GeV";
      } 
      addHistogram(new TH1F (name.c_str(), nameaxis.c_str(), 2, -0.5, 0.5+m_fEFSA), m_histdireff);
      TH1 *h = hist(name, m_histdireff);
      h->GetXaxis()->SetBinLabel(iL2MuonSA+1, "L2MuonSA");
      h->GetXaxis()->SetBinLabel(iEFSA+1, "EF SA");
    }

    // High-pt 3-bin summary:
    name = chainName + "_highpt3bins_effwrtL1";
    nameaxis = name + "; pt bins; Efficiency";
    addHistogram(new TH1F (name.c_str(), nameaxis.c_str(), 2, -0.5, 1.5), m_histdireff);
    TH1 *h = hist(name, m_histdireff);
    if(!m_HI_pp_mode){ 
      h->GetXaxis()->SetBinLabel(1, "12-20 GeV Z T&P");
      h->GetXaxis()->SetBinLabel(2, "20-25 GeV Z T&P");
    }else{ 
      h->GetXaxis()->SetBinLabel(1, "60-100 GeV Z T&P");
      h->GetXaxis()->SetBinLabel(2, "100-500 GeV Z T&P");
    }
    ATH_MSG_DEBUG("end bookChainDQA_MSonly for chain=" << chainName );

  } else if(newLumiBlockFlag()){
  }

  return StatusCode::SUCCESS;
}

StatusCode HLTMuonMonTool::bookChainDQA_standard(const std::string& cName )
{
  std::string chainName = cName;  // YY modified 26.06.2011

  if( newRunFlag() ) {

    ATH_MSG_DEBUG("bookChainDQA_standard for chain=" << chainName ); 

    std::string name;
    std::string nameaxis;

    // 0.25 for <  8GeV : 32bins
    // 0.33 for < 17GeV : 27bins
    // 0.50 for < 25GeV : 16bins
    // 1.00 for >=25GeV : 25bins
    // 2.50 for >=50GeV : 20bins
    //const int lopt_nbins = 32 + 27 + 16 + 25 + 20;
    int tmp_bin = 32 + 27 + 16 + 25 + 20;
    if( !m_HI_pp_mode ){
      tmp_bin = 32 + 27 + 16 ;
    }else{
      tmp_bin = 32 + 27 + 16 + 25 + 20;
    }
    const int lopt_nbins = tmp_bin;
    float lopt_bins[150];
    //float lopt_bins[lopt_nbins+1];
    lopt_bins[0]=0.;
    float boundary=0.;
    for( int bin = 1; bin <= lopt_nbins ; bin ++) {
      if( !m_HI_pp_mode ){
	if( bin <= 32 )               boundary += 0.25;
	else if( bin <= 32+27 )       boundary += 1./3.;
	else if( bin <= 32+27+16 )    boundary += 0.5;
	lopt_bins[bin] = boundary;
      }else{
	if( bin <= 32 )               boundary += 0.25;
	else if( bin <= 32+27 )       boundary += 1./3.;
	else if( bin <= 32+27+16 )    boundary += 0.5;
	else if( bin <= 32+27+16+25 ) boundary += 1.0;
	else if( bin <= 32+27+16+25+20 ) boundary += 2.5;
	lopt_bins[bin] = boundary;
      }
    }

    float pt_bins[150];
    //float pt_bins[lopt_nbins+1];

    // initialising pt_bins[] 
    std::string chainName = cName;  // YY modified 26.06.2011
    int pt_nbins = lopt_nbins;
    for (int i = 0; i < pt_nbins + 1; i++) {
      pt_bins[i] = lopt_bins[i];
    }


    //*****************************************//
    // add by Yuan :  to book the histogram //
    std::string bestr[2] = {"_Barrel", "_Endcap"};

    if(chainName.find("EFFS")!= string::npos){

      // require one muon match with the pre_trigger ROI
      name = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      // require one muon match with the pre_trigger ROI,  mu  <= 15
      name = chainName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      // require one muon match with the pre_trigger ROI,  15 < mu  < 20 
      name = chainName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      // require one muon match with the pre_trigger ROI,  20 < mu  
      name = chainName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      // FS trigger efficiency summary for different mu range 
      name = chainName + "_EFplateau_wrtOffline_mu_dependence";
      nameaxis = name + "; <#mu>; efficiency";
      addHistogram(new TH1F (name.c_str(), nameaxis.c_str(), 3, -0.5, 2.5), m_histdireff);
      TH1 *h = hist(name, m_histdireff);
      h->GetXaxis()->SetBinLabel(1, "#mu <= 15");
      h->GetXaxis()->SetBinLabel(2, "15 < #mu <= 20");
      h->GetXaxis()->SetBinLabel(3, "#mu > 20");

      // check the efficiency separately for barrel and endcap region
      for(int be = 0; be < 2; be++){
	name     = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + bestr[be] + "_Denominator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + bestr[be] + "_Numerator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + bestr[be];
	nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      }


      // FS trigger efficiency summary for barrel and endcap
      name = chainName + "_EFplateau_wrtOffline";
      nameaxis = name + "; Region; Efficiency";
      addHistogram(new TH1F (name.c_str(), nameaxis.c_str(), 2, -0.5, 1.5), m_histdireff);
      h = hist(name, m_histdireff);
      if(!m_HI_pp_mode){
      h->GetXaxis()->SetBinLabel(1, "Barrel 4-25 GeV");
      h->GetXaxis()->SetBinLabel(2, "Endcap 4-25 GeV");
      }else{
      h->GetXaxis()->SetBinLabel(1, "Barrel 25-100 GeV");
      h->GetXaxis()->SetBinLabel(2, "Endcap 25-100 GeV");
      }
      // not requiring one muon match with the pre_trigger ROI, only check the efficiency vs subleading pT muon
      name = chainName + "_Turn_On_Curve_wrt_subleading_MuidCB" + "_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_Turn_On_Curve_wrt_subleading_MuidCB" + "_Numerator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = chainName + "_Turn_On_Curve_wrt_subleading_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      // pseudo T&P method to check the pre trigger efficiency 
      name = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB_Numerator";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff);
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      // pseudo T&P method to check the pre trigger efficiency w.r.t L1
      name = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB_Numerator";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      name = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff);
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      // pseudo T&P method to check the pre trigger L1 efficiency w.r.t offline 
      name = "EFFSpre_dimuonTP_L1_Turn_On_Curve_wrt_probe_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff);
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      return StatusCode::SUCCESS;
    }
    //*****************************************//

  } else if(newLumiBlockFlag()){
  }

  return StatusCode::SUCCESS;
}



StatusCode HLTMuonMonTool::bookChainDQA_generic(const std::string& cName, bool isIsoOffline)
{
  std::string chainName = cName;  // YY modified 26.06.2011

  if( newRunFlag() ) {

    ATH_MSG_DEBUG("bookChainDQA_generic for chain=" << chainName ); 

    std::string name;
    std::string nameaxis;

    // ==== Booking histograms ====

    // 0.25 for <  8GeV : 32bins
    // 0.33 for < 17GeV : 27bins
    // 0.50 for < 25GeV : 16bins
    // 1.00 for >=25GeV : 25bins
    // 2.50 for >=50GeV : 20bins
    //const int lopt_nbins = 32 + 27 + 16 + 25 + 20;
    int tmp_bin = 0;
    if(!m_HI_pp_mode){
      tmp_bin = 32 + 27 + 16;
    }else{
      tmp_bin = 32 + 27 + 16 + 25 + 20; 
    }
    const int lopt_nbins = tmp_bin;
    float lopt_bins[150];
    //float lopt_bins[lopt_nbins+1];
    lopt_bins[0]=0.;
    float boundary=0.;
    for( int bin = 1; bin <= lopt_nbins ; bin ++) {
      if(!m_HI_pp_mode){
	if( bin <= 32 )               boundary += 0.25;
	else if( bin <= 32+27 )       boundary += 1./3.;
	else if( bin <= 32+27+16 )    boundary += 0.5;
	lopt_bins[bin] = boundary;
      }else{
	if( bin <= 32 )               boundary += 0.25;
	else if( bin <= 32+27 )       boundary += 1./3.;
	else if( bin <= 32+27+16 )    boundary += 0.5;
	else if( bin <= 32+27+16+25 ) boundary += 1.0;
	else if( bin <= 32+27+16+25+20 ) boundary += 2.5;
	lopt_bins[bin] = boundary;
      }
	//std:: cout << " bin " << bin << " " <<  lopt_bins[bin] << std::endl;
    }

    float pt_bins[150];
    //float pt_bins[lopt_nbins+1];
    // initialising pt_bins[] 
    std::string chainName = cName;  // YY modified 26.06.2011
    int pt_nbins = lopt_nbins;
    for (int i = 0; i < pt_nbins + 1; i++) {
      pt_bins[i] = lopt_bins[i];
    }

    // High-pt binning 0-300 GeV
    // 5 GeV for 0-100 GeV     : 20 bins
    // 10 GeV for 100-150 GeV  : 5 bins
    // 50 GeV for 150-300 GeV  : 3 bins
    const int hipt_nbins = 20 + 5 + 3;
    float hipt_bins[hipt_nbins + 1];
    hipt_bins[0] = 0.;
    boundary = 0.;
    for (int ibin = 1; ibin <= hipt_nbins; ibin++) {
      if (ibin <= 20) { boundary += 5.; }
      else if (ibin <= 25) { boundary += 10.; }
      else if (ibin <= 28) { boundary += 50.; }
      hipt_bins[ibin] = boundary;
    }


    /*
       const float pt_min   =  0;
       const float pt_max   = 50;
       const int   pt_nbins = 100;
       */

    const float dr_min   = 0.0;
    const float dr_max   = 1.0;
    const int   dr_nbins = 100;

    int eta_nbins = 108;
    float eta_range = 2.7;
    int phi_bins = 96;
    float phi_range = CLHEP::pi;

    std::string monalg[3]={"_L2MuonSA", "_MuComb", "_EFmuon"};
    std::string bestr[2] = {"_Barrel", "_Endcap"};


    // for ES
    for (int i = 0; i <= m_maxESbr; i++) {
      if(!m_CB_mon_ESbr[i])continue; 
      name     = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      // New L1 eff. turn-on curves
      name     = chainName + m_triggerES[i] + "_L1" + "_Turn_On_Curve_wrt_MuidCB";
      nameaxis = name + "; Muid CB pT (GeV); Efficiency";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
      hist(name, m_histdireff)->Sumw2();
      addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      for (int be = 0; be < 2; be++) {
	name     = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + m_triggerES[i] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + bestr[be] + "_Denominator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + m_triggerES[i] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
	nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      }
    }

    for (int ihpt = 0; ihpt < 3; ihpt++) {  // reserving also high-pt and MSb histograms for wrt offline and b/e
      //trigger not-aware = wrt offline
      if (ihpt == 0) {
	chainName = cName;
	pt_nbins = lopt_nbins;
	for (int i = 0; i < pt_nbins + 1; i++) {
	  pt_bins[i] = lopt_bins[i];
	}
      } else if (ihpt == 1) {
	chainName = cName + m_hptName;
	pt_nbins = hipt_nbins;
	for (int i = 0; i < pt_nbins + 1; i++) {
	  pt_bins[i] = hipt_bins[i];
	}
      } else { // ihpt == 2 i.e. MSb histos
	chainName = cName + m_MSchainName;
	pt_nbins = hipt_nbins;
	for (int i = 0; i < pt_nbins + 1; i++) {
	  pt_bins[i] = hipt_bins[i];
	}
      }	
      if(ihpt==1)continue;
      // remove hpt histograms (Li Yuan 27.02.13)
      //trigger not-aware
      name     = chainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
      nameaxis = name + "; Muid CB pT (GeV); Events";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
      hist(name, m_histdireffnumdenom)->Sumw2();

      // Barrel/endcap (YY 20.05.10) - Monitoring L2MuonSA, MuComb, MuonEFCB wrt offline
      for (int be = 0; be < 2; be++) {
	name     = chainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();
      }
    }

    std::string wrtalg[3] = {"_L1", "_L2MuonSA", "_MuComb"};

    for (int alg = 0; alg < 3; alg++) {

      for (int ihpt = 0; ihpt < 3; ihpt++) {  // reserving also high-pt and MSb histograms for wrt upstream
	//trigger not-aware = wrt offline
	if (ihpt == 0) {
	  chainName = cName;
	  pt_nbins = lopt_nbins;
	  for (int i = 0; i < pt_nbins + 1; i++) {
	    pt_bins[i] = lopt_bins[i];
	  }
	} else if (ihpt == 1) {
	  chainName = cName + m_hptName;
	  pt_nbins = hipt_nbins;
	  for (int i = 0; i < pt_nbins + 1; i++) {
	    pt_bins[i] = hipt_bins[i];
	  }
	} else { // ihpt == 3 i.e. MSb histos
	  chainName = cName + m_MSchainName;
	  pt_nbins = hipt_nbins;
	  for (int i = 0; i < pt_nbins + 1; i++) {
	    pt_bins[i] = hipt_bins[i];
	  }
	}	

	if(ihpt==1)continue;
	// remove hpt histograms (Li Yuan 27.02.13)

	name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	// YY 20.05.10 - booking histos below, which been perhaps missing?
	// name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	// nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	// addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	// hist(name, m_histdireff)->Sumw2();

	name     = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

	name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      }

      // Reset necessary inside the loop ....
      chainName = cName;
      pt_nbins = lopt_nbins;
      for (int i = 0; i < pt_nbins + 1; i++) {
	pt_bins[i] = lopt_bins[i];
      }

      // for ES
      for (int i = 0; i <= m_maxESbr; i++) {
	if(!m_CB_mon_ESbr[i])continue; 

	name     = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
	nameaxis = name + "; Muid CB pT (GeV); Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	hist(name, m_histdireffnumdenom)->Sumw2();

	name     = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      }


      for (int i = 0; i <= m_maxESbr; i++) {
	if(!m_CB_mon_ESbr[i])continue; 
	name     = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	hist(name, m_histdireff)->Sumw2();
	addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

      }



      // Barrel/endcap (YY 20.05.10) - Monitoring L2MuonSA, MuComb, MuonEFCB wrt upstream / offline
      for (int ihpt = 0; ihpt < 3; ihpt++) {  // reserving also high-pt and MSb histograms for wrt upstream
	//trigger not-aware = wrt offline
	if (ihpt == 0) {
	  chainName = cName;
	  pt_nbins = lopt_nbins;
	  for (int i = 0; i < pt_nbins + 1; i++) {
	    pt_bins[i] = lopt_bins[i];
	  }
	} else if (ihpt == 1) {
	  chainName = cName + m_hptName;
	  pt_nbins = hipt_nbins;
	  for (int i = 0; i < pt_nbins + 1; i++) {
	    pt_bins[i] = hipt_bins[i];
	  }
	} else { // ihpt == 3 i.e. MSb histos
	  chainName = cName + m_MSchainName;
	  pt_nbins = hipt_nbins;
	  for (int i = 0; i < pt_nbins + 1; i++) {
	    pt_bins[i] = hipt_bins[i];
	  }
	}	

	if(ihpt==1)continue;
	// remove hpt histograms (Li Yuan 27.02.13)

	for (int be = 0; be < 2; be++) {
	  name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
	  nameaxis = name + "; Muid CB pT (GeV); Events";
	  addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	  hist(name, m_histdireffnumdenom)->Sumw2();

	  name     = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
	  nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	  addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	  hist(name, m_histdireff)->Sumw2();
	  addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

	  name     = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
	  nameaxis = name + "; Muid CB pT (GeV); Events";
	  addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireffnumdenom );
	  hist(name, m_histdireffnumdenom)->Sumw2();

	  name     = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
	  nameaxis = name + "; Muid CB pT (GeV); Efficiency";
	  addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );
	  hist(name, m_histdireff)->Sumw2();
	  addProfile( new TProfile( (name+"_Profile").c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdireff );

	}
      }

      chainName = cName;
      pt_nbins = lopt_nbins;
      for (int i = 0; i < pt_nbins + 1; i++) {
	pt_bins[i] = lopt_bins[i];
      }
      // 2D eta-phi histograms for each level
      name     = chainName + monalg[alg] + "_etaphi_wrt_MuidCB";
      nameaxis = name + "; #eta; #phi";
      addHistogram( new TH2F( name.c_str(), nameaxis.c_str(),
	    eta_nbins, -eta_range, eta_range, phi_bins, -phi_range, phi_range), m_histdirdist2d);
      hist2(name, m_histdirdist2d)->Sumw2();

      name     = chainName + monalg[alg] + "_etaphi_coarse_wrt_MuidCB";
      nameaxis = name + "; #eta; #phi";
      addHistogram( new TH2F( name.c_str(), nameaxis.c_str(),
	    s_eta_cnbins, m_bins_eta, s_phi_cnbins, m_bins_phi), m_histdirdist2d);
      hist2(name, m_histdirdist2d)->Sumw2();
      name     = chainName + monalg[alg] + "_etaphi_coarse1d_wrt_MuidCB";
      nameaxis = name + "; (i_bin#phi * N_#eta + i_bin#eta); Number of events / area";
      addHistogram( new TH1F( name.c_str(), nameaxis.c_str(),
	    s_eta_cnbins * s_phi_cnbins, 0., (Double_t)(s_eta_cnbins * s_eta_cnbins)), m_histdirdist2d);
      hist(name, m_histdirdist2d)->Sumw2();

      // YY: 1D eta-phi histogram added for mufast
      if (alg == 0) {
	name     = chainName + monalg[alg] + "_eta_wrt_MuidCB";
	nameaxis = name + "; #eta";
	addHistogram(new TH1F(name.c_str(), nameaxis.c_str(), eta_nbins, -eta_range, eta_range), m_histdirdist2d);
	hist(name, m_histdirdist2d)->Sumw2();
	name     = chainName + monalg[alg] + "_phi_wrt_MuidCB";
	nameaxis = name + "; #phi";
	addHistogram(new TH1F(name.c_str(), nameaxis.c_str(), phi_bins, -phi_range, phi_range), m_histdirdist2d);
	hist(name, m_histdirdist2d)->Sumw2();
      }

      // if( alg != 0 && alg != 2 )continue;
      if ( alg == 0 || alg == 2 ) {
	name     = chainName + "_deltaR_btw_L1_MuidCB_For" + monalg[alg];
	nameaxis = name + "; deltaR; Events";
	addHistogram( new TH1F( name.c_str(), nameaxis.c_str(), dr_nbins, dr_min, dr_max ), m_histdireffnumdenom );
      }
    }

    // MuonType-dependent distribution histos

    for (int imtyp = 0; imtyp < 4; imtyp++) {
      name = chainName + m_EFAlgName[imtyp] + "_pt_wrt_MuidCB";
      nameaxis  = name + "; pT; Events";
      addHistogram( new TH1F(name.c_str(), nameaxis.c_str(), pt_nbins, pt_bins), m_histdirdist2d);
      name = chainName + m_EFAlgName[imtyp] + "_eta_wrt_MuidCB";
      nameaxis  = name + "; #eta; Events";
      addHistogram( new TH1F(name.c_str(), nameaxis.c_str(), eta_nbins, -eta_range, eta_range), m_histdirdist2d);
      name = chainName + m_EFAlgName[imtyp] + "_phi_wrt_MuidCB";
      nameaxis  = name + "; #phi; Events";
      addHistogram( new TH1F(name.c_str(), nameaxis.c_str(), phi_bins, -phi_range, phi_range), m_histdirdist2d);
    }

    name = chainName + "_EFMuonType_wrt_MuidCB";
    nameaxis = name + "; Algorithm; Events";
    addHistogram(new TH1F(name.c_str(), nameaxis.c_str(), 3, 0.5, 3.5), m_histdirdist2d);
    TH1 *hhh = hist(name, m_histdirdist2d);
    // hhh->GetXaxis()->SetBinLabel(1, "iPt = 0");
    hhh->GetXaxis()->SetBinLabel(1, m_EFAlgName[0].c_str());
    hhh->GetXaxis()->SetBinLabel(2, m_EFAlgName[1].c_str());
    hhh->GetXaxis()->SetBinLabel(3, m_EFAlgName[2].c_str());

    // Booking plateau summary histograms

    //    for (std::vector<std::string>::const_iterator itr = m_vectkwd.begin(); itr < m_vectkwd.end(); itr++) {
    for (int ies = 0; ies <= m_maxESbr; ies++) {
      if(!m_CB_mon_ESbr[ies])continue; 
      name = chainName + "_highpt_effsummary_by" + m_triggerES[ies]; // for generic: make summary for all histograms
      if( !m_HI_pp_mode ){
	nameaxis = name + "; Algorithm ; Express efficiency 15-25GeV"; 
      }else{
	nameaxis = name + "; Algorithm ; Express efficiency 60-100GeV"; 
      }
      addHistogram(new TH1F (name.c_str(), nameaxis.c_str(), 3, -0.5, 0.5+m_fEFCB), m_histdireff); // for generic
      TH1 *h = hist(name, m_histdireff);
      h->GetXaxis()->SetBinLabel(iL2MuonSA+1, "L2MuonSA");
      h->GetXaxis()->SetBinLabel(iMuComb+1, "MuComb");
      h->GetXaxis()->SetBinLabel(iEFCB+1, "EF algorithm"); // for generic
      // h->GetXaxis()->SetBinLabel(iMuGirl+1, "MuGirl");
    }

    // High-pt 3-bin summary: always 2bins now 
    if (!isIsoOffline) {
      name = chainName + "_highpt3bins_effwrtL1";
      nameaxis = name + "; pt bins; Efficiency";
      addHistogram(new TH1F (name.c_str(), nameaxis.c_str(), 2, -0.5, 1.5), m_histdireff);
      TH1 *h = hist(name, m_histdireff);
      if(!m_HI_pp_mode){
	h->GetXaxis()->SetBinLabel(1, "12-20 GeV Jpsi T&P");
	h->GetXaxis()->SetBinLabel(2, "20-25 GeV Jpsi T&P");
      }else{
	h->GetXaxis()->SetBinLabel(1, "60-100 GeV Z T&P");
	h->GetXaxis()->SetBinLabel(2, "100-500 GeV Z T&P");
      }
    } else {
      name = chainName + "_highpt3bins_effwrtL1";
      nameaxis = name + "; pt bins; Efficiency";
      addHistogram(new TH1F (name.c_str(), nameaxis.c_str(), 2, -0.5, 1.5), m_histdireff);
      TH1 *h = hist(name, m_histdireff);
      if( !m_HI_pp_mode ){
	h->GetXaxis()->SetBinLabel(1, "12-20 GeV Jpsi T&P");
	h->GetXaxis()->SetBinLabel(2, "20-25 GeV Jpsi T&P");
      }else{
	h->GetXaxis()->SetBinLabel(1, "60-100 GeV Z T&P");
	h->GetXaxis()->SetBinLabel(2, "100-500 GeV Z T&P");
      }
    }      

    // High-pt L1 plateau summary (Barrel and Endcap): for generic chain
    name = chainName + "_highptL1plateau_wrtOffline";
    nameaxis = name + "; region; efficiency";
    addHistogram(new TH1F (name.c_str(), nameaxis.c_str(), 2, -0.5, 1.5), m_histdireff);
    TH1 *h = hist(name, m_histdireff);
    if( !m_HI_pp_mode ){
    h->GetXaxis()->SetBinLabel(1, "Barrel 12-20 GeV Jpsi T&P");
    h->GetXaxis()->SetBinLabel(2, "Endcap 12-20 GeV Jpsi T&P");
    }else{
    h->GetXaxis()->SetBinLabel(1, "Barrel 60-100 GeV Z T&P");
    h->GetXaxis()->SetBinLabel(2, "Endcap 60-100 GeV Z T&P");
    }
    ATH_MSG_DEBUG("end bookChainDQA_standard for chain=" << chainName );

  } else if(newLumiBlockFlag()){
  }

  return StatusCode::SUCCESS;
  }



  StatusCode HLTMuonMonTool::fillCommonDQA()
  {
    using namespace Trig;

    static int errcnt = 0;

    ATH_MSG_DEBUG(" fillCommonDQA"); 

    m_lumiblock = -1;
    m_event = -1;
    for(int i=0; i <= INDMBIAS; i++) {
      m_passed[i] = false;
    }

    // YY: get ES vector: moved from fillChainDQA to here
    m_esvect.clear();
    if (0 == errcnt) {
      m_esvect = getESbits();
      if (m_esvect.front() == m_nullstr) {
	if (errcnt < 1) {
	  // if (errcnt < 1000) {
	  ATH_MSG_INFO("Failed to get ES bits");
	  errcnt++;
	}
	// Do not return: continue processing
	}
      }

      const EventInfo* evt(nullptr);
      int event, run;
      if( StatusCode::SUCCESS == evtStore()->retrieve(evt)){
        event = evt->event_ID()->event_number();
        run = evt->event_ID()->run_number();
         ATH_MSG_DEBUG (" EventInfo : "   << " event: " << event << " run: " << run);
      }else{
         ATH_MSG_WARNING (" Unable to retrieve EventInfo from StoreGate ");
 	 hist("Number_Of_Events", m_histdirrate )->Fill( m_lumiblock );
         return StatusCode::SUCCESS;
      }

      if( !evt ){
      	ATH_MSG_WARNING(" Could not find event");
      	hist("Number_Of_Events", m_histdirrate )->Fill( m_lumiblock );
      	return StatusCode::SUCCESS;
      }

      const EventID* evtid = evt->event_ID();

      if(! evtid ){
	ATH_MSG_WARNING(" no evtid object");
	hist("Number_Of_Events", m_histdirrate )->Fill( m_lumiblock );
	return StatusCode::SUCCESS;
      }

      m_lumiblock = evtid->lumi_block() ;
      m_event     = evtid->event_number() ;

      hist("Number_Of_Events", m_histdirrate )->Fill( m_lumiblock );


      //Trigger aware
      for( std::map<std::string, std::string>::iterator it=m_ztpmap.begin(); it != m_ztpmap.end() ; it++ ){
	ATH_MSG_DEBUG( it->first );

	if( getTDT()->isPassed( (it->first) )  ){
	  //ATH_MSG_FATAL(" pass" << *it );
	  std::string name     = "Number_Of_"+ it->second + "_Passed_Events" ;
	  hist( name, m_histdirrate )->Fill( m_lumiblock );

	  name     = it->second + "_Triggers_Rate" ;
	  hist( name, m_histdirrate )->Fill( m_lumiblock, 1/120. );

	}
	//cosmic
	if( getTDT()->isPassed( (it->first + "_cosmic") )  ){
	  //ATH_MSG_FATAL(" pass" << *it <<"_cosmic");
	  std::string name     = "Number_Of_"+ it->second + "_Passed_Events" ;
	  hist( name, m_histdirrate )->Fill( m_lumiblock );

	}
      }//chains



      //Express stream trigger bits: YY 21.01.11
      // updated for real config: 15.02.11
      std::vector<std::string> vs_ESstd;
      if( !m_HI_pp_mode ){
	vs_ESstd.push_back("HLT_mu8"); // increasing stat for muZTP, which requests now ES bits
      }else{
	vs_ESstd.push_back("HLT_mu26_ivarmedium"); // increasing stat for muZTP, which requests now ES bits
      }

      std::vector<std::string> vs_ESnoniso;
      vs_ESnoniso.push_back("HLT_mu8");   // for HI, but HI does not use iso
      vs_ESnoniso.push_back("HLT_mu14");  // for EnhancedBias
      vs_ESnoniso.push_back("HLT_mu26");
      vs_ESnoniso.push_back("HLT_mu24");      
      
      std::vector<std::string> vs_EStag;
      //vs_EStag.push_back("HLT_mu24_muCombTag_NoEF_tight"); // pp v4
      vs_EStag.push_back("HLT_mu20_idperf"); // pp v5
      vs_EStag.push_back("HLT_mu6_idperf");

      std::vector<std::string> vs_ESid;
      //vs_ESid.push_back("HLT_mu18_IDTrkNoCut_tight");  // pp v4
      //vs_ESid.push_back("HLT_mu22_IDTrkNoCut_tight");
      vs_ESid.push_back("HLT_mu20_idperf"); // pp v5
      vs_ESid.push_back("HLT_mu6_idperf");

      // independent chains for L1 monitor
      std::vector<std::string> vs_ESindep;
      vs_ESindep.push_back("HLT_e24vh_medium1");
      vs_ESindep.push_back("HLT_e24vh_medium1_IDTrkNoCut");

      vs_ESindep.push_back("HLT_tau20_medium1");
      vs_ESindep.push_back("HLT_tau29_IDTrkNoCut");

      vs_ESindep.push_back("HLT_j35_a4tchad");
      vs_ESindep.push_back("HLT_j180_a4tchad");
      vs_ESindep.push_back("HLT_b55_NoCut_j55_a4tchad");

      vs_ESindep.push_back("HLT_xe30");
      vs_ESindep.push_back("HLT_xe80");
      vs_ESindep.push_back("HLT_te1200");

      // for HI: three ES streams - names to be checked again
      std::vector<std::string> vs_ESHIL1;
      vs_ESHIL1.push_back("HLT_L1MU0_NoAlg");
      vs_ESHIL1.push_back("HLT_L1MU4_NoAlg");
      std::vector<std::string> vs_ESHIid;
      vs_ESHIid.push_back("HLT_mu4_MSonly_L1TE50");
      vs_ESHIid.push_back("HLT_mu4_IDTrkNoCut");
      vs_ESHIid.push_back("HLT_mu4T_IDTrkNoCut");
      std::vector<std::string> vs_ESHIindep;
      // vs_ESHIindep.push_back("HLT_j15_a2hi_EFFS_L1ZDC");
      vs_ESHIindep.push_back("HLT_j20_a2hi_EFFS_L1ZDC");
      vs_ESHIindep.push_back("HLT_j75_a2hi_EFFS_L1ZDC");
      vs_ESHIindep.push_back("HLT_L1EM16_NoAlg");
      vs_ESHIindep.push_back("HLT_g10_loose");
      vs_ESHIindep.push_back("HLT_2g5_loose");

      for (int i = 0; i <= m_maxESbr; i++) {
	m_passedES[i] = false;
      }

      if (m_requestESchains) {
	std::vector<std::string>::iterator itrES;
	// if (0 == errcnt) {  // fill eschains bit only when we have no error on getting ES bit  //attention
	for (itrES = vs_ESstd.begin(); itrES != vs_ESstd.end(); itrES++) {
	  // if (isPassedES(m_esvect, *itrES)) { // YY modified: no request on lower chain //attention
	  if (getTDT()->isPassed(*itrES)) { // YY modified: no request on lower chain
	    m_passedES[ESSTD] = true;
	    ATH_MSG_DEBUG("----- CommonDQA: ESstd " << *itrES << " and EF " << *itrES << " passed");
	  }
	}
	for (itrES = vs_ESnoniso.begin(); itrES != vs_ESnoniso.end(); itrES++) {
	  if (getTDT()->isPassed(*itrES)) {
	    m_passedESNONISO = true;
	    ATH_MSG_DEBUG("----- CommonDQA: ESnoniso " << *itrES << " and EF " << *itrES << " passed");
	  }
	}
	for (itrES = vs_EStag.begin(); itrES != vs_EStag.end(); itrES++) {
	  //if (isPassedES(m_esvect, *itrES)) { // YY modified: no request on lower chain //attention
	  if (getTDT()->isPassed(*itrES)) { // YY modified: no request on lower chain
	    m_passedES[ESTAG] = true;
	    ATH_MSG_DEBUG("----- CommonDQA: EStag " << *itrES << " and EF " << *itrES << " passed");
	  }
	}
	for (itrES = vs_ESid.begin(); itrES != vs_ESid.end(); itrES++) {
	  //if (isPassedES(m_esvect, *itrES)) { // YY modified: no request on lower chain //attention
	  if (getTDT()->isPassed(*itrES)) { // YY modified: no request on lower chain 
	    m_passedES[ESID] = true;
	    ATH_MSG_DEBUG("----- CommonDQA: ESid " << *itrES << " and EF " << *itrES << " passed");
	  }
	}
	for (itrES = vs_ESindep.begin(); itrES != vs_ESindep.end(); itrES++) {
	  //if (isPassedES(m_esvect, *itrES)) { // YY modified: no request on lower chain //attention
	  if (getTDT()->isPassed(*itrES)) { // YY modified: no request on lower chain
	    m_passedES[ESINDEP] = true;
	    ATH_MSG_DEBUG("----- CommonDQA: ESindep " << *itrES << " and EF " << *itrES << " passed");
	  }
	}
	for (itrES = vs_ESHIL1.begin(); itrES != vs_ESHIL1.end(); itrES++) {
	  if (isPassedES(m_esvect, *itrES)) { // YY modified: no request on lower chain
	    m_passedES[ESHIL1] = true;
	    ATH_MSG_DEBUG("----- CommonDQA: ESHIL1 " << *itrES << " and EF " << *itrES << " passed");
	  }
	}
	for (itrES = vs_ESHIid.begin(); itrES != vs_ESHIid.end(); itrES++) {
	  if (isPassedES(m_esvect, *itrES)) { // YY modified: no request on lower chain
	    m_passedES[ESHIID] = true;
	    ATH_MSG_DEBUG("----- CommonDQA: ESHid " << *itrES << " and EF " << *itrES << " passed");
	  }
	}
	for (itrES = vs_ESHIindep.begin(); itrES != vs_ESHIindep.end(); itrES++) {
	  if (isPassedES(m_esvect, *itrES)) { // YY modified: no request on lower chain
	    m_passedES[ESHIINDEP] = true;
	    ATH_MSG_DEBUG("----- CommonDQA: ESHIindep " << *itrES << " and EF " << *itrES << " passed");
	  }
	}
	// }
	} else {
	  // enabling just standard-chain histogram only
	  m_passedES[ESSTD] = true;
	}


	//measure trigger overlap
	fillTriggerOverlap();

	//new check L1 flag
	StatusCode sc = fillL1MuRoI();
	if ( sc.isFailure() ) {
	  ATH_MSG_WARNING(" Cannot retrieve MuonRoIInfo ");
	  return StatusCode::SUCCESS;
	}

	return StatusCode::SUCCESS;
	}

	StatusCode HLTMuonMonTool::fillChainDQA()
	{

	  ATH_MSG_DEBUG("fillChainDQA"); 

	  StatusCode retval = StatusCode::SUCCESS;
	  // YY: 25.05.10 to clear m_checked:
	  retval.getCode();

	  // getting ES vector moved to CommonDQA

	  for (int i=0; i < (int)m_allESchain.size() ; i++ ){
	    //if ( isPassedES( m_esvect, m_allESchain[i] ) ) hist("Number_Of_ES_Triggers", m_histdir)->Fill(i);
	    if ( getTDT()->isPassed(m_allESchain[i]) ) hist("Number_Of_ES_Triggers", m_histdir)->Fill(i);
	  }


	  std::vector<std::string>::const_iterator it;

	  if( !checkOfflineSAdR() || !checkOfflineCBdR() ) return retval;

	  int itr;
	  for(it=m_chainsMSonly.begin(), itr=0; it != m_chainsMSonly.end() ; it++, itr++ ){
	    StatusCode sc = fillChainDQA_MSonly(*it, m_histChainMSonly[itr]);
	    if (sc.isFailure()) {
	      ATH_MSG_VERBOSE("fillChainDQA_MSonly failed for chain=" << *it );
	      retval = StatusCode::RECOVERABLE;
	    }
	  }

	  for(it=m_chainsEFFS.begin(), itr=0; it != m_chainsEFFS.end() ; it++, itr++ ){
	    if(itr > 0) continue;
	    StatusCode sc = fillChainDQA_standard(*it, m_histChainEFFS[0]);
	    if (sc.isFailure()) {
	      ATH_MSG_VERBOSE("fillChainDQA_standard failed for chain=" << *it );
	      retval = StatusCode::RECOVERABLE;
	    }
	  }
	  for(it=m_chainsGeneric.begin(), itr=0; it != m_chainsGeneric.end() ; it++,itr++ ){
	    StatusCode sc = fillChainDQA_generic(*it, m_histChainGeneric[itr], false);
	    if (sc.isFailure()) {
	      ATH_MSG_VERBOSE("fillChainDQA_generic failed for chain=" << *it );
	      retval = StatusCode::RECOVERABLE;
	    }
	  }

	  for(it=m_chainsEFiso.begin(), itr=0; it != m_chainsEFiso.end() ; it++, itr++ ){
	    StatusCode sc = fillChainDQA_generic(*it, m_histChainEFiso[itr], true);
	    if (sc.isFailure()) {
	      ATH_MSG_VERBOSE("fillChainDQA_generic failed for chain=" << *it );
	      retval = StatusCode::RECOVERABLE;
	    }
	  }

	  return retval;
	}

	HLTMuonMonTool::ptThresEtaRegion HLTMuonMonTool::getEtaRegion(const float eta)
	{
	  if     ( fabs(eta) < 1.05 ) { return BRL; }
	  else if( fabs(eta) < 1.5 )  { return EC1; }
	  else if( fabs(eta) < 2.0 )  { return EC2; }
	  else if( fabs(eta) < 2.5 )  { return EC3; }
	  return EC3;
	}

	float HLTMuonMonTool::getPtThresTrigMuonEFCB(const int thres, const float eta)
	{
	  float pt_thres = -1;
	  static int errcount = 0;

	  ptThresEtaRegion etaRegion = getEtaRegion(eta);

	  if( thres == 40  ) {
	    if     ( etaRegion==BRL ) { pt_thres = 38.76; }
	    else if( etaRegion==EC1 ) { pt_thres = 38.54; }
	    else if( etaRegion==EC2 ) { pt_thres = 38.38; }
	    else                      { pt_thres = 38.31; }
	  }
	  else if( thres == 20 ) {
	    if     ( etaRegion==BRL ) { pt_thres = 19.47; }
	    else if( etaRegion==EC1 ) { pt_thres = 19.33; }
	    else if( etaRegion==EC2 ) { pt_thres = 19.30; }
	    else                      { pt_thres = 19.22; }
	  }
	  else if( thres == 15 ) {
	    if     ( etaRegion==BRL ) { pt_thres = 14.63; }
	    else if( etaRegion==EC1 ) { pt_thres = 14.49; }
	    else if( etaRegion==EC2 ) { pt_thres = 14.42; }
	    else                      { pt_thres = 14.38; }
	  }
	  else if( thres == 13 ) {
	    if     ( etaRegion==BRL ) { pt_thres = 12.67; }
	    else if( etaRegion==EC1 ) { pt_thres = 12.55; }
	    else if( etaRegion==EC2 ) { pt_thres = 12.49; }
	    else                      { pt_thres = 12.46; }
	  }
	  else if( thres == 10 ) {
	    if     ( etaRegion==BRL ) { pt_thres =  9.77; }
	    else if( etaRegion==EC1 ) { pt_thres =  9.67; }
	    else if( etaRegion==EC2 ) { pt_thres =  9.62; }
	    else                      { pt_thres =  9.57; }
	  }
	  else if( thres == 6 ) {
	    if     ( etaRegion==BRL ) { pt_thres =  5.88; }
	    else if( etaRegion==EC1 ) { pt_thres =  5.81; }
	    else if( etaRegion==EC2 ) { pt_thres =  5.78; }
	    else                      { pt_thres =  5.76; }
	  }
	  else  {
	    if( thres != 4 ) {
	      if (errcount < 1) {
		ATH_MSG_INFO("CB: not proper thres=" << thres);
		errcount++;
	      }
	      pt_thres = 100.0; // threshold 100 GeV = efficiency 0%, which can be visible from histograms.
	    } else {
	      if     ( etaRegion==BRL ) { pt_thres =  3.93; }
	      else if( etaRegion==EC1 ) { pt_thres =  3.91; }
	      else if( etaRegion==EC2 ) { pt_thres =  3.88; }
	      else                      { pt_thres =  3.88; }
	    }
	  }


	  return pt_thres;
	}

	float HLTMuonMonTool::getPtThresTrigMuonEFSA(const int thres, const float eta)
	{
	  float pt_thres = -1;
	  static int errcount = 0;

	  ptThresEtaRegion etaRegion = getEtaRegion(eta);

	  // YY 6.10.2010 threshold for 30, 50 and 70 GeV added, they are now correct
	  if( thres == 40  ) {
	    if     ( etaRegion==BRL ) { pt_thres = 31.5; }
	    else if( etaRegion==EC1 ) { pt_thres = 30.0; }
	    else if( etaRegion==EC2 ) { pt_thres = 28.5; }
	    else                      { pt_thres = 32.5; }
	  }
	  else if( thres == 70  ) {
	    if     ( etaRegion==BRL ) { pt_thres = 63.0; }
	    else if( etaRegion==EC1 ) { pt_thres = 63.0; }
	    else if( etaRegion==EC2 ) { pt_thres = 63.0; }
	    else                      { pt_thres = 63.0; }
	  }
	  else if( thres == 60  ) {
	    if     ( etaRegion==BRL ) { pt_thres = 54.0; }
	    else if( etaRegion==EC1 ) { pt_thres = 54.0; }
	    else if( etaRegion==EC2 ) { pt_thres = 54.0; }
	    else                      { pt_thres = 54.0; }
	  }
	  else if( thres == 50  ) {
	    if     ( etaRegion==BRL ) { pt_thres = 45.0; }
	    else if( etaRegion==EC1 ) { pt_thres = 45.0; }
	    else if( etaRegion==EC2 ) { pt_thres = 45.0; }
	    else                      { pt_thres = 45.0; }
	  }
	  else if( thres == 30  ) {
	    if     ( etaRegion==BRL ) { pt_thres = 25.0; }
	    else if( etaRegion==EC1 ) { pt_thres = 24.5; }
	    else if( etaRegion==EC2 ) { pt_thres = 23.0; }
	    else                      { pt_thres = 26.0; }
	  }
	  else if( thres == 22 ) {
	    if     ( etaRegion==BRL ) { pt_thres = 19.1; }
	    else if( etaRegion==EC1 ) { pt_thres = 20.0; }
	    else if( etaRegion==EC2 ) { pt_thres = 18.4; }
	    else                      { pt_thres = 19.6; }
	  }
	  else if( thres == 20 ) {
	    if     ( etaRegion==BRL ) { pt_thres = 17.5; }
	    else if( etaRegion==EC1 ) { pt_thres = 18.5; }
	    else if( etaRegion==EC2 ) { pt_thres = 17.0; }
	    else                      { pt_thres = 18.0; }
	  }
	  else if( thres == 15 ) {
	    if     ( etaRegion==BRL ) { pt_thres = 13.0; }
	    else if( etaRegion==EC1 ) { pt_thres = 14.0; }
	    else if( etaRegion==EC2 ) { pt_thres = 13.0; }
	    else                      { pt_thres = 14.0; }
	  }
	  else if( thres == 13 ) {
	    if     ( etaRegion==BRL ) { pt_thres = 11.4; }
	    else if( etaRegion==EC1 ) { pt_thres = 12.0; }
	    else if( etaRegion==EC2 ) { pt_thres = 11.1; }
	    else                      { pt_thres = 12.0; }
	  }
	  else if( thres == 10 ) {
	    if     ( etaRegion==BRL ) { pt_thres =  8.9; }
	    else if( etaRegion==EC1 ) { pt_thres =  9.0; }
	    else if( etaRegion==EC2 ) { pt_thres =  8.4; }
	    else                      { pt_thres =  9.2; }
	  }
	  else if( thres == 6 ) {
	    if     ( etaRegion==BRL ) { pt_thres =  5.4; }
	    else if( etaRegion==EC1 ) { pt_thres =  4.5; }
	    else if( etaRegion==EC2 ) { pt_thres =  4.9; }
	    else                      { pt_thres =  5.3; }
	  }
	  else {
	    if( thres != 4 ) {
	      if (errcount < 1) {
		ATH_MSG_INFO("SA: not proper thres=" << thres);
		errcount++;
	      }
	      pt_thres = 100.0; // threshold 100 GeV = efficiency 0%, which can be visible from histograms.
	    } else {
	      if     ( etaRegion==BRL ) { pt_thres =  3.0; }
	      else if( etaRegion==EC1 ) { pt_thres =  2.5; }
	      else if( etaRegion==EC2 ) { pt_thres =  2.5; }
	      else                      { pt_thres =  2.5; }
	    }
	  }

	  //

	  return pt_thres;
	}

	StatusCode HLTMuonMonTool::fillChainDQA_MSonly(const std::string& chainName, const std::string & histcName)
	{
	  ATH_MSG_DEBUG("----- fillChainDQA_MSonly: chain=" << chainName << " ----- histchain=" << histcName);

	  using namespace Trig;
	  bool isBarrelChain = false;
	  bool isTighterChain = false;
	  //int nnn = chainName.find("MSonly_barrel");
	  int nnn = chainName.find("0eta105_msonly");  // run2 name changed
	  if (nnn > 0) {
	    isBarrelChain = true;
	  }
	  nnn = chainName.find("MSonly_tighter");
	  if (nnn > 0) {
	    isTighterChain = true;
	  }

	  // 19.07.10 enabling request for lower chain
	  //bool EF_lower_passed = getTDT()->isPassed( m_lowerChains[chainName] );
	  bool EF_lower_passed = true;

	  StatusCode retval = StatusCode::SUCCESS;
	  // YY: 25.05.10 to clear m_checked:
	  retval.getCode();

	  FeatureContainer fHLT = getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs);

	  std::vector<Combination> combsHLT = fHLT.getCombinations();
	  ATH_MSG_DEBUG("nr combsHLT=" << combsHLT.size());  
	  for(std::vector<Combination>::const_iterator it=combsHLT.begin(); it!=combsHLT.end(); it++) {
	    ATH_MSG_DEBUG("..." << *it );
	  }

	  const float DR_CUT = 0.2; // 2012

	  std::string name;
	  int thresh = m_thresh[chainName];

	  std::string bestr[2] = {"_Barrel", "_Endcap"};

	  // loop on each SA probe

	  for(int i_rec=0; i_rec<(int)m_RecMuonSA_pt.size(); i_rec++) {

	    // YY added - remove SA-only tracks from efficiency numerator/denominator
	    /* ATH_MSG_INFO("HLTMuonMon: fillChainDQA_MSonly: " << i_rec << "-th muon, comb flag "
	       << m_RecMuonSA_hasCB[i_rec] << " pt " << m_RecMuonSA_pt[i_rec]); */
	    if (!m_RecMuonSA_isGoodCB[i_rec]) {
	      ATH_MSG_DEBUG("HLTMuonMon: fillChainDQA_MSonly: not a good combined muon" << i_rec);
	      continue;  //for LS1, remove requirements on Hits and impact parameter, attention
	    }

	    float rec_eta = m_RecMuonSA_eta[i_rec];
	    float rec_phi = m_RecMuonSA_phi[i_rec];
	    float rec_pt  = m_RecMuonSA_pt[i_rec];
	    ATH_MSG_DEBUG("++ i_rec=" << i_rec); 
	    ATH_MSG_DEBUG("rec: eta/phi/pt=" << rec_eta << " / " << rec_phi << " / " << rec_pt);
	    //    if( rec_pt > 50. ) rec_pt = 50.;
	    // if( rec_pt > 100. ) rec_pt = 100.;

	    // index for Barrel/Endcap
	    int iBarrelSA = (fabs(rec_eta) < 1.05) ? 0 : 1;
	    int iTighterSA = (fabs(rec_eta) < 2.0) ? 0 : 1;

	    if (0 != iBarrelSA && isBarrelChain) {
	      ATH_MSG_DEBUG( chainName << " is barrel-only, returns since the matching muon is in the endcap ");
	      continue; // exiting the outer-most "for" loop
	    }
	    if (0 != iTighterSA && isTighterChain) {
	      ATH_MSG_DEBUG( chainName << " is for eta < 2.0, returns since the matching muon is in forward area ");
	      continue; // exiting the outer-most "for" loop
	    }

	    if (EF_lower_passed) {
	      name     = histcName + "_Turn_On_Curve_wrt_MuidSA_Denominator";
	      ATH_MSG_DEBUG( name << " filling" ); 
	      hist(name, m_histdireffnumdenom)->Fill(rec_pt);

	      // Barrel/Endcap (YY 27.05.10) Denominator for "wrtOffline"
	      name     = histcName + "_Turn_On_Curve_wrt_MuidSA" + bestr[iBarrelSA] + "_Denominator";
	      ATH_MSG_DEBUG( name << " filling" );
	      hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	    }


	    // if (ES_lower_passed) 
	    // ES trigger-aware
	    for (int i = 0; i <= m_maxESbr; i++) {
	      if(!m_MS_mon_ESbr[i])continue; 
	      if (m_passedES[i]) {
		name     = histcName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidSA_Denominator";
		ATH_MSG_DEBUG( name << " filling" ); 
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		// New Barrel/endcap for ES L1
		name     = histcName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidSA" + bestr[iBarrelSA] + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	      }
	    }

	    // L2 L2MuonSA
	    float dr_min_l2 = 9999;
	    int   id_min_l2 = -1;
	    std::string monalg = "_L2MuonSA";
	    std::string wrtalg = "_L1";

	    for(int iL2=0; iL2<(int)combsHLT.size(); iL2++) {
	      std::vector< Feature<TrigRoiDescriptor> > initRois = combsHLT[iL2].get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
	      if( initRois.size() != 1 ) {
		ATH_MSG_WARNING("nr TrigRoiDescriptor is not 1 for chain=L2_" << chainName);
		retval = StatusCode::RECOVERABLE;
	      }
	      else {
		float dr = calc_dR(rec_eta,rec_phi,initRois[0].cptr()->eta(),initRois[0].cptr()->phi());
		name     = histcName + "_deltaR_btw_L1_MuidSA_For" + monalg;
		ATH_MSG_DEBUG( name << " filling" ); 
		hist(name, m_histdireffnumdenom)->Fill(dr);
		if( dr < dr_min_l2 ) { 
		  dr_min_l2 = dr;
		  id_min_l2 = iL2; 
		}
	      }
	    }

	    //    if( dr_min_l2 < DR_CUT && (EF_lower_passed || ES_lower_passed)) 
	    if( dr_min_l2 < DR_CUT && id_min_l2 != -1 ) {
	      if (EF_lower_passed) {
		ATH_MSG_DEBUG("L2: dRmin=" << dr_min_l2); 
		name     = histcName + monalg + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		// Barrel/Endcap
		name     = histcName + monalg + "_Turn_On_Curve_wrt" + wrtalg + bestr[iBarrelSA] + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	      }
	      // if (ES_lower_passed) 
	      for (int i = 0; i <= m_maxESbr; i++) {
		if(!m_MS_mon_ESbr[i])continue; 
		if (m_passedES[i]) {
		  ATH_MSG_DEBUG("L2: dRmin=" << dr_min_l2);
		  name     = histcName + m_triggerES[i] + monalg + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  // Barrel/Endcap for L1  : monalg = L2MuonSA, wrtalg = L1
		  name     = histcName + m_triggerES[i] + monalg + "_Turn_On_Curve_wrt" + wrtalg + bestr[iBarrelSA] + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		}
	      }

	      //
	      bool mf_active = false;
	      std::vector< Feature<xAOD::L2StandAloneMuonContainer> > mf = combsHLT[id_min_l2].get<xAOD::L2StandAloneMuonContainer>("MuonL2SAInfo",TrigDefs::alsoDeactivateTEs);
	      if( mf.size() == 1 ) {
		mf_active = mf[0].te()->getActiveState();
		ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*mf[0].te()) << " / " << mf_active); 
		if(m_access_hypoTE){
		  const HLT::TriggerElement *hypo = getDirectSuccessorHypoTEForL2(mf[0].te(), "L2MuonSA", chainName);
		  if(hypo){
		    mf_active = hypo->getActiveState();
		    ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*hypo) << " / " << mf_active); 
		  }
		}
	      }
	      if( mf_active ) {
		// float mf_pt = mf[0].cptr()->pt();
		const xAOD::L2StandAloneMuonContainer* mf_cont = mf[0];

		float mf_eta = mf_cont->at(0)->eta();
		float mf_phi = mf_cont->at(0)->phi();

		if (EF_lower_passed) {
		  name     = histcName + monalg + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		  // Barrel/Endcap
		  name     = histcName + monalg + bestr[iBarrelSA] + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		  // 2D coverage hist
		  name     = histcName + monalg + "_etaphi_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(mf_eta, mf_phi);
		  name     = histcName + monalg + "_etaphi_coarse_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(mf_eta, mf_phi);
		}

		// if (ES_lower_passed) 
		for (int i = 0; i <= m_maxESbr; i++) {
		  if(!m_MS_mon_ESbr[i])continue; 
		  if (m_passedES[i]) {
		    name     = histcName + m_triggerES[i] + monalg + "_Turn_On_Curve_Numerator";
		    ATH_MSG_DEBUG( name << " filling" );
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		}
	      }
	    }

	    // EF TrigMuonEF SA
	    float dr_min_ef = 9999;
	    int   id_min_ef = -1;
	    int   last_step = -1;
	    monalg = "_MuonEFMS";
	    wrtalg = "_L2MuonSA";
	    std::string monalg2 = "_MuonEFSA";


	    const HLT::Chain* chainDetail = m_ExpertMethods->getChainDetails( chainName);
	    const TrigConf::HLTChain* configChain = 0;
	    if (chainDetail) {
	      configChain = chainDetail->getConfigChain();
	      //lastStep = chainDetail->getChainStep();
	    }
	    std::vector<const HLT::TriggerElement*>::const_iterator p_te1;
	    std::vector<TrigConf::HLTTriggerElement*>::const_iterator p_te2;
	    std::vector<TrigConf::HLTSignature*>::const_iterator p_sig;
	    std::vector<TrigConf::HLTSignature*> dummylist;
	    std::vector<TrigConf::HLTSignature*>& siglist = dummylist;

	    if (configChain) siglist = configChain->signatureList();

	    for(int iEF=0; iEF<(int)combsHLT.size(); iEF++) {
	      std::vector< Feature<TrigRoiDescriptor> > initRois = combsHLT[iEF].get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
	      ATH_MSG_DEBUG("nr TrigRoiDescriptor is not 1 for chain=EF_" << chainName << " RoI # " << initRois.size());
	      if( initRois.size() != 1 ) {
		// ATH_MSG_WARNING("nr TrigRoiDescriptor is not 1 for chain=EF_" << chainName);
		ATH_MSG_DEBUG("nr TrigRoiDescriptor is not 1 for chain=EF_" << chainName);
		dr_min_ef = -1.0; // No RoI = Full scan trigger: fill efficiency nevertheless
		// retval = StatusCode::RECOVERABLE;
	      }
	      else {
		float dr = calc_dR(rec_eta,rec_phi,initRois[0].cptr()->eta(),initRois[0].cptr()->phi());
		name     = histcName + "_deltaR_btw_L1_MuidSA_For" + monalg;
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(dr);
		if( dr < dr_min_ef ) { 
		  dr_min_ef = dr;  
		  id_min_ef = iEF; 
		  last_step = 0;

		  //get last step 
		  //loop over TE
		  const std::vector<const HLT::TriggerElement*>& tes = combsHLT[iEF].tes();
		  for (p_te1=tes.begin(); p_te1!=tes.end(); ++p_te1) {
		    bool s = false;

		    //loop over signature list
		    for (p_sig=siglist.begin(); p_sig!=siglist.end(); ++p_sig) {
		      vector<TrigConf::HLTTriggerElement*>& tes2 = (*p_sig)->outputTEs();
		      for (p_te2=tes2.begin(); p_te2!=tes2.end(); ++p_te2) {
			if ( (*p_te1)->getId() == 
			    TrigConf::HLTUtils::string2hash( (*p_te2)->name(), "TE")) {
			  s = true;
			  break;
			}
		      }
		      if (s) {
			if ( (int) (*p_sig)->signature_counter() > last_step) 
			  last_step = (int) (*p_sig)->signature_counter();
			break;
		      }
		    }
		  }
		}
	      }
	    }

	    //    if( dr_min_ef < DR_CUT && (EF_lower_passed || ES_lower_passed)) 
	    if( dr_min_ef < DR_CUT && id_min_ef>=0 ) {  // 20.11.11: now it also means that the ROI is not found, which is indicated as dr being negative
	      // but this condition is not fulfilled if EF feature does not exist in this event
	      ATH_MSG_DEBUG("EF: dRmin=" << dr_min_ef); 

	      if (EF_lower_passed) {
		name     = histcName + monalg + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		name     = histcName + monalg2 + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		// Barrel/Endcap
		name     = histcName + monalg2 + "_Turn_On_Curve_wrt" + wrtalg + bestr[iBarrelSA] + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	      }
	      // if (ES_lower_passed) 
	      // ES trigger-aware
	      for (int i = 0; i <= m_maxESbr; i++) {
		if(!m_MS_mon_ESbr[i])continue; 
		if (m_passedES[i]) {
		  name     = histcName + m_triggerES[i] + monalg + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		  name     = histcName + m_triggerES[i] + monalg2 + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		}
	      }
	      //
	      float ef_ms_pt  = 0.;
	      float ef_ms_eta = 0.;
	      float ef_sa_pt  = 0.;
	      float ef_sa_eta = 0.;

	      // YY added:
	      float ef_ms_phi = 0.;
	      float ef_sa_phi = 0.;

	      // Check if EF feature matches to offline object for full-scan triggers
	      bool ef_ms_match = false;
	      bool ef_sa_match = false;
	      float ms_min_dltr = 9999.;
	      float sa_min_dltr = 9999.;
	      int id_min_msef = -1;
	      int id_min_saef = -1;
	      bool ef_active = false;

	      if (dr_min_ef >= 0.) {
		ef_ms_match = true; // YY for full-scan coding
		ef_sa_match = true; // YY for full-scan coding


		ef_active       = combsHLT[id_min_ef].active();
		ATH_MSG_DEBUG("...comb active=" << ef_active ); //i
		std::vector< Feature<xAOD::MuonContainer> > ef = combsHLT[id_min_ef].get<xAOD::MuonContainer>("MuonEFInfo",TrigDefs::alsoDeactivateTEs);    
		ATH_MSG_DEBUG("...ef size=" << ef.size() ); //i
		if( ef.size() == 1 ) {

		  ATH_MSG_DEBUG("...EFInfo: label/active=" << getTEName(*ef[0].te()) << " / " << ef[0].te()->getActiveState());

		  std::vector<Feature<xAOD::MuonContainer> >::const_iterator fIt = ef.begin();
		  for ( ; fIt != ef.end() ; ++fIt) {
		    const xAOD::MuonContainer* ef_cont = (*fIt);
		    if (ef_cont) {
		      ATH_MSG_DEBUG(" ef_contContainter size:"<<ef_cont->size()); 

		      xAOD::MuonContainer::const_iterator iCont = ef_cont->begin();
		      for(; iCont != ef_cont->end(); iCont++) {
			const xAOD::TrackParticle *ef_ms_trk;
			ef_ms_trk = (*iCont)->trackParticle(xAOD::Muon::TrackParticleType::MuonSpectrometerTrackParticle);

			ef_ms_pt  = (*iCont)->pt();
			ef_ms_eta = (*iCont)->eta();
			ef_ms_phi = (*iCont)->phi(); // YY
			if( ef_ms_trk ){
			  if( ef_ms_trk->pt() <= 0.){
			    ef_ms_pt  = -1.;
			    ef_ms_eta = 0.;
			    ef_ms_phi = 0.; // YY
			  }else if( ef_ms_pt < fabs(ef_ms_trk->pt()) / CLHEP::GeV ){
			    ef_ms_pt  = fabs(ef_ms_trk->pt()) / CLHEP::GeV;
			    ef_ms_eta = ef_ms_trk->eta();
			    ef_ms_phi = ef_ms_trk->phi();  // YY
			  }
			  ATH_MSG_DEBUG("           MS eta/pt=" << ef_ms_eta << " / " << ef_ms_pt/CLHEP::GeV ); 
			  ATH_MSG_DEBUG("MS track found for last_step " << last_step); //i
			}else{
			  ATH_MSG_DEBUG("No MS track found for last_step " << last_step); 
			}

			ef_sa_pt  = (*iCont)->pt();
			ef_sa_eta = (*iCont)->eta();
			ef_sa_phi = (*iCont)->phi(); // YY
			if( (*iCont)->muonType()!=xAOD::Muon::MuonStandAlone && (*iCont)->muonType()!=xAOD::Muon::Combined){
			  ef_sa_pt  = -1.;
			  ef_sa_eta = 0.;
			  ef_sa_phi = 0.; // YY
			}else if( ef_ms_trk){
			  if( ef_sa_pt < fabs(ef_ms_trk->pt()) / CLHEP::GeV ){
			    ef_sa_pt  = fabs(ef_ms_trk->pt()) / CLHEP::GeV;
			    ef_sa_eta = ef_ms_trk->eta();
			    ef_sa_phi = ef_ms_trk->phi(); // YY
			  }
			  ATH_MSG_DEBUG("SA track found for last_step " << last_step); //i
			}else{
			  ATH_MSG_DEBUG("No SA track found for last_step " << last_step); //i
			}
			ATH_MSG_DEBUG("           SA eta/pt=" << ef_sa_eta << " / " << ef_sa_pt/CLHEP::GeV ); 
		      }
		    }
		  }
		}
	      } else { // full-scan chain

		for(int iEF=0; iEF<(int)combsHLT.size(); iEF++) {
		  ATH_MSG_DEBUG("...comb active=" << ef_active ); //i
		  if (id_min_ef < 0) continue;
		  std::vector< Feature<xAOD::MuonContainer> > ef = combsHLT[id_min_ef].get<xAOD::MuonContainer>("MuonEFInfo",TrigDefs::alsoDeactivateTEs);    
		  ATH_MSG_DEBUG("...ef size=" << ef.size() ); //i
		  if( ef.size() == 1 ) {
		    float dltr;
		    ATH_MSG_DEBUG("...EFInfo: label/active=" << getTEName(*ef[0].te()) << " / " << ef[0].te()->getActiveState()); //i

		    std::vector<Feature<xAOD::MuonContainer> >::const_iterator fIt = ef.begin();
		    for ( ; fIt != ef.end() ; ++fIt) {
		      const xAOD::MuonContainer* ef_cont = (*fIt);
		      if (ef_cont) {
			ATH_MSG_DEBUG(" FS ef_contContainter size:"<<ef_cont->size()); 

			xAOD::MuonContainer::const_iterator iCont = ef_cont->begin();
			for(; iCont != ef_cont->end(); iCont++) {
			  const xAOD::TrackParticle *ef_ms_trk;
			  ef_ms_trk = (*iCont)->trackParticle(xAOD::Muon::TrackParticleType::MuonSpectrometerTrackParticle);

			  ef_ms_pt  = (*iCont)->pt();
			  ef_ms_eta = (*iCont)->eta();
			  ef_ms_phi = (*iCont)->phi(); // YY
			  if( ef_ms_trk ){
			    if( ef_ms_trk->pt() <= 0.){
			      ef_ms_pt  = -1.;
			      ef_ms_eta = 0.;
			      ef_ms_phi = 0.; // YY
			    }else if( ef_ms_pt < fabs(ef_ms_trk->pt()) / CLHEP::GeV ){
			      ef_ms_pt  = fabs(ef_ms_trk->pt()) / CLHEP::GeV;
			      float ef_ms_eta_tmp = ef_ms_trk->eta();
			      float ef_ms_phi_tmp = ef_ms_trk->phi();  // YY
			      dltr = CalculateDeltaR(rec_eta, rec_phi, ef_ms_eta_tmp, ef_ms_phi_tmp);
			      if (dltr < ms_min_dltr) {
				ms_min_dltr = dltr;
				id_min_msef = iEF;
				ef_ms_eta = ef_ms_eta_tmp;
				ef_ms_phi = ef_ms_phi_tmp;
			      }      
			    }
			    ATH_MSG_DEBUG("FS           MS eta/pt=" << ef_ms_eta << " / " << ef_ms_pt/CLHEP::GeV ); //i
			    ATH_MSG_DEBUG("MS track found for last_step " << last_step); //i
			  }else{
			    ATH_MSG_DEBUG("No MS track found for last_step " << last_step); //i
			  }

			  ef_sa_pt  = (*iCont)->pt();
			  ef_sa_eta = (*iCont)->eta();
			  ef_sa_phi = (*iCont)->phi(); // YY
			  if( (*iCont)->muonType()!=xAOD::Muon::MuonStandAlone && (*iCont)->muonType()!=xAOD::Muon::Combined){
			    ef_sa_pt  = -1.;
			    ef_sa_eta = 0.;
			    ef_sa_phi = 0.; // YY
			  }else if( ef_ms_trk){
			    if( ef_sa_pt < fabs(ef_ms_trk->pt()) / CLHEP::GeV ){
			      ef_sa_pt  = fabs(ef_ms_trk->pt()) / CLHEP::GeV;
			      float ef_sa_eta_tmp = ef_ms_trk->eta();
			      float ef_sa_phi_tmp = ef_ms_trk->phi(); // YY
			      dltr = CalculateDeltaR(rec_eta, rec_phi, ef_sa_eta_tmp, ef_sa_phi_tmp);
			      if (dltr < sa_min_dltr) {
				sa_min_dltr = dltr;
				id_min_saef = iEF;
				ef_sa_eta = ef_sa_eta_tmp;
				ef_sa_phi = ef_sa_phi_tmp;
			      }      
			    }
			    ATH_MSG_DEBUG("FS           SA eta/pt=" << ef_sa_eta << " / " << ef_sa_pt/CLHEP::GeV ); 
			    ATH_MSG_DEBUG("SA track found for last_step " << last_step); //i
			  }else{
			    ATH_MSG_DEBUG("No SA track found for last_step " << last_step); //i
			  }
			}
		      }
		    }
		  }
		} // loop over MSonly EF TEs
		if (ms_min_dltr < DR_CUT) {
		  ef_ms_match = true;
		  ATH_MSG_DEBUG("MS Delta R, flag " << ms_min_dltr << " " << id_min_msef); //i
		}
		if (sa_min_dltr < DR_CUT) {
		  ef_sa_match = true;
		  if (id_min_saef < 0) {
		    ATH_MSG_WARNING("inconsistency in SA matching");
		  } else {
		    ef_active = combsHLT[id_min_saef].active();
		    ATH_MSG_DEBUG("SA Delta R, flag " << sa_min_dltr << " " << id_min_saef); //i
		  }
		}
	      }

	      //MS pt/eta checks
	      if( ef_ms_match && ef_ms_pt > getPtThresTrigMuonEFSA(thresh, ef_ms_eta) ){ // matching required only for full-scan chains
		if (EF_lower_passed) {
		  ATH_MSG_DEBUG("MSonly MS passed last step " << last_step );
		  name     = histcName + monalg + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  // 2D coverage hist
		  name     = histcName + monalg + "_etaphi_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(ef_ms_eta, ef_ms_phi);
		  name     = histcName + monalg + "_etaphi_coarse_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(ef_ms_eta, ef_ms_phi);
		}
		// if (ES_lower_passed) 
		// ES trigger-aware
		for (int i = 0; i <= m_maxESbr; i++) {
		  if(!m_MS_mon_ESbr[i])continue; 
		  if (m_passedES[i]) {
		    ATH_MSG_DEBUG("MSonly MS passed last step " << last_step );
		    name     = histcName + m_triggerES[i] + monalg + "_Turn_On_Curve_Numerator";
		    ATH_MSG_DEBUG( name << " filling" );
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		}
	      }

	      //SA pt/eta checks
	      //if( ef_ms_pt > getPtThresTrigMuonEFSA(thresh, ef_ms_eta) &&
	      //  ef_sa_pt > getPtThresTrigMuonEFSA(thresh, ef_sa_eta) )
	      if( ef_sa_match && ef_active ) { // matching required only for full-scan triggers
		if (EF_lower_passed) {
		  ATH_MSG_DEBUG("MSonly last step " << last_step );
		  name     = histcName + monalg2 + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		  // Barrel/Endcap
		  name     = histcName + monalg2 + bestr[iBarrelSA] + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		  // 2D coverage hist
		  name     = histcName + monalg2 + "_etaphi_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(ef_sa_eta, ef_sa_phi);
		  name     = histcName + monalg2 + "_etaphi_coarse_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(ef_sa_eta, ef_sa_phi);
		}
		// if (ES_lower_passed) 
		// ES trigger-aware
		for (int i = 0; i <= m_maxESbr; i++) {
		  if(!m_MS_mon_ESbr[i])continue; 
		  if (m_passedES[i]) {
		    ATH_MSG_DEBUG("MSonly last step " << last_step );
		    name     = histcName + m_triggerES[i] + monalg2 + "_Turn_On_Curve_Numerator";
		    ATH_MSG_DEBUG( name << " filling" );
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		}     
	      }else{
		ATH_MSG_DEBUG("MSonly not active last step " << last_step );
	      }
	    }

	  } // end of SA loop

	  //

	  return retval;
	}

	StatusCode HLTMuonMonTool::fillChainDQA_standard(const std::string& chainName, const std::string & histcName)
	{
	  ATH_MSG_DEBUG("----- fillChainDQA_standard: chain=" << chainName << " histname=" <<histcName<< " -----"); 

	  using namespace Trig;

	  const float DR_CUT = 0.2; // 2012

	  //*****************************************//
	  // add by Yuan //
	  std::string bestr[2] = {"_Barrel", "_Endcap"};
	  std::vector<std::pair<float, int> > RecMuonCB_pt_index;
	  if(RecMuonCB_pt_index.size()!=0)RecMuonCB_pt_index.clear();

	  StatusCode sc;
	  StoreGateSvc* p_detStore;
	  std::string tag;

	  //Set pointer on DetectorStore
	  sc = service("DetectorStore", p_detStore);
	  if ( sc.isFailure() ) {
	    ATH_MSG_FATAL( "DetectorStore service not found !" ) ;
	    return StatusCode::FAILURE;
	  }
	  ATH_MSG_DEBUG( "Found DetectorStore ") ;

	  const TagInfo* tagInfo = 0;
	  sc = p_detStore->retrieve( tagInfo );
	  if (sc.isFailure()) {
	    ATH_MSG_WARNING("Could not get TagInfo");
	    return StatusCode::RECOVERABLE;
	  } else {
	    tagInfo->findTag("triggerStreamOfFile",tag);
	  }

	  if(chainName.find("noL1")!= string::npos){
	    if(m_RecMuonCB_pt.size()<=1) return StatusCode::SUCCESS;
	    m_esvect.clear();
	    m_esvect = getESbits();
	    std::vector<std::string>::iterator it;
	    for (it = m_esvect.begin(); it != m_esvect.end(); it++) ATH_MSG_DEBUG("----- express stream trigger :" << *it << " -----"); 

	    for(int i_rec=0; i_rec<(int)m_RecMuonCB_pt.size(); i_rec++) {  // start the loop on muon
	      if (!m_RecMuonCB_isGoodCB[i_rec]) {
		ATH_MSG_DEBUG("HLTMuonMon: fillChainDQA_Standard: not a good combined muon" << i_rec);
		continue;
	      }
	      float rec_pt  = m_RecMuonCB_pt[i_rec];
	      RecMuonCB_pt_index.push_back(std::make_pair(rec_pt,i_rec));
	    }  // end the loop on muon
	    if(RecMuonCB_pt_index.size()<=1) return StatusCode::SUCCESS;

	    std::sort(RecMuonCB_pt_index.begin(),RecMuonCB_pt_index.end(),my_sort<std::pair<float, int> >());  // sorted the muon pt
	    if(!m_HI_pp_mode){
	      if(RecMuonCB_pt_index[0].first < 4 || RecMuonCB_pt_index[1].first < 1) return StatusCode::SUCCESS;
	    }else{
	      if(RecMuonCB_pt_index[0].first < 18 || RecMuonCB_pt_index[1].first < 3) return StatusCode::SUCCESS;
	    }
	    int mu1_index = RecMuonCB_pt_index[0].second;
	    int mu2_index = RecMuonCB_pt_index[1].second;

	    std::string EF_pre_trigger= m_FS_pre_trigger;   
	    std::string EF_pre_trigger_second= m_FS_pre_trigger_second;   

	    if( tag == "express" && !m_passedES[ESSTD]) return StatusCode::SUCCESS; 
	    if(getTDT()->isPassed(EF_pre_trigger.c_str())!=1 && getTDT()->isPassed(EF_pre_trigger_second.c_str())!=1) return StatusCode::SUCCESS;

	    std::string name = histcName + "_Turn_On_Curve_wrt_subleading_MuidCB" + "_Denominator";
	    hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[mu2_index]);
	    std::string EF_FS_trigger = chainName;
	    if(getTDT()->isPassed(EF_FS_trigger.c_str())){
	      name =  histcName + "_Turn_On_Curve_wrt_subleading_MuidCB" + "_Numerator";
	      hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[mu2_index]);
	    }

	    int match_L2_RoI[2];
	    int match_L2_RoI_activate[2];
	    int match_EF_RoI[2];
	    int match_EF_RoI_activate[2];
	    for(int i = 0; i<2; i++){
	      match_L2_RoI[i] = 0;
	      match_L2_RoI_activate[i] = 0;
	      match_EF_RoI[i] = 0;
	      match_EF_RoI_activate[i] = 0;
	    }


	    //*************  check the first pre trigger mu18it_tight *******************//
	    FeatureContainer my_fHLT = getTDT()->features(m_FS_pre_trigger,TrigDefs::alsoDeactivateTEs);

	    std::vector<Combination> my_combsHLT = my_fHLT.getCombinations();

	    // to match with the initial ROI at L2
	    for(int n_mu = 0; n_mu <2;n_mu ++){
	      int mu_index;
	      if(n_mu == 0) mu_index = mu1_index;
	      else if(n_mu == 1) mu_index = mu2_index;
	      float my_dr_min_l2 = 9999;
	      int   my_id_min_l2 = -1;
	      for(int iL2=0; iL2<(int)my_combsHLT.size(); iL2++) {
		std::vector< Feature<TrigRoiDescriptor> > initRois = my_combsHLT[iL2].get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
		if( initRois.size() != 1 ) {
		  ATH_MSG_WARNING("nr TrigRoiDescriptor is not 1 for chain=L2_" << chainName);
		}
		else {
		  float dr = calc_dR(m_RecMuonCB_eta[mu_index],m_RecMuonCB_phi[mu_index],initRois[0].cptr()->eta(),initRois[0].cptr()->phi());
		  if( dr < my_dr_min_l2 ) {
		    my_dr_min_l2 = dr;
		    my_id_min_l2 = iL2;
		  }
		}
	      }

	      if( my_dr_min_l2 < DR_CUT && my_id_min_l2 != -1) {

		match_L2_RoI[n_mu] = 1;
		// check the muon fire mu24_tight ROI or not   L2MuonSA
		bool mf_active = false;
		std::vector< Feature<xAOD::L2StandAloneMuonContainer> > mf = my_combsHLT[my_id_min_l2].get<xAOD::L2StandAloneMuonContainer>("MuonL2SAInfo",TrigDefs::alsoDeactivateTEs);
		if( mf.size() == 1 ) {
		  mf_active = mf[0].te()->getActiveState();
		  ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*mf[0].te()) << " / " << mf_active);
		  if(m_access_hypoTE){
		    const HLT::TriggerElement *hypo = getDirectSuccessorHypoTEForL2(mf[0].te(), "L2MuonSA", chainName);
		    if(hypo){
		      mf_active = hypo->getActiveState();
		      ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*hypo) << " / " << mf_active);
		    }
		  }
		}
		//if(!mf_active)return StatusCode::SUCCESS;

		bool combMf_active = false;
		std::vector< Feature<xAOD::L2CombinedMuonContainer> > combMf = my_combsHLT[my_id_min_l2].get<xAOD::L2CombinedMuonContainer>("MuonL2CBInfo",TrigDefs::alsoDeactivateTEs);
		if( combMf.size() == 1 ) {
		  combMf_active = combMf[0].te()->getActiveState();
		  ATH_MSG_DEBUG("...combMF: label/active=" << getTEName(*combMf[0].te()) << " / " << combMf_active);
		  if(m_access_hypoTE){
		    const HLT::TriggerElement *hypo = getDirectSuccessorHypoTEForL2(mf[0].te(), "L2muComb", chainName);
		    if(hypo){
		      combMf_active = hypo->getActiveState();
		      ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*hypo) << " / " << combMf_active);
		    }
		  }
		}
		//if(!combMf_active)return StatusCode::SUCCESS;
		if(mf_active && combMf_active)match_L2_RoI_activate[n_mu] = 1;


	      }

	      float my_dr_min_ef = 9999;
	      int   my_id_min_ef = -1;
	      // to match with the initial ROI at EF 

	      for(int iEF=0; iEF<(int)my_combsHLT.size(); iEF++) {
		std::vector< Feature<TrigRoiDescriptor> > initRois = my_combsHLT[iEF].get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
		if( initRois.size() != 1 ) {
		  ATH_MSG_WARNING("nr TrigRoiDescriptor is not 1 for chain=EF_" << chainName);
		}
		else {
		  float dr = calc_dR(m_RecMuonCB_eta[mu_index],m_RecMuonCB_phi[mu_index],initRois[0].cptr()->eta(),initRois[0].cptr()->phi());
		  if( dr < my_dr_min_ef ) {
		    my_dr_min_ef = dr;
		    my_id_min_ef = iEF;
		  }
		}
	      }
	      if(my_dr_min_ef < DR_CUT){
		match_EF_RoI[n_mu] = 1;
		bool ef_active       = (my_combsHLT[my_id_min_ef]).active();
		//if(!ef_active) return StatusCode::SUCCESS;
		if(ef_active) match_EF_RoI_activate[n_mu] = 1;
	      }

	    }

	    //*************  check the second pre trigger mu24i_tight *******************//
	    FeatureContainer my_fHLT_mu24_imedium = getTDT()->features(m_FS_pre_trigger_second,TrigDefs::alsoDeactivateTEs);

	    std::vector<Combination> my_combsHLT_mu24_imedium= my_fHLT_mu24_imedium.getCombinations();

	    // to match with the initial ROI at L2
	    for(int n_mu = 0; n_mu <2;n_mu ++){
	      int mu_index;
	      if(n_mu == 0) mu_index = mu1_index;
	      else if(n_mu == 1) mu_index = mu2_index;
	      float my_dr_min_l2 = 9999;
	      int   my_id_min_l2 = -1;
	      for(int iL2=0; iL2<(int)my_combsHLT_mu24_imedium.size(); iL2++) {
		std::vector< Feature<TrigRoiDescriptor> > initRois = my_combsHLT_mu24_imedium[iL2].get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
		if( initRois.size() != 1 ) {
		  ATH_MSG_WARNING("nr TrigRoiDescriptor is not 1 for chain=L2_" << chainName);
		}
		else {
		  float dr = calc_dR(m_RecMuonCB_eta[mu_index],m_RecMuonCB_phi[mu_index],initRois[0].cptr()->eta(),initRois[0].cptr()->phi());
		  if( dr < my_dr_min_l2 ) {
		    my_dr_min_l2 = dr;
		    my_id_min_l2 = iL2;
		  }
		}
	      }

	      if( my_dr_min_l2 < DR_CUT && my_id_min_l2 != -1 ) {

		match_L2_RoI[n_mu] = 1;
		// check the muon fire mu24_tight ROI or not   L2MuonSA
		bool mf_active = false;
		std::vector< Feature<xAOD::L2StandAloneMuonContainer> > mf = my_combsHLT_mu24_imedium[my_id_min_l2].get<xAOD::L2StandAloneMuonContainer>("MuonL2SAInfo",TrigDefs::alsoDeactivateTEs);
		if( mf.size() == 1 ) {
		  mf_active = mf[0].te()->getActiveState();
		  ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*mf[0].te()) << " / " << mf_active);
		  if(m_access_hypoTE){
		    const HLT::TriggerElement *hypo = getDirectSuccessorHypoTEForL2(mf[0].te(), "L2MuonSA", chainName);
		    if(hypo){
		      mf_active = hypo->getActiveState();
		      ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*hypo) << " / " << mf_active);
		    }
		  }
		}
		//if(!mf_active)return StatusCode::SUCCESS;
		
		bool combMf_active = false;
		std::vector< Feature<xAOD::L2CombinedMuonContainer> > combMf = my_combsHLT_mu24_imedium[my_id_min_l2].get<xAOD::L2CombinedMuonContainer>("MuonL2CBInfo",TrigDefs::alsoDeactivateTEs);
		if( combMf.size() == 1 ) {
		  combMf_active = combMf[0].te()->getActiveState();
		  ATH_MSG_DEBUG("...combMF: label/active=" << getTEName(*combMf[0].te()) << " / " << combMf_active);
		  if(m_access_hypoTE){
		    const HLT::TriggerElement *hypo = getDirectSuccessorHypoTEForL2(combMf[0].te(), "L2muComb", chainName);
		    if(hypo){
		      combMf_active = hypo->getActiveState();
		      ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*hypo) << " / " << combMf_active);
		    }
		  }
		}
		//if(!combMf_active)return StatusCode::SUCCESS;
		if(mf_active && combMf_active)match_L2_RoI_activate[n_mu] = 1;


	      }

	      float my_dr_min_ef = 9999;
	      int   my_id_min_ef = -1;
	      // to match with the initial ROI at EF 

	      for(int iEF=0; iEF<(int)my_combsHLT_mu24_imedium.size(); iEF++) {
		std::vector< Feature<TrigRoiDescriptor> > initRois = my_combsHLT_mu24_imedium[iEF].get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
		if( initRois.size() != 1 ) {
		  ATH_MSG_WARNING("nr TrigRoiDescriptor is not 1 for chain=EF_" << chainName);
		}
		else {
		  float dr = calc_dR(m_RecMuonCB_eta[mu_index],m_RecMuonCB_phi[mu_index],initRois[0].cptr()->eta(),initRois[0].cptr()->phi());
		  if( dr < my_dr_min_ef ) {
		    my_dr_min_ef = dr;
		    my_id_min_ef = iEF;
		  }
		}
	      }
	      if(my_dr_min_ef < DR_CUT && my_id_min_ef >= 0){
		match_EF_RoI[n_mu] = 1;
		bool ef_active       = (my_combsHLT_mu24_imedium[my_id_min_ef]).active();
		//if(!ef_active) return StatusCode::SUCCESS;
		if(ef_active) match_EF_RoI_activate[n_mu] = 1;
	      }

	    }


	    // now make a random number for tag muon //
	    int tag_muon_index = -1;
	    int probe_muon_index = -1;
	    bool probe_muon_pass = false;

	    int matched_tag_muon_index = -1;
	    int matched_probe_muon_index = -1;
	    bool matched_probe_muon_pass = false;

	    m_random3.SetSeed(m_event);
	    double random_number = m_random3.Gaus(0,1);
	    ATH_MSG_DEBUG(" the random number: "<<random_number);
	    if(random_number > 0) {   /// if the random number greater than 0,  check the leading muon first
	      if(match_L2_RoI_activate[0] && match_EF_RoI_activate[0]){   // tag leading muon
		tag_muon_index = mu1_index;
		probe_muon_index = mu2_index;
		probe_muon_pass = (match_L2_RoI_activate[1] && match_EF_RoI_activate[1]);

		if(match_L2_RoI[1] && match_EF_RoI[1]) {
		  matched_tag_muon_index = mu1_index;
		  matched_probe_muon_index = mu2_index;
		  matched_probe_muon_pass = (match_L2_RoI_activate[1] && match_EF_RoI_activate[1]);
		}
	      }else if(match_L2_RoI_activate[1] && match_EF_RoI_activate[1]){  // tag subleading muon
		tag_muon_index = mu2_index;
		probe_muon_index = mu1_index;
		probe_muon_pass = (match_L2_RoI_activate[0] && match_EF_RoI_activate[0]);

		if(match_L2_RoI[0] && match_EF_RoI[0]) {
		  matched_tag_muon_index = mu2_index;
		  matched_probe_muon_index = mu1_index;
		  matched_probe_muon_pass = (match_L2_RoI_activate[0] && match_EF_RoI_activate[0]);
		}
	      }
	    }else {   /// else if the random number smaller than 0,  check the subleading muon first
	      if(match_L2_RoI_activate[1] && match_EF_RoI_activate[1]){   // tag subleading muon
		tag_muon_index = mu2_index;
		probe_muon_index = mu1_index;
		probe_muon_pass = (match_L2_RoI_activate[0] && match_EF_RoI_activate[0]);

		if(match_L2_RoI[0] && match_EF_RoI[0]) {
		  matched_tag_muon_index = mu2_index;
		  matched_probe_muon_index = mu1_index;
		  matched_probe_muon_pass = (match_L2_RoI_activate[0] && match_EF_RoI_activate[0]);
		}
	      } else if(match_L2_RoI_activate[0] && match_EF_RoI_activate[0]){   // tag leading muon
		tag_muon_index = mu1_index;
		probe_muon_index = mu2_index;
		probe_muon_pass = (match_L2_RoI_activate[1] && match_EF_RoI_activate[1]);

		if(match_L2_RoI[1] && match_EF_RoI[1]) {
		  matched_tag_muon_index = mu1_index;
		  matched_probe_muon_index = mu2_index;
		  matched_probe_muon_pass = (match_L2_RoI_activate[1] && match_EF_RoI_activate[1]);
		}

	      }
	    }


	    const EventInfo* evt;
	    int event, run;
	    if( StatusCode::SUCCESS == evtStore()->retrieve(evt)){
	      event = evt->event_ID()->event_number();
	      run = evt->event_ID()->run_number();
	      ATH_MSG_DEBUG (" EventInfo : "   << " event: " << event << " run: " << run);

	    }else{
	      ATH_MSG_WARNING (" Unable to retrieve EventInfo from StoreGate ");
	      return StatusCode::SUCCESS;

	    }

	    float mean_mu = evt->averageInteractionsPerCrossing();

	    // start to dump the probe muon information //
	    if(tag_muon_index > -1){
	      ATH_MSG_DEBUG(" ####### found tag muon ######### ");
	      // index for Barrel/Endcap
	      int probe_iBarrelCB = (fabs(m_RecMuonCB_eta[probe_muon_index]) < 1.05) ? 0 : 1;

	      // for the Full Scan efficiency //
	      std::string name = histcName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
	      hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);

	      name = histcName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + bestr[probe_iBarrelCB] + "_Denominator";
	      hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);

	      if(mean_mu <= 15){
		name = histcName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
		hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);
	      } else if(mean_mu <= 20){
		name = histcName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
		hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);
	      } else {
		name = histcName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";
		hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);
	      } 


	      if(getTDT()->isPassed(EF_FS_trigger.c_str())){
		name = histcName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";
		hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);

		name = histcName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + bestr[probe_iBarrelCB] + "_Numerator";
		hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);

		if(mean_mu <= 15){
		  name = histcName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";
		  hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);
		} else if(mean_mu <= 20){
		  name = histcName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";
		  hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);
		} else {
		  name = histcName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";
		  hist(name, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);
		} 

	      }

	      // for the preselection trigger efficiency //
	      std::string denom = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB_Denominator";
	      hist(denom, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);
	      std::string nomin = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB_Numerator";
	      if(probe_muon_pass) {
		hist(nomin, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[probe_muon_index]);
		// swap the tag and probe muons //
		hist(denom, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[tag_muon_index]);
		hist(nomin, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[tag_muon_index]);
	      }

	      if(matched_tag_muon_index > -1){
		denom = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB_Denominator";
		hist(denom, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[matched_probe_muon_index]);
		nomin = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB_Numerator";
		if(matched_probe_muon_pass){
		  hist(nomin, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[matched_probe_muon_index]);
		  // swap the tag and probe muons //
		  hist(denom, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[matched_tag_muon_index]);
		  hist(nomin, m_histdireffnumdenom)->Fill(m_RecMuonCB_pt[matched_tag_muon_index]);

		}
	      }
	    }


	  }
	  //*****************************************//
	  return StatusCode::SUCCESS;

	}


	StatusCode HLTMuonMonTool::fillChainDQA_generic(const std::string& chainName, const std::string& HistchainName,  bool isIsolOffline)
	{
	  ATH_MSG_DEBUG("----- fillChainDQA_generic: chain=" << chainName << " histChain:  "<<HistchainName);

	  using namespace Trig;

	  bool EF_lower_passed = true;
	  bool MS_lower_passed = false;
	  std::vector<std::string>::iterator it;
	  for (it = m_MSchain.begin(); it != m_MSchain.end(); it++) {
	    if (getTDT()->isPassed(*it)) {
	      MS_lower_passed = true;
	    }
	  }

	  StatusCode retval = StatusCode::SUCCESS;
	  // YY: 25.05.10 to clear m_checked:
	  retval.getCode();


	  FeatureContainer fHLT = getTDT()->features(chainName,TrigDefs::alsoDeactivateTEs);  

	  std::vector<Combination> combsHLT = fHLT.getCombinations();
	  ATH_MSG_DEBUG("nr combsHLT=" << combsHLT.size());  
	  for(std::vector<Combination>::const_iterator it=combsHLT.begin(); it!=combsHLT.end(); it++) {
	    ATH_MSG_DEBUG("..." << *it << " active:" << it->active());  
	  }


	  const float DR_CUT = 0.2; // YY 2012 setting

	  std::string name;
	  std::string bestr[2] = {"_Barrel", "_Endcap"};

	  // loop on each CB probe

	  //float isolThresh = (isIsolOffline ? 0.1 : 0.5);
	  //float isolThresh =  0.5;  //looser isolation in run2
	  float isolThresh = (isIsolOffline ? 0.06 : 0.5);  // tighter isolation in run2

	  for(int i_rec=0; i_rec<(int)m_RecMuonCB_pt.size(); i_rec++) {

	    if (!m_RecMuonCB_isGoodCB[i_rec]) {  
	      ATH_MSG_DEBUG("HLTMuonMon: fillChainDQA_Standard: not a good combined muon" << i_rec);
	      continue;  // for LS1, remove cuts on Hits and impact parameter, uncomment for collision.
	    }

	    float rec_eta = m_RecMuonCB_eta[i_rec];
	    float rec_phi = m_RecMuonCB_phi[i_rec];
	    float rec_pt  = m_RecMuonCB_pt[i_rec];

	    float rec_ptcone30 = m_RecMuonCB_ptcone30[i_rec];

	    //    if ((rec_ptcone40*0.001) / rec_pt > 0.5) continue; // tomoe added 09/06/2011
	    // if ((rec_ptcone40*0.001) / rec_pt > isolThresh) continue; // tomoe added 09/06/2011
	    if ((rec_ptcone30*0.001) / rec_pt > isolThresh) continue; // use cone30 in run2 

	    ATH_MSG_DEBUG("++ i_rec=" << i_rec);
	    //ATH_MSG_INFO("rec: eta/phi/pt=" << rec_eta << " / " << rec_phi << " / " << rec_pt);
	    ATH_MSG_DEBUG("rec: eta/phi/pt=" << rec_eta << " / " << rec_phi << " / " << rec_pt);
	    // if( rec_pt > 50. ) rec_pt = 50.;
	    // if( rec_pt > 100. ) rec_pt = 100.;

	    // index for Barrel/Endcap
	    int iBarrelCB = (fabs(rec_eta) < 1.05) ? 0 : 1;

	    if (EF_lower_passed) {
	      name     = HistchainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	      ATH_MSG_DEBUG( name << " filling" );
	      hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	      if (MS_lower_passed) {
		name     = HistchainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	      }

	      // Barrel/Endcap (YY 20.05.10) Denominator for "wrtOffline"
	      name     = HistchainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[iBarrelCB] + "_Denominator";
	      ATH_MSG_DEBUG( name << " filling" );
	      hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	      if (MS_lower_passed) {
		name     = HistchainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[iBarrelCB] + "_Denominator";
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	      }
	    }

	    // if (ES_lower_passed) {}
	    // ES trigger-aware
	    for (int i = 0; i <= m_maxESbr; i++) {
		name     = HistchainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	      if(!m_CB_mon_ESbr[i])continue; 
	      if (m_passedES[i]) {
		name     = HistchainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		// New Barrel/endcap for ES L1
		name     = HistchainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB" + bestr[iBarrelCB] + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
	      }
	    }

	    // L2 L2MuonSA and muComb
	    float dr_min_l2 = 9999;
	    int   id_min_l2 = -1;
	    std::string monalg  = "_L2MuonSA";
	    std::string monalg2 = "_MuComb";
	    std::string wrtalg = "_L1";
	    std::string wrtalg2 = "_L2MuonSA";

	    for(int iL2=0; iL2<(int)combsHLT.size(); iL2++) {
	      //std::vector< Feature<TrigRoiDescriptor> > initRois = fHLT.get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
	      std::vector< Feature<TrigRoiDescriptor> > initRois = combsHLT[iL2].get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
	      ATH_MSG_DEBUG("TrigRoiDescriptorCollection size: "<<initRois.size());
	      if( initRois.size() != 1 ) {
		ATH_MSG_WARNING("nr TrigRoiDescriptor is not 1 for chain=L2_" << chainName);
		retval = StatusCode::RECOVERABLE;
	      }
	      else {
		float dr = calc_dR(rec_eta,rec_phi,initRois[0].cptr()->eta(),initRois[0].cptr()->phi());
		name     = HistchainName + "_deltaR_btw_L1_MuidCB_For" + monalg;
		ATH_MSG_DEBUG( name << " filling" ); 
		hist(name, m_histdireffnumdenom)->Fill(dr);
		if( dr < dr_min_l2 ) {
		  dr_min_l2 = dr; 
		  id_min_l2 = iL2;
		}
	      }
	    }
	    if( dr_min_l2 < DR_CUT && id_min_l2 != -1) {
	      if (EF_lower_passed) {
		ATH_MSG_DEBUG("L2: dRmin=" << dr_min_l2);
		name     = HistchainName + monalg + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		if (MS_lower_passed) {
		  name     = HistchainName + m_MSchainName + monalg + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		}
		// Barrel/Endcap
		name     = HistchainName + monalg + "_Turn_On_Curve_wrt" + wrtalg + bestr[iBarrelCB] + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		if (MS_lower_passed) {
		  name     = HistchainName + m_MSchainName + monalg + "_Turn_On_Curve_wrt" + wrtalg + bestr[iBarrelCB] + "_Denominator";
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		}
	      }
	      // if (ES_lower_passed) {}
	      // ES trigger-aware
	      for (int i = 0; i <= m_maxESbr; i++) {
		if(!m_CB_mon_ESbr[i])continue; 
		ATH_MSG_DEBUG("L2: dRmin=" << dr_min_l2);
		if (m_passedES[i]) {
		  name     = HistchainName + m_triggerES[i] + monalg + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  // Barrel/Endcap for L1  : monalg = L2MuonSA, wrtalg = L1
		  name     = HistchainName + m_triggerES[i] + monalg + "_Turn_On_Curve_wrt" + wrtalg + bestr[iBarrelCB] + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		}
	      }

	      // === L2MuonSA ===
	      bool mf_active = false;
	      std::vector< Feature<xAOD::L2StandAloneMuonContainer> > mf = combsHLT[id_min_l2].get<xAOD::L2StandAloneMuonContainer>("MuonL2SAInfo",TrigDefs::alsoDeactivateTEs);

	      ATH_MSG_DEBUG(" L2MuonSA container size: "<< mf.size());  
	      if( mf.size() == 1 ) {
		mf_active = mf[0].te()->getActiveState();
		ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*mf[0].te()) << " / " << mf_active); 
		if(m_access_hypoTE){
		  const HLT::TriggerElement *hypo = getDirectSuccessorHypoTEForL2( mf[0].te(), "L2MuonSA", chainName);
		  if(hypo){
		    mf_active = hypo->getActiveState();
		    ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*hypo) << " / " << mf_active); 
		  }
		}
	      }


	      if( mf_active ) {
		const xAOD::L2StandAloneMuonContainer* mf_cont = mf[0];

		float mf_pt  = mf_cont->at(0)->pt();
		float mf_eta = mf_cont->at(0)->eta();
		float mf_phi = mf_cont->at(0)->phi();

		if (EF_lower_passed) { // if EF_lower_passed is always true, it is a catch-all branch.
		  name     = HistchainName + monalg + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  if (MS_lower_passed ) {
		    name     = HistchainName + m_MSchainName + monalg + "_Turn_On_Curve_Numerator";
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }

		  // Barrel/Endcap
		  name     = HistchainName + monalg + bestr[iBarrelCB] + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  if (MS_lower_passed) {
		    name     = HistchainName + m_MSchainName + monalg + bestr[iBarrelCB] + "_Turn_On_Curve_Numerator";
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }

		  // 2D coverage hist
		  ATH_MSG_DEBUG ( "Offline/L2MuonSA pt" << rec_pt << ", " << mf_pt );
		  ATH_MSG_DEBUG ( "Offline/L2MuonSA eta" << rec_eta << ", " << mf_eta );
		  ATH_MSG_DEBUG ( "Offline/L2MuonSA phi" << rec_phi << ", " << mf_phi );
		  name     = HistchainName + monalg + "_etaphi_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(mf_eta, mf_phi);
		  name     = HistchainName + monalg + "_etaphi_coarse_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(mf_eta, mf_phi);
		  // 1D covarage: L2MuonSA only
		  name     = HistchainName + monalg + "_eta_wrt_MuidCB";
		  hist(name, m_histdirdist2d)->Fill(mf_eta);
		  name     = HistchainName + monalg + "_phi_wrt_MuidCB";
		  hist(name, m_histdirdist2d)->Fill(mf_phi);
		}

		// ES trigger-aware
		for (int i = 0; i <= m_maxESbr; i++) {
		  if(!m_CB_mon_ESbr[i])continue; 
		  if (m_passedES[i]) {
		    name     = HistchainName + m_triggerES[i] + monalg + "_Turn_On_Curve_Numerator";
		    ATH_MSG_DEBUG( name << " filling" );
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		}
	      }

	      // === MuComb ===
	      bool combMf_active = false;
	      if( mf_active ) {
		if (EF_lower_passed) {
		  name     = HistchainName + monalg2 + "_Turn_On_Curve_wrt" + wrtalg2 + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  if (MS_lower_passed) {
		    name     = HistchainName + m_MSchainName + monalg2 + "_Turn_On_Curve_wrt" + wrtalg2 + "_Denominator";
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		  // Barrel/Endcap
		  name     = HistchainName + monalg2 + "_Turn_On_Curve_wrt" + wrtalg2 + bestr[iBarrelCB] + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  if (MS_lower_passed) {
		    name     = HistchainName + m_MSchainName + monalg2 + "_Turn_On_Curve_wrt" + wrtalg2 + bestr[iBarrelCB] + "_Denominator";
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		}
		// if (ES_lower_passed) {}
		// ES trigger-aware
		for (int i = 0; i <= m_maxESbr; i++) {
		  if(!m_CB_mon_ESbr[i])continue; 
		  if (m_passedES[i]) {
		    name     = HistchainName + m_triggerES[i] + monalg2 + "_Turn_On_Curve_wrt" + wrtalg2 + "_Denominator";
		    ATH_MSG_DEBUG( name << " filling" );
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		}

		//

		std::vector< Feature<xAOD::L2CombinedMuonContainer> > combMf = combsHLT[id_min_l2].get<xAOD::L2CombinedMuonContainer>("MuonL2CBInfo",TrigDefs::alsoDeactivateTEs);

		if( combMf.size() == 1 ) {
		  combMf_active = combMf[0].te()->getActiveState();
		  ATH_MSG_DEBUG("...combMF: label/active=" << getTEName(*combMf[0].te()) << " / " << combMf_active); 
		  if(m_access_hypoTE){
		    const HLT::TriggerElement *hypo = getDirectSuccessorHypoTEForL2( combMf[0].te(), "L2muComb", chainName);
		    if(hypo){
		      combMf_active = hypo->getActiveState();
		      ATH_MSG_DEBUG("...mF: label/active=" << getTEName(*hypo) << " / " << combMf_active);
		    }
		  }
		}


		if( combMf_active ) {
		  const xAOD::L2CombinedMuonContainer* combMf_cont = combMf[0];

		  float combMf_pt  = combMf_cont->at(0)->pt();
		  float combMf_eta = combMf_cont->at(0)->eta();
		  float combMf_phi = combMf_cont->at(0)->phi();



		  if (EF_lower_passed) {
		    name     = HistchainName + monalg2 + "_Turn_On_Curve_Numerator";
		    ATH_MSG_DEBUG( name << " filling" );
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		    if (MS_lower_passed) {
		      name     = HistchainName + m_MSchainName + monalg2 + "_Turn_On_Curve_Numerator";
		      hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		    }

		    // Barrel/Endcap
		    name     = HistchainName + monalg2 + bestr[iBarrelCB] + "_Turn_On_Curve_Numerator";
		    ATH_MSG_DEBUG( name << " filling" );
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		    if (MS_lower_passed) {
		      name     = HistchainName + m_MSchainName + monalg2 + bestr[iBarrelCB] + "_Turn_On_Curve_Numerator";
		      hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		    }

		    // 2D coverage hist
		    ATH_MSG_DEBUG ( "Offline/muComb pt" << rec_pt << ", " << combMf_pt );
		    ATH_MSG_DEBUG ( "Offline/muComb eta" << rec_eta << ", " << combMf_eta );
		    ATH_MSG_DEBUG ( "Offline/muComb phi" << rec_phi << ", " << combMf_phi );
		    name     = HistchainName + monalg2 + "_etaphi_wrt_MuidCB";
		    hist2(name, m_histdirdist2d)->Fill(combMf_eta, combMf_phi);
		    name     = HistchainName + monalg2 + "_etaphi_coarse_wrt_MuidCB";
		    hist2(name, m_histdirdist2d)->Fill(combMf_eta, combMf_phi);
		  }
		  // ES trigger-aware
		  for (int i = 0; i <= m_maxESbr; i++) {
		    if(!m_CB_mon_ESbr[i])continue; 
		    if (m_passedES[i]) {
		      name     = HistchainName + m_triggerES[i] + monalg2 + "_Turn_On_Curve_Numerator";
		      ATH_MSG_DEBUG( name << " filling" );
		      hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		    }
		  }     
		}
	      }
	    }

	    // == an EF chain ==
	    float dr_min_ef = 9999;
	    int   id_min_ef = -1;
	    int   last_step = -1;
	    wrtalg = "_MuComb";
	    std::string monalg3 = "_EFmuon";

	    //const HLT::Chain* chainDetail = m_ExpertMethods->getChainDetails("EF_" + chainName); // sept25
	    const HLT::Chain* chainDetail = m_ExpertMethods->getChainDetails(chainName); // sept25
	    const TrigConf::HLTChain* configChain = 0;
	    if (chainDetail) {
	      configChain = chainDetail->getConfigChain();
	      //lastStep = chainDetail->getChainStep();
	    }
	    std::vector<const HLT::TriggerElement*>::const_iterator p_te1;
	    std::vector<TrigConf::HLTTriggerElement*>::const_iterator p_te2;
	    std::vector<TrigConf::HLTSignature*>::const_iterator p_sig;
	    std::vector<TrigConf::HLTSignature*> dummylist;
	    std::vector<TrigConf::HLTSignature*>& siglist = dummylist;

	    if (configChain) siglist = configChain->signatureList();

	    for(int iEF=0; iEF<(int)combsHLT.size(); iEF++) {
	      std::vector< Feature<TrigRoiDescriptor> > initRois = combsHLT[iEF].get<TrigRoiDescriptor>("initialRoI",TrigDefs::alsoDeactivateTEs);
	      if( initRois.size() != 1 ) {
		ATH_MSG_WARNING("nr TrigRoiDescriptor is not 1 for chain=EF_" << chainName);
		retval = StatusCode::RECOVERABLE;
	      }
	      else {
		// check RoI <-> rec matching
		float dr = calc_dR(rec_eta,rec_phi,initRois[0].cptr()->eta(),initRois[0].cptr()->phi());
		name     = HistchainName + "_deltaR_btw_L1_MuidCB_For" + monalg3; // YY changed from monalg -> monalg3
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(dr);
		if( dr < dr_min_ef ) {
		  dr_min_ef = dr; 
		  id_min_ef = iEF;
		  last_step = 0;

		  //get last step 
		  //loop over TE
		  const std::vector<const HLT::TriggerElement*>& tes = combsHLT[iEF].tes();
		  for (p_te1=tes.begin(); p_te1!=tes.end(); ++p_te1) {
		    bool s = false;

		    //loop over signature list
		    for (p_sig=siglist.begin(); p_sig!=siglist.end(); ++p_sig) {
		      vector<TrigConf::HLTTriggerElement*>& tes2 = (*p_sig)->outputTEs();
		      for (p_te2=tes2.begin(); p_te2!=tes2.end(); ++p_te2) {
			if ( (*p_te1)->getId() == 
			    TrigConf::HLTUtils::string2hash( (*p_te2)->name(), "TE")) {
			  s = true;
			  break;
			}
		      }
		      if (s) {
			if ( (int) (*p_sig)->signature_counter() > last_step) 
			  last_step = (int) (*p_sig)->signature_counter();
			break;
		      }
		    }
		  }
		}
	      }
	    }
	    if( dr_min_ef < DR_CUT && id_min_ef >= 0) {
	      ATH_MSG_DEBUG("EF: dRmin=" << dr_min_ef);

	      if (EF_lower_passed) {
		name     = HistchainName + monalg3 + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		// Barrel/Endcap
		name     = HistchainName + monalg3 + "_Turn_On_Curve_wrt" + wrtalg + bestr[iBarrelCB] + "_Denominator";
		ATH_MSG_DEBUG( name << " filling" );
		hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		// repeating, with highPT range
		if (MS_lower_passed) {
		  name     = HistchainName + m_MSchainName + monalg3 + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);

		  // Barrel/Endcap
		  name     = HistchainName + m_MSchainName + monalg3 + "_Turn_On_Curve_wrt" + wrtalg + bestr[iBarrelCB] + "_Denominator";
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		}

	      }
	      // if (ES_lower_passed) {}
	      // ES trigger-aware
	      for (int i = 0; i <= m_maxESbr; i++) {
		if(!m_CB_mon_ESbr[i])continue; 
		if (m_passedES[i]) {
		  name     = HistchainName + m_triggerES[i] + monalg3 + "_Turn_On_Curve_wrt" + wrtalg + "_Denominator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		}
	      }

	      float ef_cb_pt  = 0.;
	      float ef_cb_eta = 0.;
	      float ef_cb_phi = 0.;
	      int ef_cb_mtype = 0;
	      float tmp_ef_cb_pt  = 0.;
	      float tmp_ef_cb_eta = 0.;
	      float tmp_ef_cb_phi = 0.;
	      int   tmp_ef_cb_mtype = 0;

	      /*      bool isPassed_chain  = getTDT()->getChainGroup("EF_"+chainName)->isPassed(); */
	      bool ef_active       = combsHLT[id_min_ef].active();
	      ATH_MSG_DEBUG("...comb active=" << ef_active );

	      std::vector< Feature<xAOD::MuonContainer> > efarr[2]; 
	      efarr[0] = fHLT.get<xAOD::MuonContainer>("MuonEFInfo",TrigDefs::alsoDeactivateTEs);
	      ATH_MSG_DEBUG("...MuonEFInfo size=" << efarr[0].size() );
	      efarr[1] = fHLT.get<xAOD::MuonContainer>("eMuonEFInfo",TrigDefs::alsoDeactivateTEs);
	      ATH_MSG_DEBUG("...eMuonEFInfo size=" << efarr[1].size() );
	      float maxdr = 9999.;
	      for (int ief = 0; ief < 2; ief++) {
		if( efarr[ief].size() >= 1 ) {
		  std::vector< Feature<xAOD::MuonContainer> > ef = efarr[ief]; 

		  ATH_MSG_DEBUG("...xAOD EFInfo: label/active=" << getTEName(*ef[0].te()) << " / " << ef[0].te()->getActiveState());

		  std::vector<Feature<xAOD::MuonContainer> >::const_iterator fIt = ef.begin();
		  for ( ; fIt != ef.end() ; ++fIt) {
		    const xAOD::MuonContainer* ef_cont = (*fIt);
		    if (ef_cont) {
		      ATH_MSG_DEBUG(" CB ef_contContainter size:"<<ef_cont->size()); 

		      xAOD::MuonContainer::const_iterator iCont = ef_cont->begin();
		      for(; iCont != ef_cont->end(); iCont++) {
			const xAOD::TrackParticle *ef_cb_trk;
			ef_cb_trk = (*iCont)->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle);

			ef_cb_pt = (*iCont)->pt()/ CLHEP::GeV;
			ef_cb_eta = (*iCont)->eta();
			ef_cb_phi = (*iCont)->phi();
			ef_cb_mtype = (*iCont)->muonType();
			if( ef_cb_trk){
			  if( ef_cb_trk->pt() == 0.){
			    ef_cb_pt  = -1.;
			    ef_cb_eta = 0.;
			    ef_cb_phi = 0.;  // YY
			    ef_cb_mtype = 0;
			  }else if( ef_cb_pt < fabs(ef_cb_trk->pt()) / CLHEP::GeV ){
			    ef_cb_pt  = fabs(ef_cb_trk->pt()) / CLHEP::GeV;
			    ef_cb_eta = ef_cb_trk->eta();
			    ef_cb_phi = ef_cb_trk->phi(); // YY
			    ef_cb_mtype = (*iCont)->muonType();
			  }
			  ATH_MSG_DEBUG("CB eta/pt=" << ef_cb_eta << " / " << ef_cb_pt/CLHEP::GeV );
			  ATH_MSG_DEBUG("CB track found for last_step " << last_step);
			}else{
			  ATH_MSG_DEBUG("No CB track found for last_step " << last_step);
			}
			float tmpdr = calc_dR(rec_eta,rec_phi,ef_cb_eta,ef_cb_phi);
			if(tmpdr<maxdr){
			  maxdr       = tmpdr;
			  tmp_ef_cb_pt    = ef_cb_pt ;
			  tmp_ef_cb_eta   = ef_cb_eta ;
			  tmp_ef_cb_phi   = ef_cb_phi ;
			  tmp_ef_cb_mtype = ef_cb_mtype ;
			}
			ef_cb_pt    = tmp_ef_cb_pt ;
			ef_cb_eta   = tmp_ef_cb_eta ;
			ef_cb_phi   = tmp_ef_cb_phi ;
			ef_cb_mtype = tmp_ef_cb_mtype ;

			ATH_MSG_DEBUG("CB eta/pt=" << ef_cb_eta << " / " << ef_cb_pt/CLHEP::GeV );
		      }
		    }
		  }
		}
	      }

	      //CB pt/eta checks
	      if( ef_active ){
		//if( pt_cut ){}
		if (EF_lower_passed) {
		  ATH_MSG_DEBUG("last step " << last_step );

		  name     = HistchainName + monalg3 + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  if (MS_lower_passed) {
		    name     = HistchainName + m_MSchainName + monalg3 + "_Turn_On_Curve_Numerator";
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		  // Barrel/Endcap
		  name     = HistchainName + monalg3 + bestr[iBarrelCB] + "_Turn_On_Curve_Numerator";
		  ATH_MSG_DEBUG( name << " filling" );
		  hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  if (MS_lower_passed) {
		    name     = HistchainName + m_MSchainName + monalg3 + bestr[iBarrelCB] + "_Turn_On_Curve_Numerator";
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }

		  // 2D coverage hist
		  ATH_MSG_DEBUG( "Offline/EFCB pt" << rec_pt << ", " << ef_cb_pt ); 
		  ATH_MSG_DEBUG( "Offline/EFCB eta" << rec_eta << ", " << ef_cb_eta );
		  ATH_MSG_DEBUG( "Offline/EFCB phi" << rec_phi << ", " << ef_cb_phi );
		  name     = HistchainName + monalg3 + "_etaphi_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(ef_cb_eta, ef_cb_phi);
		  name     = HistchainName + monalg3 + "_etaphi_coarse_wrt_MuidCB";
		  hist2(name, m_histdirdist2d)->Fill(ef_cb_eta, ef_cb_phi);

		  if (ef_cb_mtype > 0 and ef_cb_mtype <= 3) {
		    // MuonType-dependent distribution histos, inclusive
		    name     = HistchainName + m_EFAlgName[ef_cb_mtype - 1] + "_pt_wrt_MuidCB";
		    hist(name, m_histdirdist2d)->Fill(ef_cb_pt);
		    name     = HistchainName + m_EFAlgName[ef_cb_mtype - 1] + "_eta_wrt_MuidCB";
		    hist(name, m_histdirdist2d)->Fill(ef_cb_eta);
		    name     = HistchainName + m_EFAlgName[ef_cb_mtype - 1] + "_phi_wrt_MuidCB";
		    hist(name, m_histdirdist2d)->Fill(ef_cb_phi);
		    name     = HistchainName + m_EFAlgName[3] + "_pt_wrt_MuidCB";
		    hist(name, m_histdirdist2d)->Fill(ef_cb_pt);
		    name     = HistchainName + m_EFAlgName[3] + "_eta_wrt_MuidCB";
		    hist(name, m_histdirdist2d)->Fill(ef_cb_eta);
		    name     = HistchainName + m_EFAlgName[3] + "_phi_wrt_MuidCB";
		    hist(name, m_histdirdist2d)->Fill(ef_cb_phi);
		  } else {
		    ATH_MSG_DEBUG("ef_cb_mtype " << ef_cb_mtype);
		  }

		  // stat info on MuonType
		  name = HistchainName + "_EFMuonType_wrt_MuidCB";
		  hist(name, m_histdirdist2d)->Fill(float(ef_cb_mtype));

		}

		// if (ES_lower_passed) {}
		// ES trigger-aware
		for (int i = 0; i <= m_maxESbr; i++) {
		    name     = HistchainName + m_triggerES[i] + monalg3 + "_Turn_On_Curve_Numerator";
		  if(!m_CB_mon_ESbr[i])continue; 
		  if (m_passedES[i]) {
		    ATH_MSG_DEBUG("last step " << last_step );
		    name     = HistchainName + m_triggerES[i] + monalg3 + "_Turn_On_Curve_Numerator";
		    ATH_MSG_DEBUG( name << " filling" );
		    hist(name, m_histdireffnumdenom)->Fill(rec_pt);
		  }
		}

	      }else{
		ATH_MSG_DEBUG("not active last step " << last_step );
	      }


	    }

	  } // end of CB loop

	  //
	  return retval;
	}



	StatusCode HLTMuonMonTool::procChainDQA()
	{
	  ATH_MSG_DEBUG("procChainDQA");

	  StatusCode retval = StatusCode::SUCCESS;

	  // YY: 25.05.10 to clear m_checked:
	  retval.getCode();

	  procTriggerOverlap();

	  std::vector<std::string>::const_iterator it;
	  for(it=m_histChainMSonly.begin(); it != m_histChainMSonly.end() ; it++ ){
	    StatusCode sc = procChainDQA_MSonly(*it);
	    if(sc.isFailure()) {
	      ATH_MSG_VERBOSE("procChainDQA_MSonly failed for chain=" << *it );
	      retval = StatusCode::RECOVERABLE;
	    }
	  }

	  for(it=m_histChainEFFS.begin(); it != m_histChainEFFS.end() ; it++ ){
	    StatusCode sc = procChainDQA_standard(*it);
	    if (sc.isFailure()) {
	      ATH_MSG_VERBOSE("procChainDQA_standard failed for chain=" << *it );
	      retval = StatusCode::RECOVERABLE;
	    }
	  }


	  for(it=m_histChainGeneric.begin(); it != m_histChainGeneric.end() ; it++ ){
	    StatusCode sc = procChainDQA_generic(*it);
	    if (sc.isFailure()) {
	      ATH_MSG_VERBOSE("procChainDQA_generic failed for chain=" << *it );
	      retval = StatusCode::RECOVERABLE;
	    }
	  }


	  for(it=m_histChainEFiso.begin(); it != m_histChainEFiso.end() ; it++ ){
	    StatusCode sc = procChainDQA_generic(*it);
	    if (sc.isFailure()) {
	      ATH_MSG_VERBOSE("procChainDQA_generic failed for chain=" << *it );
	      retval = StatusCode::RECOVERABLE;
	    }
	  }


	  StatusCode sc = procChainDQA_HighPt();
	  if (sc.isFailure()) {
	    ATH_MSG_VERBOSE("procChainDQA_HighPt failed");
	    retval = StatusCode::RECOVERABLE;
	  }


	  if(endOfRunFlag()){
	    //triggers/event
	    for(std::map<std::string, std::string>::iterator it=m_ztpmap.begin(); it != m_ztpmap.end() ; it++ ){

	      std::string name1     = "Number_Of_"+ it->second + "_Passed_Events" ;
	      std::string name2     = it->second + "_Triggers_Per_Event" ;

	      //hist( name2, m_histdirrateratio ) -> Divide ( hist(name1, m_histdirrate), hist("Number_Of_Events", m_histdirrate));

	    }
	  }

	  return retval;
	}

	StatusCode HLTMuonMonTool::procChainDQA_MSonly(const std::string& chainName )
	{
	  ATH_MSG_DEBUG("procChainDQA_MSonly");

	  std::string denom;
	  std::string numer;
	  std::string effi;

	  if( endOfRunFlag() ){

	    std::string monalg[3]={"_L2MuonSA", "_MuonEFMS", "_MuonEFSA"};

	    std::string wrtalg[3]={"_L1", "_L2MuonSA", "_L2MuonSA"};
	    std::string bestr[2] = {"_Barrel", "_Endcap"};

	    //wrt SA muon && Upstream trigger
	    for( int alg = 0 ; alg < 3 ;alg ++ ){
	      denom = chainName + "_Turn_On_Curve_wrt_MuidSA_Denominator";
	      numer = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	      effi  = chainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidSA";
	      //hist(effi, m_histdireff)->Divide( hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom), 1, 1, "B" );
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
	      // Uncommented by YY 12.04.10

	      if (true) { // all
		double sumeff, sumerr;
		double sumn = hist(numer, m_histdireffnumdenom)->Integral(m_iMSL, m_iMSH);
		double sumd = hist(denom, m_histdireffnumdenom)->Integral(m_iMSL, m_iMSH);
		if (sumd == 0.) {
		  sumeff = 0.;
		  sumerr = 0.;
		} else {
		  sumeff = (double)sumn / (double)sumd;
		  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		}
		int ibin_holx = -1;
		if (0 == alg) {
		  int ibintmp = iL2MuonSA;
		  ibin_holx = ibintmp;
		} else if (2 == alg) {
		  int ibintmp = iEFSA;  
		  ibin_holx = ibintmp;
		}

		if (ibin_holx >= 0) {
		  std::string s = chainName + "_highpt_effsummary_by" + m_vectkwd.at(4);
		  ATH_MSG_DEBUG("hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr);
		  hist(s, m_histdireff)->SetBinContent(ibin_holx+1, sumeff);
		  hist(s, m_histdireff)->SetBinError(ibin_holx+1, sumerr);
		}
	      }

	      denom = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	      numer = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	      effi = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];

	      //  for ES ------------------------------------------------------------------------------------
	      for (int i = 0; i <= m_maxESbr; i++) {
		if(!m_MS_mon_ESbr[i])continue; 

		if (0 == alg) {
		  // L1 efficiency: new for 2011 HI runs and afterward
		  // only division once since it is "the zero-th" algorithm
		  denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidSA_Denominator";
		  numer = chainName + m_triggerES[i] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + "_Denominator";
		  effi  = chainName + m_triggerES[i] + "_L1" + "_Turn_On_Curve_wrt_MuidSA";
		  // Need to implement barrel and endcap ...
		  for (int be = 0; be < 2; be++) {
		    denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidSA" + bestr[be] + "_Denominator";
		    numer = chainName + m_triggerES[i] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + bestr[be] + "_Denominator";
		    effi  = chainName + m_triggerES[i] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
		  }
		}

		denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidSA_Denominator";
		numer = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
		effi  = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt_MuidSA";

		// MSonly summary: viewed only by EStag sample
		if (ESTAG == i || ESINDEP == i) {
		  double sumeff, sumerr;
		  double sumn = hist(numer, m_histdireffnumdenom)->Integral(m_iMSL, m_iMSH);
		  double sumd = hist(denom, m_histdireffnumdenom)->Integral(m_iMSL, m_iMSH);
		  if (sumd == 0.) {
		    sumeff = 0.;
		    sumerr = 0.;
		  } else {
		    sumeff = (double)sumn / (double)sumd;
		    sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		  }
		  int ibin_holx = -1;
		  if (0 == alg) {
		    int tmp_idx =  iL2MuonSA;
		    ibin_holx   =  tmp_idx;
		  } else if (2 == alg) {
		    int tmp_idx = iEFSA;
		    ibin_holx   = tmp_idx;
		  }

		  if (ibin_holx >= 0) {
		    std::string s = chainName + "_highpt_effsummary_by" + m_triggerES[i];
		    ATH_MSG_DEBUG("hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr);
		    hist(s, m_histdireff)->SetBinContent(ibin_holx+1, sumeff);
		    hist(s, m_histdireff)->SetBinError(ibin_holx+1, sumerr);
		  }
		}

		denom = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
		numer = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
		effi = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	      }
	      //  for ES : END ------------------------------------------------------------------------------


	      if (0 == alg || 2 == alg) {  // Barrel/Endcap
		for (int be = 0; be < 2; be++) {
		  denom = chainName + "_Turn_On_Curve_wrt_MuidSA" + bestr[be] + "_Denominator";
		  numer = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		  effi  = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidSA";
		  //hist(effi, m_histdireff)->Divide( hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom), 1, 1, "B" );
		  fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
		  // Uncommented by YY 12.04.10

		  denom = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
		  numer = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		  effi = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
		  //hist(effi, m_histdireff)->Divide( hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom), 1, 1, "B" );
		  fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
		}
	      } 

	    }

	    for (int ialg = 0; ialg < 3; ialg++) {
	      denom = chainName + monalg[ialg] + "_etaphi_coarse_wrt_MuidCB";
	      numer = chainName + monalg[ialg] + "_etaphi_coarse1d_wrt_MuidCB";
	      // ATH_MSG_INFO("denom " << denom << " numer " << numer);
	      TH2F *h2d = (TH2F *)hist2(denom, m_histdirdist2d);
	      TH1F *h1d = (TH1F *)hist(numer, m_histdirdist2d);
	      refill_2d1d_coarse(h2d, h1d);
	    }

	  } else if( endOfLumiBlockFlag() ){
	  }

	  return StatusCode::SUCCESS;
	}

	StatusCode HLTMuonMonTool::procChainDQA_standard(const std::string& chainName )
	{
	  ATH_MSG_DEBUG("procChainDQA_standard");

	  std::string denom;
	  std::string numer;
	  std::string effi;
	  
	  int iSTDL = 39;
	  int iSTDH = 120;
	  if(!m_HI_pp_mode){
	    iSTDL = 17;
	    iSTDH = 75;
	  }else{
	    iSTDL = 75;//25GeV
	    iSTDH = 120;
	  }

	  if( endOfRunFlag() ){


	    // add by Yuan :  to book the histogram //
	    if(chainName.find("EFFS")!= string::npos){

	      std::string bestr[2] = {"_Barrel", "_Endcap"};

	      denom = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";

	      numer = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";

	      effi =  chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));


	      for (int be =0; be<2; be++){

		denom = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + bestr[be] + "_Denominator";

		numer = chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + bestr[be] + "_Numerator";

		effi =  chainName + "_tagEFFSpre" + "_Turn_On_Curve_wrt_probe_MuidCB" + bestr[be];
		fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));

		double sumd, sumn, sumeff, sumerr;
		sumn = hist(numer, m_histdireffnumdenom)->Integral(iSTDL, iSTDH);
		sumd = hist(denom, m_histdireffnumdenom)->Integral(iSTDL, iSTDH);
		if (sumd == 0.) {
		  sumeff = 0.;
		  sumerr = 0.;
		} else {
		  sumeff = (double)sumn / (double)sumd;
		  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		}
		std::string s = chainName + "_EFplateau_wrtOffline" ;
		ATH_MSG_DEBUG("hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr);
		hist(s, m_histdireff)->SetBinContent(be+1, sumeff);
		hist(s, m_histdireff)->SetBinError(be+1, sumerr);



	      }

	      denom = chainName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";

	      numer = chainName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";

	      effi =  chainName + "_tagEFFSpre_mu0_15" + "_Turn_On_Curve_wrt_probe_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));


	      denom = chainName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";

	      numer = chainName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";

	      effi =  chainName + "_tagEFFSpre_mu15_20" + "_Turn_On_Curve_wrt_probe_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));


	      denom = chainName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Denominator";

	      numer = chainName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB" + "_Numerator";

	      effi =  chainName + "_tagEFFSpre_mu20" + "_Turn_On_Curve_wrt_probe_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));


	      std::string mu_range_names[3] = {"_mu0_15", "_mu15_20","_mu20"};

	      for (int mu_range =0; mu_range<3; mu_range++){

		denom = chainName + "_tagEFFSpre" + mu_range_names[mu_range] + "_Turn_On_Curve_wrt_probe_MuidCB"  + "_Denominator";

		numer = chainName + "_tagEFFSpre" + mu_range_names[mu_range] + "_Turn_On_Curve_wrt_probe_MuidCB"  + "_Numerator";

		effi =  chainName + "_tagEFFSpre" + mu_range_names[mu_range] + "_Turn_On_Curve_wrt_probe_MuidCB" ;
		fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
	      

		double sumd, sumn, sumeff, sumerr;
		sumn = hist(numer, m_histdireffnumdenom)->Integral(iSTDL, iSTDH);
		sumd = hist(denom, m_histdireffnumdenom)->Integral(iSTDL, iSTDH);
		if (sumd == 0.) {
		  sumeff = 0.;
		  sumerr = 0.;
		} else {
		  sumeff = (double)sumn / (double)sumd;
		  sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		}
		std::string s = chainName + "_EFplateau_wrtOffline_mu_dependence" ;
		ATH_MSG_DEBUG("hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr);
		hist(s, m_histdireff)->SetBinContent(mu_range+1, sumeff);
		hist(s, m_histdireff)->SetBinError(mu_range+1, sumerr);

	      }


	      denom = chainName + "_Turn_On_Curve_wrt_subleading_MuidCB" + "_Denominator";

	      numer = chainName + "_Turn_On_Curve_wrt_subleading_MuidCB" + "_Numerator";

	      effi =  chainName + "_Turn_On_Curve_wrt_subleading_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));


	      denom = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB_Denominator";

	      numer = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB_Numerator";

	      effi =  "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));


	      denom = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB_Denominator";

	      numer = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB_Numerator";

	      effi =  "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));


	      denom = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_probe_MuidCB_Denominator";

	      numer = "EFFSpre_dimuonTP_Turn_On_Curve_wrt_L1_probe_MuidCB_Denominator";

	      effi =  "EFFSpre_dimuonTP_L1_Turn_On_Curve_wrt_probe_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));


	    }

	  } else if(endOfLumiBlockFlag()){
	  }
	  return StatusCode::SUCCESS;
	  //*****************************************//
	}


	StatusCode HLTMuonMonTool::procChainDQA_generic(const std::string& chainName )
	{
	  ATH_MSG_DEBUG("procChainDQA_generic");

	  std::string denom;
	  std::string numer;
	  std::string effi;

	  if( endOfRunFlag() ){
	    std::string monalg[3]={"_L2MuonSA", "_MuComb", "_EFmuon"};

	    std::string wrtalg[3]={"_L1", "_L2MuonSA", "_MuComb"};
	    std::string bestr[2] = {"_Barrel", "_Endcap"};

	    //wrt CB muon && Upstream trigger
	    for( int alg = 0 ; alg < 3 ; alg++ ){
	      denom = chainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	      numer = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	      effi  = chainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));

	      denom = chainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
	      numer = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	      effi  = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));

	      denom = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	      numer = chainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	      effi = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));

	      denom = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
	      numer = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_Numerator";
	      effi = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
	      fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));

	      //  for ES ------------------------------------------------------------------------------------
	      for (int i = 0; i <= m_maxESbr; i++) {
		if(!m_CB_mon_ESbr[i])continue; 

		if (0 == alg) {
		  // L1 efficiency: new for 2011 HI runs and afterward
		  // only division once since it is "the zero-th" algorithm
		  denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
		  numer = chainName + m_triggerES[i] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + "_Denominator";
		  effi  = chainName + m_triggerES[i] + "_L1" + "_Turn_On_Curve_wrt_MuidCB";
		  fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
		  // Need to implement barrel and endcap ...
		  for (int be = 0; be < 2; be++) {
		    denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
		    numer = chainName + m_triggerES[i] + "_L2MuonSA" + "_Turn_On_Curve_wrt" + "_L1" + bestr[be] + "_Denominator";
		    effi  = chainName + m_triggerES[i] + "_L1" + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
		    fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));

		    if (ESINDEP == i) {
		      // integrating over and fill in a summary histogram
		      double sumeff, sumerr;
		      double sumn = hist(numer, m_histdireffnumdenom)->Integral(m_iSTDL, m_iSTDH); // 30-50 GeV
		      double sumd = hist(denom, m_histdireffnumdenom)->Integral(m_iSTDL, m_iSTDH);
		      if (sumd == 0.) {
			sumeff = 0.;
			sumerr = 0.;
		      } else {
			sumeff = (double)sumn / (double)sumd;
			sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		      }

		      std::string s = chainName + "_L1plateau_wrtOffline_by_ESindep";
		      ATH_MSG_DEBUG("hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr);
		      hist(s, m_histdireff)->SetBinContent(be+1, sumeff);
		      hist(s, m_histdireff)->SetBinError(be+1, sumerr);
		    }
		  }	  
		}

		denom = chainName + m_triggerES[i] + "_Turn_On_Curve_wrt_MuidCB_Denominator";
		numer = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
		effi  = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt_MuidCB";
		fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));

		// Summary ESid and ESindep
		if (0 == alg || 1 == alg || 2 == alg) {
		  // if (ESID == i || ESINDEP == i) {
		  double sumd, sumn, sumeff, sumerr;
		  sumn = hist(numer, m_histdireffnumdenom)->Integral(m_iSTDL, m_iSTDH);  // 40 - 100 GeV
		  sumd = hist(denom, m_histdireffnumdenom)->Integral(m_iSTDL, m_iSTDH);
		  if (sumd == 0.) {
		    sumeff = 0.;
		    sumerr = 0.;
		  } else {
		    sumeff = (double)sumn / (double)sumd;
		    sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		  }
		  int ibin_holx = -1;
		  if (0 == alg) {
		    ibin_holx = iL2MuonSA;
		  } else if (1 == alg) {
		    ibin_holx = iMuComb;
		  } else if (2 == alg) {
		    ibin_holx = iEFCB;
		  }

		  std::string s = chainName + "_highpt_effsummary_by" + m_triggerES[i];
		  ATH_MSG_DEBUG("hist summary: " << s << " n: " << sumn << " d: " << sumd << " eff: " << sumeff << " err: " << sumerr);
		  hist(s, m_histdireff)->SetBinContent(ibin_holx+1, sumeff);
		  hist(s, m_histdireff)->SetBinError(ibin_holx+1, sumerr);
		}

		denom = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + "_Denominator";
		numer = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_Numerator";
		effi = chainName + m_triggerES[i] + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg];
		fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));

		}
		//  for ES : END ------------------------------------------------------------------------------

		// Barrel/Endcap
		if (0 == alg || 1 == alg || 2 == alg) {
		  for (int be = 0; be < 2; be++) {
		    denom = chainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
		    numer = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		    effi  = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
		    fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
		  }


		  for (int be = 0; be < 2; be++) {
		    denom = chainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB" + bestr[be] + "_Denominator";
		    numer = chainName + m_MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		    effi  = chainName + m_MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_wrt_MuidCB";
		    fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
		  }

		  for (int be = 0; be < 2; be++) {
		    denom = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
		    numer = chainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		    effi = chainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
		    fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
		  }

		  for (int be = 0; be < 2; be++) {
		    denom = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be] + "_Denominator";
		    numer = chainName + m_MSchainName + monalg[alg] + bestr[be] + "_Turn_On_Curve_Numerator";
		    effi = chainName + m_MSchainName + monalg[alg] + "_Turn_On_Curve_wrt" + wrtalg[alg] + bestr[be];
		    fillProfile( profile(effi+"_Profile", m_histdireff), hist(numer, m_histdireffnumdenom), hist(denom, m_histdireffnumdenom));
		  }

		}

	      }
	      for (int ialg = 0; ialg < 3; ialg++) {
		denom = chainName + monalg[ialg] + "_etaphi_coarse_wrt_MuidCB";
		numer = chainName + monalg[ialg] + "_etaphi_coarse1d_wrt_MuidCB";
		// ATH_MSG_INFO("denom " << denom << " numer " << numer);
		TH2F *h2d = (TH2F *)hist2(denom, m_histdirdist2d);
		TH1F *h1d = (TH1F *)hist(numer, m_histdirdist2d);
		refill_2d1d_coarse(h2d, h1d);
	      }
	    } else if(endOfLumiBlockFlag() ){
	    }

	    return StatusCode::SUCCESS;
	  }


	  StatusCode HLTMuonMonTool::procChainDQA_HighPt()
	  {
	    // YY 2e33
	    // std::string charr[4] = {"mu20", "mu20_MG", "mu15i", "mu40_MSonly_barrel"};
	    // std::string charr[4] = {"mu22_medium", "mu22_MG_medium", "mu20i_medium", "mu40_MSonly_barrel_medium"};
	    // std::string monarr[4] = {"_MuonEFCB", "_MuGirlEF", "_MuonEFCB", "_MuonEFSA"};
	    // std::string monL2arr[4] = {"_L2MuonSA", "_MuGirlL2", "_MuIso", "_L2MuonSA"};
	    // pp_v4

	    int MAXARR = 0;
	    std::vector<std::string> charr;
	    std::vector<std::string> monarr;
	    std::vector<std::string> monL2arr;
	    std::vector<bool> isBarrelMon;
	    std::vector<bool> isMSbMon;
	    std::vector<bool> monL1;
	    std::vector<bool> isefIsolation;
	    for(unsigned int ich = 0; ich < m_chainsGeneric.size(); ich++){
	      MAXARR++;
	      charr.push_back("muChain"+std::to_string(ich+1));
	      monarr.push_back("_EFmuon");
	      monL2arr.push_back("_L2MuonSA");
	      isBarrelMon.push_back(false);
	      isMSbMon.push_back(true);
	      monL1.push_back(true);
	      isefIsolation.push_back(false);
	    }
	    for(unsigned int ich = 0; ich < m_chainsEFiso.size(); ich++){
	      MAXARR++;
	      charr.push_back("muChainEFiso"+std::to_string(ich+1));
	      monarr.push_back("_EFmuon");
	      monL2arr.push_back("_L2MuonSA");
	      isBarrelMon.push_back(false);
	      isMSbMon.push_back(false);
	      monL1.push_back(true);
	      isefIsolation.push_back(true);
	    }
	    for(unsigned int ich = 0; ich < m_chainsMSonly.size(); ich++){
	      MAXARR++;
	      charr.push_back("muChainMSonly"+std::to_string(ich+1));
	      monarr.push_back("_MuonEFSA");
	      monL2arr.push_back("_L2MuonSA");
	      isBarrelMon.push_back(true);
	      isMSbMon.push_back(false);
	      monL1.push_back(false);
	      isefIsolation.push_back(false);
	    }


	    //  const int MAXARR = 3;
	    //  std::string charr[MAXARR] = {"mu36_tight", "mu24i_tight", "mu50_MSonly_barrel_tight"};
	    //  std::string monarr[MAXARR] = {"_EFmuon", "_EFmuon", "_MuonEFSA"};
	    //  std::string monL2arr[MAXARR] = {"_L2MuonSA", "_L2MuonSA", "_L2MuonSA"};
	    //  bool isBarrelMon[MAXARR] = {false, false, true}; // enable MSonly
	    //  bool isMSbMon[MAXARR] = {true, false, false}; // Skip isol and MSonly
	    //  bool monL1[MAXARR] = {true, true, false}; // Skip MSonly
	    //  bool isefIsolation[MAXARR] = {false, true, false}; // EF isolation  add by Yuan 

	    // YY: dirty fix, should improve by get configuration from Python, but for M4 ...
	    //  if (1) {
	    //    return StatusCode::SUCCESS;
	    //  }

	    if (endOfRunFlag()) {
	      for (int ialg = 0; ialg < MAXARR; ialg++) {
		std::string chainName = charr[ialg];
		std::string MoniAlg = monarr[ialg];
		std::string MoniL2Alg = monL2arr[ialg];

		/* 1. Picking up ztp graph */
		std::string hdirztp = "HLT/MuonMon/MuZTP/" + chainName;
		std::string histZtpNum, histZtpDen;
		if (!isBarrelMon[ialg]) {
		  histZtpNum = "muZTP_Pt_4bins_EFfired_" + chainName;
		  histZtpDen = "muZTP_Pt_4bins_L1fired_" + chainName;
		} else {
		  // make sure you rewrite POSTPROSESSOR !!!!!
		  histZtpNum = "muZTP_Pt_B_4bins_EFfired_" + chainName;
		  histZtpDen = "muZTP_Pt_B_4bins_L1fired_" + chainName;
		}
		if(isefIsolation[ialg]) histZtpNum = "muZTP_Pt_4bins_EFIsofired_" + chainName;  // add by Yuan

		/* 2. Filling summary histogram from ZTP values */
		std::string sumhist = chainName + "_highpt3bins_effwrtL1";
		for (int ibin = 2; ibin <= 3; ibin++) {
		  // at the moment it is not correct if we run the algorithm # 4: mu40_MSonly_barrel .
		  double sumeff, sumerr;
		  double sumn = hist(histZtpNum, hdirztp)->Integral(ibin, ibin);
		  if(isBarrelMon[ialg] || isMSbMon[ialg]) sumn = hist(histZtpNum, hdirztp)->Integral(ibin+1, ibin+1);
		  double sumd = hist(histZtpDen, hdirztp)->Integral(ibin, ibin);
		  if(isBarrelMon[ialg] || isMSbMon[ialg]) sumd = hist(histZtpDen, hdirztp)->Integral(ibin+1, ibin+1);
		  if (sumd == 0.) {
		    sumeff = 0.;
		    sumerr = 0.;
		  } else {
		    sumeff = (double)sumn / (double) sumd;
		    sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		  }
		  hist(sumhist, m_histdireff)->SetBinContent(ibin-1, sumeff);
		  hist(sumhist, m_histdireff)->SetBinError(ibin-1, sumerr);
		}

		/* 3. Picking up chainDQ MSonly graph   abandoned!!! */   
		/* EF efficiency wrt L1, as for the ztp graph = overall HLT efficiency wrt L1: not possible, wrt offline 
		   if (isMSbMon[ialg]) {  // skip muIso and MSonly !!!
		   std::string histChNum = chainName + m_MSchainName + MoniAlg + "_Turn_On_Curve_Numerator";
		// std::string histChDen = chainName + m_MSchainName + "_Turn_On_Curve_wrt_MuidCB_Denominator";
		std::string histChDen = chainName + m_MSchainName + MoniL2Alg + "_Turn_On_Curve_wrt_L1_Denominator";

		// Integrate 100-300 GeV
		double sumeff, sumerr;
		double sumn = hist(histChNum, m_histdireffnumdenom)->Integral(21, 28);
		double sumd = hist(histChDen, m_histdireffnumdenom)->Integral(21, 28);
		if (sumd == 0.) {
		sumeff = 0.;
		sumerr = 0.;
		} else {
		sumeff = (double)sumn / (double) sumd;
		sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		}
		hist(sumhist, m_histdireff)->SetBinContent(3, sumeff);
		hist(sumhist, m_histdireff)->SetBinError(3, sumerr);
		}
		*/	

		// 4. Fill L1 summary histograms // to be put in the postprocessor!!
		if (monL1[ialg]) { // MUST skip muGirl, muIso and MSonly as histograms are not defined!!!
		  hdirztp = "HLT/MuonMon/MuZTP/" + chainName;
		  std::string histZtpNumB = "muZTP_Pt_B_L1fired_" + chainName;
		  std::string histZtpDenB = "muZTP_Pt_B_" + chainName;
		  std::string histZtpNumE = "muZTP_Pt_EC_L1fired_" + chainName;
		  std::string histZtpDenE = "muZTP_Pt_EC_" + chainName;
		  sumhist = chainName + "_highptL1plateau_wrtOffline";

		  double sumeff, sumerr;
		  double sumn = 0;
		  double sumd = 0;
		  if( !m_HI_pp_mode){
		  sumn = hist(histZtpNumB, hdirztp)->Integral(13, 25); // 30-50 GeV
		  sumd = hist(histZtpDenB, hdirztp)->Integral(13, 25);
		  
		  }else{
		  sumn = hist(histZtpNumB, hdirztp)->Integral(13, 20); // 60-100 GeV
		  //sumn = hist(histZtpNumB, hdirztp)->Integral(7, 10); // 30-50 GeV
		  sumd = hist(histZtpDenB, hdirztp)->Integral(13, 20);
		  //sumd = hist(histZtpDenB, hdirztp)->Integral(7, 10);
		  }
		  if (sumd == 0.) {
		    sumeff = 0.;
		    sumerr = 0.;
		  } else {
		    sumeff = (double)sumn / (double) sumd;
		    sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		  }
		  hist(sumhist, m_histdireff)->SetBinContent(1, sumeff);
		  hist(sumhist, m_histdireff)->SetBinError(1, sumerr);

		  if( !m_HI_pp_mode){
		    sumn = hist(histZtpNumB, hdirztp)->Integral(13, 25); // 12-25 GeV
		    //sumn = hist(histZtpNumB, hdirztp)->Integral(16, 25); // 15-25 GeV
		    sumn = hist(histZtpDenB, hdirztp)->Integral(13, 25);
		    //sumn = hist(histZtpDenB, hdirztp)->Integral(16, 25);
		  }else{
		    //sumn = hist(histZtpNumE, hdirztp)->Integral(7, 10);
		    sumn = hist(histZtpNumE, hdirztp)->Integral(13, 20);
		    sumd = hist(histZtpDenE, hdirztp)->Integral(13, 20);
		    //sumd = hist(histZtpDenE, hdirztp)->Integral(7, 10);
		  }
		  if (sumd == 0.) {
		    sumeff = 0.;
		    sumerr = 0.;
		  } else {
		    sumeff = (double)sumn / (double) sumd;
		    sumerr = sqrt((double)sumn * (1.-sumeff)) / (double)sumd;
		  }
		  hist(sumhist, m_histdireff)->SetBinContent(2, sumeff);
		  hist(sumhist, m_histdireff)->SetBinError(2, sumerr);

		}
	      }

	    } else if (endOfLumiBlockFlag()) {
	    }
	    return StatusCode::SUCCESS;
	  }


	  double HLTMuonMonTool::calc_dphi(double phi_i, double phi_j)
	  {
	    double dphi = fabs(phi_i - phi_j);
	    if( dphi > CLHEP::pi ) dphi = CLHEP::pi*2 - dphi;
	    return dphi;
	  }

	  double HLTMuonMonTool::calc_dR(double eta_i, double phi_i, double eta_j, double phi_j)
	  {
	    double deta = eta_i - eta_j;
	    double dphi = calc_dphi(phi_i, phi_j);
	    double dR   = pow( (deta*deta + dphi*dphi), 0.5 );
	    return dR;
	  }

	  bool HLTMuonMonTool::checkOfflineSAdR()
	  {

	    for(int i_rec1=0; i_rec1<(int)m_RecMuonSA_pt.size(); i_rec1++) {

	      float eta1 = m_RecMuonSA_eta[i_rec1];
	      float phi1 = m_RecMuonSA_phi[i_rec1];

	      for(int i_rec2=i_rec1+1; i_rec2<(int)m_RecMuonSA_pt.size(); i_rec2++) {

		float eta2 = m_RecMuonSA_eta[i_rec2];
		float phi2 = m_RecMuonSA_phi[i_rec2];

		float dr = calc_dR(eta1, phi1, eta2, phi2);

		if(dr < 0.8 ){
		  //ATH_MSG_INFO("checkOfflineSAdR:" << dr);
		  return false;
		}
	      }
	    }
	    return true;
	  }

	  bool HLTMuonMonTool::checkOfflineCBdR()
	  {

	    for(int i_rec1=0; i_rec1<(int)m_RecMuonCB_pt.size(); i_rec1++) {

	      float eta1 = m_RecMuonCB_eta[i_rec1];
	      float phi1 = m_RecMuonCB_phi[i_rec1];

	      for(int i_rec2=i_rec1+1; i_rec2<(int)m_RecMuonCB_pt.size(); i_rec2++) {

		float eta2 = m_RecMuonCB_eta[i_rec2];
		float phi2 = m_RecMuonCB_phi[i_rec2];

		float dr = calc_dR(eta1, phi1, eta2, phi2);

		if(dr < 0.8 ){
		  //ATH_MSG_INFO("checkOfflineCBdR:" << dr);
		  return false;
		}
	      }
	    }
	    return true;
	  }



	  StatusCode HLTMuonMonTool::fillRecMuon()
	  {
	    ATH_MSG_DEBUG("entering fillRecMuon");

	    int nOFL=0;
	    int nOFLMS=0;
	    int nOFLSA=0;
	    int nOFLCB=0;
	    int nOFLLPT=0;

	    m_RecMuonSA_pt.clear();
	    m_RecMuonSA_eta.clear();
	    m_RecMuonSA_phi.clear();
	    m_RecMuonSA_d0.clear();
	    m_RecMuonSA_z0.clear();
	    m_RecMuonSA_charge.clear();
	    m_RecMuonSA_hasCB.clear();
	    m_RecMuonSA_isGoodCB.clear();

	    m_RecMuonCB_pt.clear();
	    m_RecMuonCB_eta.clear();
	    m_RecMuonCB_phi.clear();
	    m_RecMuonCB_charge.clear();
	    m_RecMuonCB_isGoodCB.clear();
	    m_RecMuonCB_eloss.clear();   //for muIso
	    m_RecMuonCB_etcone.clear();  //for muIso
	    m_RecMuonCB_ptcone.clear();  //for muIso
	    m_RecMuonCB_ptcone30.clear();  //for muIso tomoe added
	    m_RecMuonCB_etcone30.clear();  //for muIso tomoe added


	    //const Analysis::MuonContainer* muonCont;
	    const xAOD::MuonContainer* muonCont;

	    std::string muonKey = "Muons"; // MuidMuonCollection

	    StatusCode sc = m_storeGate->retrieve(muonCont, muonKey);
	    if(sc.isFailure()){
	      ATH_MSG_WARNING("Container of muon particle with key " << muonKey << " not found in Store Gate");
	      return StatusCode::SUCCESS;
	    }

	    ATH_MSG_DEBUG("Container of muon particle with key " << muonKey << " found in Store Gate with size " << muonCont->size()); 

	    // get vertex
	    // std::string vxKey = "VxPrimaryCandidate";  // attention
	    std::string vxKey = "HLT_xAOD__VertexContainer_xPrimVx";  // May change to Offline Vx container key if possible
	    const xAOD::VertexContainer* vxCont = 0;
	    sc = m_storeGate->retrieve(vxCont, vxKey);
	    if (sc.isFailure()) {
	      ATH_MSG_INFO("Container of muon particle with key " << vxKey << " not found in Store Gate");
	      return StatusCode::SUCCESS;
	    }

	    // need to select vertex!!!
	    xAOD::VertexContainer::const_iterator p;
	    const xAOD::Vertex* vtx = 0;
	    for (p = vxCont->begin(); p != vxCont->end(); ++p) {
	      //reject dummy vertex.
	      //note1: Trk::NoVtx(=0) means dummy vertex.
	      //note2: There are mainly Trk::PriVtx(=1) but also there may be
	      //       other type of vertex (like Trk::PileUp(=3)).
	      //       Keep these vertex stored.
	      if ((*p)->vertexType() == xAOD::VxType::NoVtx) {
		continue;
	      } else {
		//vtx = &((*p)->recVertex());
		vtx = (*p);
		ATH_MSG_DEBUG("MuonHLTMon: vertex found" << (vtx->position()).x()
		    << " " << (vtx->position()).y() << " " << (vtx->position()).z());
		break;
	      }
	    }
	    if (vtx == 0) {
	      ATH_MSG_DEBUG("MuonHLTMon: no vertex found");
	    }

	    xAOD::MuonContainer::const_iterator muonItr  = muonCont->begin();
	    xAOD::MuonContainer::const_iterator muonItrE = muonCont->end();

	    // Offline

	    //const float ZERO_LIMIT  = 0.0001;

	    const float SA_PT_CUT   = 2.5; // GeV
	    const float SA_ETA_CUT  = 2.4;
	    const float SA_Z0_CUT   = 500;

	    const float CB_PT_CUT   = 2.5; // GeV
	    const float CB_ETA_CUT  = 2.4;

	    int isCombined=0;
	    for ( ; muonItr != muonItrE; ++muonItr) {
	      nOFL++;
	      float pt = fabs((*muonItr)->pt())/CLHEP::GeV;
	      ATH_MSG_DEBUG("Muon : "   
		  << " eta = " << (*muonItr)->eta()
		  << " phi = " << (*muonItr)->phi()
		  << " pt  = " << fabs((*muonItr)->pt())/CLHEP::GeV
		  << " Standalone  = " << ((*muonItr)->muonType()==xAOD::Muon::MuonStandAlone)
		  << " Combined  = " << ((*muonItr)->muonType()==xAOD::Muon::Combined)
		  << " Low pt  = " << ((*muonItr)->muonType()==xAOD::Muon::SegmentTagged && (*muonItr)->inDetTrackParticleLink()!=0));
	      //                  << " Low pt  = " << (*muonItr)->isLowPtReconstructedMuon() );  // method deprecated

	      if( (*muonItr)->muonSpectrometerTrackParticleLink().isValid() ){
		ATH_MSG_DEBUG(" count MS muon");
		nOFLMS++;
		const xAOD::TrackParticle* ms_trk=0;
		ms_trk = *((*muonItr)->muonSpectrometerTrackParticleLink());
		pt = ms_trk->pt() / CLHEP::GeV ;
		if( pt > 4 )
		  hist("Number_Of_Moore_MS_Muons_4GeV_Cut",m_histdirrate)->Fill(m_lumiblock);
		if( pt > 10 )
		  hist("Number_Of_Moore_MS_Muons_10GeV_Cut",m_histdirrate)->Fill(m_lumiblock);
	      }

	      if((*muonItr)->muonSpectrometerTrackParticleLink().isValid()){
		if((*muonItr)->muonType()!=xAOD::Muon::MuonStandAlone && (*muonItr)->muonType()!=xAOD::Muon::Combined) continue;
		ATH_MSG_DEBUG(" count SA muon"); 
		nOFLSA++;
		const xAOD::TrackParticle* ms_trk=0;
		ms_trk = *((*muonItr)->muonSpectrometerTrackParticleLink());
		if(!ms_trk) continue;
		//pt = ms_trk->pt() / CLHEP::GeV ;
		pt = fabs((*muonItr)->pt())/CLHEP::GeV; // there seem to be bug in MS trk pt, so use rec pt now
		if( pt > 4 )
		  hist("Number_Of_Moore_SA_Muons_4GeV_Cut",m_histdirrate)->Fill(m_lumiblock);
		if( pt > 10 )
		  hist("Number_Of_Moore_SA_Muons_10GeV_Cut",m_histdirrate)->Fill(m_lumiblock);
		float eta = ms_trk->eta();
		float phi = ms_trk->phi();
		float d0 = 0.;
		float z0 = 0.;
		float charge = ms_trk->charge();
		const  Trk::Perigee& ms_measPerigee = ms_trk->perigeeParameters();
		d0  = ms_measPerigee.parameters()[Trk::d0];
		z0  = ms_measPerigee.parameters()[Trk::z0];

		// YY: CB, isGoodCB;
		bool hasCB = ((*muonItr)->muonType()==xAOD::Muon::Combined);
		bool isGoodCB = false;
		if (hasCB) {
		  const xAOD::TrackParticle* comb=0;
		  comb = *((*muonItr)->combinedTrackParticleLink());
		  if (!comb) {
		    ATH_MSG_INFO("HLTMuonMon: is a combined offline muon but no pointer to TrackParticle");
		  } else {
		    pt = comb->pt()/CLHEP::GeV;
		    const  Trk::Perigee measPerigee = comb->perigeeParameters();
		    float d0comb = measPerigee.parameters()[Trk::d0];
		    float z0comb = measPerigee.parameters()[Trk::z0];
		    uint8_t nPixcomb = 0; 
		    comb->summaryValue(nPixcomb, xAOD::numberOfPixelHits);
		    uint8_t nSCTcomb = 0;
		    comb->summaryValue(nSCTcomb, xAOD::numberOfSCTHits);
		    if (vtx != 0) {
		      // if (nPixcomb >= 1 && nSCTcomb >= 6 && abs(z0comb - (vtx->position()).z()) < 5. && abs(d0comb) < 1.5) { } // Run 1 requirement
		      if ((*muonItr)->quality()<= xAOD::Muon::Medium && (*muonItr)->passesIDCuts() && ((*muonItr)->author()==xAOD::Muon::Author::MuidCo || (*muonItr)->author()==xAOD::Muon::Author::STACO)) {
			isGoodCB = true;
		      }
		      ATH_MSG_DEBUG("HLTMuonMon: SA: good " << isGoodCB << " nPix " << nPixcomb << " nSCT " << nSCTcomb
			  << " z0 " << z0comb - (vtx->position()).z() << " d0 " << d0comb); 
		    }
		  }
		}

		//if( fabs(d0) > ZERO_LIMIT || fabs(z0) > ZERO_LIMIT ) { // remove extrapolation-failed tracks, current value d0,z0 seem wrong
		if( fabs(eta) < SA_ETA_CUT &&  fabs(pt) > SA_PT_CUT && fabs(z0) < SA_Z0_CUT ) {
		  m_RecMuonSA_pt.push_back(pt);
		  m_RecMuonSA_eta.push_back(eta);
		  m_RecMuonSA_phi.push_back(phi);
		  m_RecMuonSA_d0.push_back(d0);
		  m_RecMuonSA_z0.push_back(z0);
		  m_RecMuonSA_charge.push_back(charge);
		  m_RecMuonSA_hasCB.push_back(hasCB);
		  m_RecMuonSA_isGoodCB.push_back(isGoodCB);
		  ATH_MSG_DEBUG("HLTMuonMon: SA: isgood " << isGoodCB << " pt " << pt);  
		}
		//}
	      }

	      // if((*muonItr)->isLowPtReconstructedMuon()) nOFLLPT++;  // YY method deprecated
	      if ((*muonItr)->muonType()==xAOD::Muon::SegmentTagged && (*muonItr)->inDetTrackParticleLink()!=0) nOFLLPT++;
	      if((*muonItr)->combinedTrackParticleLink()){
		ATH_MSG_DEBUG(" count CB muon");
		nOFLCB++;
		const xAOD::TrackParticle* comb=0;
		comb = *((*muonItr)->combinedTrackParticleLink());
		float pt = comb->pt() / CLHEP::GeV ;
		if( pt > 4 )
		  hist("Number_Of_Muid_Muons_4GeV_Cut",m_histdirrate)->Fill(m_lumiblock);
		if( pt > 10 )
		  hist("Number_Of_Muid_Muons_10GeV_Cut",m_histdirrate)->Fill(m_lumiblock);

		float eta = comb->eta();
		float phi = comb->phi();
		float charge = comb->charge();

		bool isGoodCB = false;
		const  Trk::Perigee measPerigee = comb->perigeeParameters();
		float d0comb = measPerigee.parameters()[Trk::d0];
		float z0comb = measPerigee.parameters()[Trk::z0];
		uint8_t nPixcomb = 0; 
		comb->summaryValue(nPixcomb, xAOD::numberOfPixelHits);
		uint8_t nSCTcomb = 0;
		comb->summaryValue(nSCTcomb, xAOD::numberOfSCTHits);

		if (vtx != 0) {
		  //if (nPixcomb >= 1 && nSCTcomb >= 6 && abs(z0comb - (vtx->position()).z()) < 5. && abs(d0comb) < 1.5) { } // Run 1 requirement
		  if ((*muonItr)->quality()<= xAOD::Muon::Medium && (*muonItr)->passesIDCuts()  && ((*muonItr)->author()==xAOD::Muon::Author::MuidCo || (*muonItr)->author()==xAOD::Muon::Author::STACO)) {
		    isGoodCB = true;
		  }
		  ATH_MSG_DEBUG("HLTMuonMon: CB: good " << isGoodCB << " nPix " << nPixcomb << " nSCT " << nSCTcomb
		      << " z0 " << z0comb - (vtx->position()).z() << " d0 " << d0comb); 
		}

		if( fabs(eta) < CB_ETA_CUT  &&  fabs(pt) > CB_PT_CUT ) {
		  m_RecMuonCB_pt.push_back(pt);
		  m_RecMuonCB_eta.push_back(eta);
		  m_RecMuonCB_phi.push_back(phi);
		  m_RecMuonCB_charge.push_back(charge);
		  m_RecMuonCB_isGoodCB.push_back(isGoodCB);
		  ATH_MSG_DEBUG("HLTMuonMon: CB: isgood " << isGoodCB << " pt " << pt); 
		  //m_RecMuonCB_eloss.push_back( (*muonItr)->energyLoss().first );
		  m_RecMuonCB_eloss.push_back( (*muonItr)->energyLossType() );
		  float ptcone20;
		  (*muonItr)->isolation(ptcone20, xAOD::Iso::ptcone20);
		  m_RecMuonCB_ptcone.push_back( ptcone20 );
		  float etcone20;
		  (*muonItr)->isolation(etcone20, xAOD::Iso::etcone20);
		  m_RecMuonCB_etcone.push_back( etcone20 );
		  float etcone30;
		  (*muonItr)->isolation(etcone30, xAOD::Iso::etcone30);
		  m_RecMuonCB_etcone30.push_back( etcone30 );
		  float ptcone30;
		  (*muonItr)->isolation(ptcone30, xAOD::Iso::ptcone30);
		  m_RecMuonCB_ptcone30.push_back( ptcone30 );

		  isCombined++;
		}
	      }
	    }

	    if(nOFL >10)       nOFL=10;
	    if(nOFLMS >10)     nOFLMS=10;
	    if(nOFLSA >10)     nOFLSA=10;
	    if(nOFLLPT >10)    nOFLLPT=10;
	    if(nOFLCB >10)     nOFLCB=10;
	    if(isCombined >10) isCombined=10;

	    ATH_MSG_DEBUG(" fill nMuon"); 
	    hist("Offline_Number_Of_Muon", m_histdir )->Fill(nOFL);
	    hist("Offline_Number_Of_MS_Muon", m_histdir )->Fill(nOFLMS);
	    hist("Offline_Number_Of_SA_Muon", m_histdir )->Fill(nOFLSA);
	    hist("Offline_Number_Of_LPT_Muon", m_histdir )->Fill(nOFLLPT);
	    hist("Offline_Number_Of_CB_Muon", m_histdir )->Fill(nOFLCB);
	    hist("Offline_Number_Of_Selected_CB_Muon", m_histdir )->Fill(isCombined);

	    if(nOFL)      hist("Common_Counter", m_histdir )->Fill((float)OFLFOUND);
	    if(nOFLMS)    hist("Common_Counter", m_histdir )->Fill((float)OFLMSFOUND);
	    if(nOFLSA)    hist("Common_Counter", m_histdir )->Fill((float)OFLSAFOUND);
	    if(nOFLLPT)   hist("Common_Counter", m_histdir )->Fill((float)OFLLPTFOUND);
	    if(nOFLCB)    hist("Common_Counter", m_histdir )->Fill((float)OFLCBFOUND);
	    if(isCombined) hist("Common_Counter", m_histdir )->Fill((float)OFLCBSELFOUND);


	    return StatusCode::SUCCESS;
	  }

	  //---------------------------------------------------------------------------------------
	  bool HLTMuonMonTool::isPassedES(std::vector<std::string>& vectES, std::string& ESChainName)
	  {
	    std::vector<std::string>::iterator it;
	    for (it = vectES.begin(); it != vectES.end(); it++) {
	      if (*it == ESChainName) {
		return true;
	      }
	    }
	    return false;
	  }

	  //---------------------------------------------------------------------------------------
	  std::vector<std::string> HLTMuonMonTool::getESbits()
	  {
	    static int errcnt = 0;

	    std::vector<std::string> retvect;
	    retvect.push_back(m_nullstr);
	    // Uncomment here for release!
	    /*  if (errcnt > 0) {
	    // 07.03.2011
	    // if already error, do nothing but returning null_terminated vector of string
	    // for saving time in storegate operation
	    return retvect;
	    } */ 

	    //
	    // Process current event
	    //
	   
      const EventInfo* event_handle(nullptr);
      if(StatusCode::FAILURE==evtStore() -> retrieve(event_handle)){
	if (errcnt < 1) {
	  ATH_MSG_DEBUG("Failed to read EventInfo");
	  errcnt++;
	}
	return retvect;
      }

      //
      // Print EventInfo and stream tags
      //
      const TriggerInfo *trig = event_handle->trigger_info();
      if(!trig) {
	if (errcnt < 1) {
	  ATH_MSG_DEBUG("Failed to get TriggerInfo");
	  errcnt++;
	}
	return retvect;
      }

	    const std::vector<TriggerInfo::StreamTag> &streams = trig->streamTags();

	    bool found_express_stream = false;
	    for(unsigned i = 0; i < streams.size(); ++i) {
	      const TriggerInfo::StreamTag &stag = streams.at(i);
	      if(stag.type() == "express" && stag.name() == "express") {
		found_express_stream = true;
		break;
	      }
	    }

	    if(!found_express_stream) {
	      if (errcnt < 1) {
		ATH_MSG_DEBUG("Failed to find express stream tag");
		errcnt++;
	      }
	      return retvect;
	    }

	    /*  ATH_MSG_INFO (">>>>>>>>>>>>>>>>"
		<< " run #" << event_handle->event_ID()->run_number()
		<< " lumi #" << event_handle->event_ID()->lumi_block()
		<< " event #" << event_handle->event_ID()->event_number() 
		<< " has express stream tag");  */

	    //const std::string key = "HLT_EXPRESS_OPI_HLT";
	    const std::string key = "HLT_TrigOperationalInfoCollection_EXPRESS_OPI_HLT";

	    if (!m_storeGate->contains<TrigOperationalInfoCollection>(key)) {
	      if (errcnt < 1) {
		ATH_MSG_INFO("Missing TrigOperationalInfoCollection with key=" << key);
		errcnt++;
	      }
	      return retvect;
	    }

	    const TrigOperationalInfoCollection *opi = 0;
	    if (!m_storeGate->retrieve<TrigOperationalInfoCollection>(opi, key).isSuccess()) {
	      if (errcnt < 1) {
		ATH_MSG_INFO("Failed to retreive TrigOperationalInfoCollection with key=" << key);
		return retvect;
	      }
	    }

	    std::vector<std::string> resvect;
	    if (opi->begin() == opi->end()) {
	      if (errcnt < 1) {
		ATH_MSG_DEBUG("getESbits: no entry");
		errcnt++;
	      }
	      resvect.push_back(m_nullstr); // set the vector that it is empty
	    } else {

	      for(TrigOperationalInfoCollection::const_iterator it = opi->begin(); it != opi->end(); ++it) {
		const TrigOperationalInfo *ptr = *it;
		if(!ptr) continue;

		const std::pair<std::vector<std::string>, std::vector<float> > infos = ptr->infos();
		resvect.insert(resvect.end(), infos.first.begin(), infos.first.end());

	      }
	    }

	    return resvect;
	  }


	  void
	    HLTMuonMonTool::fillTriggerOverlap()
	    {

	      TH2* h_ol    = hist2( "Trigger_Overlap", m_histdir   );
	      TH2* h_olpt  = hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   );

	      //Trigger
	      for(int i=0; i < (int)m_chainsOverlap.size(); i++ ){

		bool t1   = getTDT()->isPassed( m_chainsOverlap[i] );
		bool t1pt = getTDT()->isPassed( m_chainsOverlap[i], TrigDefs::passedThrough );

		if( !t1 && !t1pt )continue;

		for( int j=0; j < (int)m_chainsOverlap.size(); j++ ){

		  bool t2   = getTDT()->isPassed( m_chainsOverlap[j] );
		  bool t2pt = getTDT()->isPassed( m_chainsOverlap[j], TrigDefs::passedThrough );

		  if( !t2 && !t2pt ) continue;

		  //if both chains are passed or passed through
		  h_olpt->Fill(i,j);

		  //if both chains are passed
		  if( t1 && t2 ) h_ol->Fill(i,j);

		}
	      }

	    }

	  void
	    HLTMuonMonTool::procTriggerOverlap()
	    {



	      TH2* h_ol     = hist2( "Trigger_Overlap", m_histdir   );
	      TH2* h_olpt   = hist2( "Trigger_Overlap_Including_Passthrough", m_histdir   );

	      TH2* h_olf    = hist2( "Trigger_Overlap_Fraction", m_histdir   );
	      TH2* h_olfpt  = hist2( "Trigger_Overlap_Fraction_Including_Passthrough", m_histdir   );

	      //Trigger
	      for(int i=0; i < (int)m_chainsOverlap.size(); i++ ){

		Float_t diag   = h_ol->GetBinContent(i+1, i+1);
		Float_t diagpt = h_olpt->GetBinContent(i+1, i+1);

		for( int j=0; j < (int)m_chainsOverlap.size(); j++ ){

		  Float_t offdiag   = h_ol->GetBinContent(i+1, j+1);
		  Float_t offdiagpt = h_olpt->GetBinContent(i+1, j+1);

		  Float_t frac  = 0.;
		  Float_t efrac = 0.;

		  if( diag != 0. ){
		    frac = offdiag/diag;
		    efrac = sqrt(frac*(1.-frac)/diag);
		  }

		  Float_t fracpt  = 0.;
		  Float_t efracpt = 0.;

		  if( diagpt != 0. ){
		    fracpt  = offdiagpt/diagpt;
		    efracpt = sqrt(fracpt*(1.-fracpt)/diagpt);
		  }

		  h_olf->SetBinContent(i+1,j+1, frac);
		  h_olf->SetBinError(i+1,j+1, efrac);

		  h_olfpt->SetBinContent(i+1,j+1, fracpt);
		  h_olfpt->SetBinError(i+1,j+1, efracpt);

		}
	      }
	    }

	  //
	  void HLTMuonMonTool::refill_2d1d_coarse(TH2F *h2, TH1F *h1)
	  {

	    for (int ieta = 1; ieta <= s_eta_cnbins; ieta++) {
	      for (int iphi = 1; iphi <= s_phi_cnbins; iphi++) {
		int ibin = (ieta - 1) * s_phi_cnbins + iphi;
		// ATH_MSG_INFO("ibin " << ibin << "Histname " << h2->GetName() << " and " << h1->GetName());
		Double_t bincont = h2->GetBinContent(ieta, iphi);
		Double_t binerr = h2->GetBinError(ieta, iphi);
		Double_t area = (m_bins_eta[ieta] - m_bins_eta[ieta-1])*TMath::Pi()/8.;
		bincont = bincont / area;
		binerr = binerr / area;
		h1->SetBinContent(ibin, bincont);
		h1->SetBinError(ibin, binerr);
	      }
	    }

	  }
	  //

	  float CalculateDeltaR(float off_eta, float off_phi,float on_eta, float on_phi) {

	    float deta = off_eta - on_eta;
	    float dphi = off_phi - on_phi;
	    float dR=0;
	    if (dphi > acos(-1.)) dphi -= 2.*acos(-1.);
	    if (dphi < -acos(-1.)) dphi += 2.*acos(-1.);
	    dR = sqrt( deta*deta + dphi*dphi ) ;

	    return dR;
	  }

	  StatusCode HLTMuonMonTool::fillL1MuRoI()
	  {
	    StatusCode sc;

	    const xAOD::MuonRoIContainer* lvl1Roi;

	    std::string muonKey = "LVL1MuonRoIs"; // MuidMuonCollection

	    sc = m_storeGate->retrieve(lvl1Roi, muonKey);
	    if ( sc.isFailure() ) {
	      ATH_MSG_DEBUG(" Cannot retrieve LVL1 Muon container");
	      return StatusCode::SUCCESS;
	    }

	    const EventInfo* eventInfo;
	    int event, run;
	    if( StatusCode::SUCCESS == evtStore()->retrieve(eventInfo)){
	      event = eventInfo->event_ID()->event_number();
	      run = eventInfo->event_ID()->run_number();
	      ATH_MSG_DEBUG (" EventInfo : "   << " event: " << event << " run: " << run);

	    }else{
	      ATH_MSG_WARNING (" Unable to retrieve EventInfo from StoreGate ");
	      return StatusCode::SUCCESS;

	    }

	    EventID::number_type bcid = eventInfo->event_ID()->bunch_crossing_id();

	    bool filled = m_bunchTool->isFilled(bcid);
	    bool unpaired = m_bunchTool->isUnpaired(bcid);

	    xAOD::MuonRoIContainer::const_iterator muItr = lvl1Roi->begin(); 
	    xAOD::MuonRoIContainer::const_iterator muEnd = lvl1Roi->end(); 


	    //  if (m_bunchTool->isFilled(bcid) && !(m_bunchTool->isUnpaired(bcid))) {

	    if (m_bunchTool->isFilled(bcid)) {
	      std::string name = "L1RoI_etaphi";

	      int nBarrelRoi = 0;
	      int nEndcapRoi = 0;
	      int nForwardRoi = 0;
	      for ( ; muItr != muEnd; ++muItr) {
		hist2(name, m_histdirdist2d)->Fill((*muItr)->eta(), (*muItr)->phi());

		if (!(*muItr)->isVetoed()) { // YY for removing MuCTPI overlap-removed RoIs
		  if (xAOD::MuonRoI::Barrel == (*muItr)->getSource()) {
		    nBarrelRoi++;
		  } else if (xAOD::MuonRoI::Endcap == (*muItr)->getSource()) {
		    nEndcapRoi++;
		  } else if (xAOD::MuonRoI::Forward == (*muItr)->getSource()) {
		    nForwardRoi++;
		  }
		}
	      }

	      bool mub = false;
	      bool mue = false;
	      if (getTDT()->isPassedBits("L1_MUB") & TrigDefs::L1_isPassedBeforePrescale) {
		mub = true;
	      }
	      if (getTDT()->isPassedBits("L1_MUE") & TrigDefs::L1_isPassedBeforePrescale) {
		mue = true;
	      }

	      ATH_MSG_DEBUG("mu Roi Barrel: " << nBarrelRoi << " Endcap: " << nEndcapRoi << " Forward: " << nForwardRoi);
	      ATH_MSG_DEBUG("mub:           " << mub <<        " mue:    " << mue);

	      int nEC_F_Roi = nEndcapRoi + nForwardRoi;

	      name     = "L1_MUB_L1_MUE_match_muon_RoIs";

	      if (mub && nBarrelRoi > 0) {
		hist(name, m_histdirrate)->Fill(3.);
	      }
	      if (mue && nEC_F_Roi > 0) {
		hist(name, m_histdirrate)->Fill(4.);
	      }
	      if (!mub && nBarrelRoi == 0) {
		hist(name, m_histdirrate)->Fill(1.);
	      }
	      if (!mue && nEC_F_Roi == 0) {
		hist(name, m_histdirrate)->Fill(2.);
	      }
	      if (!mub && nBarrelRoi > 0) { // no L1_MUB hit
		hist(name, m_histdirrate)->Fill(5.);
		ATH_MSG_DEBUG("L1_MUB: 5");
		ATH_MSG_DEBUG("BCID " << bcid << " isFilled " << filled << " isUnpaired " << unpaired);
	      }
	      if (!mue && nEC_F_Roi > 0) { // no L1_MUE hit
		hist(name, m_histdirrate)->Fill(6.);
		ATH_MSG_DEBUG("L1_MUE: 6");
		ATH_MSG_DEBUG("BCID " << bcid << " isFilled " << filled << " isUnpaired " << unpaired);
	      }
	      if (mub && nBarrelRoi == 0) { // L1_MUB fired while no RoI
		hist(name, m_histdirrate)->Fill(7.);
		ATH_MSG_DEBUG("L1_MUB: 7");
		ATH_MSG_DEBUG("BCID " << bcid << " isFilled " << filled << " isUnpaired " << unpaired);
	      }
	      if (mue && nEC_F_Roi == 0) { // L1_MUE fired while no RoI
		hist(name, m_histdirrate)->Fill(8.);
		ATH_MSG_DEBUG("L1_MUE: 8");
		ATH_MSG_DEBUG("BCID " << bcid << " isFilled " << filled << " isUnpaired " << unpaired);
	      }
	    }


	    return StatusCode::SUCCESS;
	  }
