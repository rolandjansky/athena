/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitoring/HLTMETMonTool.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>

#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"

//___________________________________________________________________________________________________________
HLTMETMonTool::HLTMETMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : IHLTMonTool(type, name, parent),
    m_debuglevel(false) {

  // switches 
  declareProperty("PrintMETTriggerStats", m_is_print_met_trig_stats=false);
  declareProperty("MakeHistosForAllMETTriggers", m_is_make_histos_for_all_met_trig=false);
  declareProperty("MakeExpertHistos", m_is_make_expert_histos=false);
  declareProperty("MonPathBase", m_mon_path="/HLT/METMon");
  declareProperty("DoTriggerEfficiency", m_is_do_trigger_effic=true);

	  std::string str_array_1[]={"L1_XE20", "L1_XE50",        // l1 met triggers
	                             "L2_xe20", "L2_xe20_noMu",        // l2 met triggers
	                             "EF_xe20", "EF_xe20_noMu" };      // ef met triggers
	  std::string str_array_2[]={"EF_J10",                                // jet trigger
	                             "EF_MbSp",                 // minimum bias trigger
	                             "EF_mu6_rpc" };                             // other (physics?) trigger 
	  unsigned int size1 = sizeof(str_array_1)/sizeof(*str_array_1);
	  unsigned int size2 = sizeof(str_array_2)/sizeof(*str_array_2);
	  m_met_triggers.assign(str_array_1, str_array_1 + size1);
	  m_sample_selection_triggers.assign(str_array_2, str_array_2 + size2);
  declareProperty("METTriggers", m_met_triggers);
  declareProperty("SampleSelectionTriggers", m_sample_selection_triggers);

  declareProperty("LVL1_ROI_Key", m_lvl1_roi_key="LVL1_ROI");
  
  declareProperty("L2METKey", m_l2met_key="NoL2METKey");
  declareProperty("L2FEBKey", m_l2feb_key="NoL2FEBKey");
  declareProperty("EFMETKey", m_efmet_key="HLT_TrigEFMissingET");
  declareProperty("IsRecMETPlot", m_doRecMET=false);  
  declareProperty("RecMETKey", m_recmet_key="MET_RefFinal");  
  declareProperty("RecMETCaloKey", m_recmetcalo_key="MET_Calib");

  declareProperty("compNamesEF", m_compNamesEF);
  declareProperty("compNamesL2", m_compNamesL2);
  declareProperty("bitNamesL2", m_bitNamesL2);
  declareProperty("bitNamesEF", m_bitNamesEF);
}


//___________________________________________________________________________________________________________
HLTMETMonTool::~HLTMETMonTool() {
  *m_log << MSG::DEBUG << " deleting HLTMETMonTool " << endreq;
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::init() {

  // init message stream
  m_log->setLevel(outputLevel());
  m_debuglevel = (m_log->level() <= MSG::DEBUG);

  *m_log << MSG::DEBUG << "HLTMETMonTool : in initialize()" << endreq;

  if (m_debuglevel) {
    *m_log << MSG::DEBUG << "HLTMETMonTool : L1Key  = " << m_lvl1_roi_key << endreq;
    *m_log << MSG::DEBUG << "HLTMETMonTool : L2METKey = " << m_l2met_key << endreq;
    *m_log << MSG::DEBUG << "HLTMETMonTool : L2FEBKey = " << m_l2feb_key << endreq;
    *m_log << MSG::DEBUG << "HLTMETMonTool : EFMETKey = " << m_efmet_key << endreq;
  }

  // point to the signatures we want to make histos for
  if (m_is_make_histos_for_all_met_trig) {
    m_l1_met_signatures_tolook = &m_all_l1_met_triggers;
    m_l2_met_signatures_tolook = &m_all_l2_met_triggers;
    m_ef_met_signatures_tolook = &m_all_ef_met_triggers;
  } else {
    m_l1_met_signatures_tolook = &m_l1_met_signatures;
    m_l2_met_signatures_tolook = &m_l2_met_signatures;
    m_ef_met_signatures_tolook = &m_ef_met_signatures;
  }

  return StatusCode::SUCCESS;
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::book() {
  if (m_debuglevel) *m_log << MSG::DEBUG << "HLTMETMonTool : in book()" << endreq;

//  if(isNewRun) { // do these only at a new run - now in ManagedMonitorTool class
    // get all met triggers
    if(m_is_make_histos_for_all_met_trig) // get all met trigger items
      getAllMETTriggers();  
    else { // met triggers specified in the input list
      if(m_met_triggers.size()) {
        // check if input met triggers are in trigger configuration
        // put the correct met triggers in m_l*_met_signatures and so on
        checkTriggers(m_met_triggers, true);
      }
    }

    // sample selection triggers specified in the input list
    if(m_sample_selection_triggers.size()) {
      // check if input sample selection triggers are in triggger configuration
      // put the correct met triggers in m_sample_selection_signatures
      checkTriggers(m_sample_selection_triggers, false);
    }
//  }

  // book histograms
  // LS 24 Jan 2014 - No need for interval checking as this is now handled in ManagedMonitorTool class
//  if (isNewEventsBlock) {
    //std::string lbnMETMonGroup = m_mon_path + "/lbnstatus";
    ////addMonGroup (new MonGroup(this, lbnMETMonGroup.c_str(), shift, lumiBlock));
    //addMonGroup (new MonGroup(this, lbnMETMonGroup.c_str(), shift, eventsBlock));
    //setCurrentMonGroup(lbnMETMonGroup);
    //addEFStatusHistogram();
//  } // do nothing
  
//  if (isNewLumiBlock) {
    /*std::string lbnMETMonGroup = m_mon_path + "/lbnstatus";
    addMonGroup (new MonGroup(this, lbnMETMonGroup.c_str(), shift, lumiBlock));
    setCurrentMonGroup(lbnMETMonGroup);
    addEFStatusHistogram();
    addL2StatusHistogram();*/
//  }
  
//  if (isNewRun) { // do these only at a new run
    /** histogram limits */
    // et
    low[0] = -13.5;   high[0] = 601.5;  nbins[0] = 205;
    // sumet
    low[1] = -27.;    high[1] = 1803.;  nbins[1] = 305;
    // phi, dPhi
    low[2] = -3.1416; high[2] = 3.1416; nbins[2] = 32;
    // dEt, dEx, dEy
    low[3] = -100.;   high[3] = 100.;   nbins[3] = 100;


    bookHistograms_allStats();
    
    // for now all the histograms other than extension for online are considered expert histograms
    if (m_is_make_expert_histos) {
      if (m_l1_met_signatures_tolook->size())
        bookL1Histograms();
      if (m_l2_met_signatures_tolook->size())
        bookL2Histograms();
      if (m_ef_met_signatures_tolook->size())
        bookEFHistograms();
      if (m_is_do_trigger_effic)
        bookEfficHistograms();
    }


//  }

  return StatusCode::SUCCESS;
} 


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fill() {
  if (m_debuglevel) *m_log << MSG::DEBUG << "HLTMETMonTool :: in fill()" << endreq;

  trigger_decision();

  StatusCode sc = fillMETHistos();
  if (sc.isFailure()) {
    *m_log << MSG::WARNING << "The method fillMETHistos(...) failed" << endreq;
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::proc() {
  if (m_debuglevel) *m_log << MSG::DEBUG << "HLTMETMonTool : in proc()" << endreq;

//  if(isEndOfEventsBlock){} // do nothing
//  if(isEndOfLumiBlock){} // do nothing
//  if(isEndOfRun) { // // do these only at the end of a run
    /*
    if(m_is_print_met_trig_stats)
      printMETTriggerStats();

    std::map<std::string, int>::const_iterator it_s;
    std::map<std::string, int>::const_iterator it;
    TH1 *m_RecMET_et_denom;
    TH1 *m_RecMET_phi_denom;
    TH1 *m_RecMET_et_num;
    TH1 *m_RecMET_phi_num;
    std::string levels[3] = {"L1", "L2", "EF"};
    std::vector<std::map<std::string,int>*> m_met_signatures_tolook;
    m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
    m_met_signatures_tolook.push_back(m_l2_met_signatures_tolook);
    m_met_signatures_tolook.push_back(m_ef_met_signatures_tolook);
    unsigned int size = m_met_signatures_tolook.size();


    if (m_is_make_expert_histos) {

      setCurrentMonGroup(m_mon_path + "/RecMissingET_allStats");
      m_RecMET_et_denom = hist("RecMET_et");
      m_RecMET_phi_denom = hist("RecMET_phi");
      m_RecMET_et_denom->Sumw2();
      m_RecMET_phi_denom->Sumw2();

      for (unsigned int i=0; i<size; i++) {
        for(it = m_met_signatures_tolook[i]->begin(); it != m_met_signatures_tolook[i]->end(); it++) {
          setCurrentMonGroup(m_mon_path + "/Expert/"+levels[i]+"METTrigger/"+it->first);
          m_RecMET_et_num = hist("RecMET_et");
          m_RecMET_phi_num = hist("RecMET_phi");
          m_RecMET_et_num->Sumw2();
          m_RecMET_phi_num->Sumw2();
          std::string et_effic_name = levels[i] + "_et_effic_wrt_RecMET";
          std::string phi_effic_name = levels[i] + "_phi_effic_wrt_RecMET";
          hist(et_effic_name)->Divide(m_RecMET_et_num, m_RecMET_et_denom);
          hist(phi_effic_name)->Divide(m_RecMET_phi_num, m_RecMET_phi_denom);
        }
      }
    }
    if (m_is_do_trigger_effic) {

      for(it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
        setCurrentMonGroup(m_mon_path + "/Expert/Efficiency/"+it_s->first);
        m_RecMET_et_denom = hist("RecMET_et");
        m_RecMET_phi_denom = hist("RecMET_phi");
        m_RecMET_et_denom->Sumw2();
        m_RecMET_phi_denom->Sumw2();

        for (unsigned int i=0; i<size; i++) {
          for(it = m_met_signatures_tolook[i]->begin(); it != m_met_signatures_tolook[i]->end(); it++) {
            setCurrentMonGroup(m_mon_path + "/Expert/Efficiency/"+it_s->first+"/"+it->first);
            m_RecMET_et_num = hist("RecMET_et");
            m_RecMET_phi_num = hist("RecMET_phi");
            m_RecMET_et_num->Sumw2();
            m_RecMET_phi_num->Sumw2();
            std::string et_effic_name = levels[i] + "_et_effic_wrt_RecMET";
            std::string phi_effic_name = levels[i] + "_phi_effic_wrt_RecMET";
            hist(et_effic_name)->Divide(m_RecMET_et_num, m_RecMET_et_denom);
            hist(phi_effic_name)->Divide(m_RecMET_phi_num, m_RecMET_phi_denom);
          }
        }
      }
    }
    */
//  } // endofrun block

  return StatusCode::SUCCESS;
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::bookHistograms_allStats() {

  
  bool doL2MET = (m_l2met_key != "NoL2METKey");
  if(doL2MET) {
    std::string L2mongroupName = m_mon_path + "/L2MissingET_Fex";
    addMonGroup (new MonGroup(this, L2mongroupName.c_str(), run));
    setCurrentMonGroup(L2mongroupName);
    addHistogram(new TH1F("L1_METx",    "LVL1 METx (GeV);METx (GeV)",         199, -298.5,  298.5));
    addHistogram(new TH1F("L1_METy",    "LVL1 METy (GeV);METy (GeV)",         199, -298.5,  298.5));
    addHistogram( new TH1F("L1_MEx_log","L1 Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
    addHistogram( new TH1F("L1_MEy_log","L1 Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
    addHistogram( new TH1F("L1_MET_log","L1 |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 40, -1.875, 4.125));
    addHistogram( new TH1F("L1_SumEt_log","L1 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",   40, -1.875, 4.125));

    addBasicL1Histograms();
    addHistogram(new TH1F("L2_METx",    "LVL2 METx (GeV);METx (GeV)",         199, -298.5,  298.5));
    addHistogram(new TH1F("L2_METy",    "LVL2 METy (GeV);METy (GeV)",         199, -298.5,  298.5));
    addHistogram( new TH1F("L2_mu_SumEt","L2 #mu SumEt (GeV);#mu SumEt (GeV)", 305, -27., 1803.));
    addHistogram( new TH1F("L2_MEx_log","L2 Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
    addHistogram( new TH1F("L2_MEy_log","L2 Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
    addHistogram( new TH1F("L2_MET_log","L2 |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 40, -1.875, 4.125));
    addHistogram( new TH1F("L2_SumEt_log","L2 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",   40, -1.875, 4.125));
    addHistogram( new TH1F("L2_mu_SumEt_log","L2 #mu Sum |E_{T}|;#mu log_{10}(SumE_{T}/GeV)", 40, -1.875, 4.125));
    addBasicL2Histograms();
    addMoreL2Histograms();
    addL2StatusHistogram();
    addL1vsL2Histograms();
  }

  bool doL2FEB = (m_l2feb_key != "NoL2FEBKey");
  if(doL2FEB) {
    std::string L2FEBmongroupName = m_mon_path + "/L2MissingET_FEB";
    addMonGroup (new MonGroup(this, L2FEBmongroupName.c_str(), run));
    setCurrentMonGroup(L2FEBmongroupName);
    addL2FEBHistograms();
  }
  std::string EFmongroupName = m_mon_path + "/EFMissingET_Fex";
  addMonGroup (new MonGroup(this, EFmongroupName.c_str(), run));
  setCurrentMonGroup(EFmongroupName);
  addHistogram(new TH1F("EF_MEx_lin",   "EF Missing E_{x};E_{x} (GeV)",                    199, -298.5,  298.5));
  addHistogram(new TH1F("EF_MEy_lin",   "EF Missing E_{y};E_{y} (GeV)",                    199, -298.5,  298.5));
  addBasicEFHistograms();
  addMoreEFHistograms();
  addEFStatusHistogram();
  addL1vsEFHistograms();
  addL2vsEFHistograms();

  /*  std::string RecmongroupName = m_mon_path + "/RecMissingET_allStats";
      addMonGroup (new MonGroup(this, RecmongroupName.c_str(), shift, run));
      setCurrentMonGroup(RecmongroupName);
      addBasicRecHistograms();
      addL1vsRecHistograms();
      addL2vsRecHistograms();
      addEFvsRecHistograms();
      */

  // DQFlags
  std::string DQFlagmongroupName = m_mon_path + "/DQFlags";
  addMonGroup (new MonGroup(this, DQFlagmongroupName.c_str(), run));
  setCurrentMonGroup(DQFlagmongroupName);
  addDQFlagHistograms();
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::bookXSCalibHistograms(int level) {
  // histograms to monitor the XS calibration by plotting
  // MEx resolution vs Sqrt(SumET) for zero-bias events

  // binning in sqrt(SumET)
  int nS = 100;
  float Swidth = 0.2; // 0.1 GeV^0.5/bin
  float Smin = 0;
  float Smax = Smin + Swidth*nS;
  // binning in MET
  int nM = 101;
  float Mwidth = 3;   // 3 GeV/bin
  float Mmin = -1.5;
  float Mmax = Mmin + Mwidth*nM;

  TString title=" MET vs #sqrt{SumET}";
  TString Slabel=" #sqrt{SumET/GeV}";
  TString Mlabel=" MET/GeV";


  // binning in MEx, MEy
  int nMx = 201;
  float Mxwidth = 3;   // 3 GeV/bin
  float Mxmin = -601.5;
  float Mxmax = Mxmin + Mxwidth*nMx;
  TString titleX=" MEx vs #sqrt{SumET}";
  TString MlabelX=" MEx/GeV";
  TString titleY=" MEy vs #sqrt{SumET}";
  TString MlabelY=" MEy/GeV";

  if(level == 1) {
    addHistogram( new TH2F("L1MEx_VS_SqrtSET_All",TString("L1")+titleX+" (All Events);L1"+Slabel+";L1"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("L1MEy_VS_SqrtSET_All",TString("L1")+titleY+" (All Events);L1"+Slabel+";L1"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("L1MET_VS_SqrtSET_All",TString("L1")+title+" (All Events);L1"+Slabel+";L1"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax) );
    addHistogram( new TH2F("L1MEx_VS_SqrtSET_RD0",TString("L1")+titleX+" (ZB Events);L1"+Slabel+";L1"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("L1MEy_VS_SqrtSET_RD0",TString("L1")+titleY+" (ZB Events);L1"+Slabel+";L1"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("L1MET_VS_SqrtSET_RD0",TString("L1")+title+" (ZB Events);L1"+Slabel+";L1"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax) );
  }
  if(level == 2) {
    addHistogram( new TH2F("L2MEx_VS_SqrtSET_All",TString("L2")+titleX+" (All Events);L2"+Slabel+";L2"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("L2MEy_VS_SqrtSET_All",TString("L2")+titleY+" (All Events);L2"+Slabel+";L2"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("L2MET_VS_SqrtSET_All",TString("L2")+title+" (All Events);L2"+Slabel+";L2"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax) );
    addHistogram( new TH2F("L2MEx_VS_SqrtSET_RD0",TString("L2")+titleX+" (ZB Events);L2"+Slabel+";L2"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("L2MEy_VS_SqrtSET_RD0",TString("L2")+titleY+" (ZB Events);L2"+Slabel+";L2"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("L2MET_VS_SqrtSET_RD0",TString("L2")+title+" (ZB Events);L2"+Slabel+";L2"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax) );
  }
  if(level == 3) {
    addHistogram( new TH2F("EFMEx_VS_SqrtSET_All",TString("EF")+titleX+" (All Events);EF"+Slabel+";EF"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("EFMEy_VS_SqrtSET_All",TString("EF")+titleY+" (All Events);EF"+Slabel+";EF"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("EFMET_VS_SqrtSET_All",TString("EF")+title+" (All Events);EF"+Slabel+";EF"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax) );
    addHistogram( new TH2F("EFMEx_VS_SqrtSET_RD0",TString("EF")+titleX+" (ZB Events);EF"+Slabel+";EF"+MlabelX,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("EFMEy_VS_SqrtSET_RD0",TString("EF")+titleY+" (ZB Events);EF"+Slabel+";EF"+MlabelY,nS,Smin,Smax,nMx,Mxmin,Mxmax) );
    addHistogram( new TH2F("EFMET_VS_SqrtSET_RD0",TString("EF")+title+" (ZB Events);EF"+Slabel+";EF"+Mlabel,nS,Smin,Smax,nM,Mmin,Mmax) );
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::bookL1Histograms() {
  std::string m_generic_path_trigmetmonitoring = m_mon_path;
  std::map<std::string,int>::const_iterator it;

  for (it = m_l1_met_signatures_tolook->begin(); it != m_l1_met_signatures_tolook->end(); it++) {
    std::string m_expert_path_trigmetmonitoring = m_generic_path_trigmetmonitoring + "/Expert/L1METTrigger/" + it->first;

    //declare a group of histograms
    addMonGroup(new MonGroup(this, m_expert_path_trigmetmonitoring, run));
    setCurrentMonGroup(m_expert_path_trigmetmonitoring);
    addBasicL1Histograms();
    addBasicEFHistograms();
    addBasicRecHistograms();
    addL1vsRecHistograms();
   
    // trigger efficiency histograms
    addHistogram(new TH1F("L1_et_effic_wrt_RecMET", "et_effic_wrt_RecMET_L1 ; Et", nbins[0],low[0],high[0]));
    addHistogram(new TH1F("L1_phi_effic_wrt_RecMET", "phi_effic_wrt_RecMET_L1 ; Phi", nbins[2],low[2],high[2]));

    if (m_debuglevel) {
      *m_log << MSG::DEBUG << "INSIDE HLTMETMonTool : " << m_expert_path_trigmetmonitoring
          << "/" << " Histograms" << " booked successfully" << endreq;
    }
  }
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::bookL2Histograms() {
  std::string m_generic_path_trigmetmonitoring = m_mon_path;
  std::map<std::string,int>::const_iterator it;

  for (it = m_l2_met_signatures_tolook->begin(); it != m_l2_met_signatures_tolook->end(); it++) {
    std::string m_expert_path_trigmetmonitoring = m_generic_path_trigmetmonitoring + "/Expert/L2METTrigger/" + it->first;

    //declare a group of histograms
    addMonGroup(new MonGroup(this, m_expert_path_trigmetmonitoring, run));
    setCurrentMonGroup(m_expert_path_trigmetmonitoring);
    addBasicL2Histograms();
    addBasicEFHistograms();
    addL1vsL2Histograms();
    addBasicRecHistograms();         
    addL2vsRecHistograms();
   
    // trigger efficiency histograms
    addHistogram(new TH1F("L2_et_effic_wrt_RecMET", "et_effic_wrt_RecMET_L2 ; Et", nbins[0],low[0],high[0]));
    addHistogram(new TH1F("L2_phi_effic_wrt_RecMET", "phi_effic_wrt_RecMET_L2 ; Phi", nbins[2],low[2],high[2]));

    if (m_debuglevel) {
      *m_log << MSG::DEBUG << "INSIDE HLTMETMonTool : " << m_expert_path_trigmetmonitoring
          << "/" << " Histograms" << " booked successfully" << endreq;
    }
  }
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::bookEFHistograms() {

  std::string m_generic_path_trigmetmonitoring = m_mon_path;

  std::map<std::string,int>::const_iterator it;

  for (it = m_ef_met_signatures_tolook->begin(); it != m_ef_met_signatures_tolook->end(); it++) {
    std::string m_expert_path_trigmetmonitoring = m_generic_path_trigmetmonitoring + "/Expert/EFMETTrigger/" + it->first;

    //declare a group of histograms
    addMonGroup(new MonGroup(this, m_expert_path_trigmetmonitoring, run));
    setCurrentMonGroup(m_expert_path_trigmetmonitoring);
    addBasicEFHistograms();
    addL1vsEFHistograms();
    addL2vsEFHistograms();
    addBasicRecHistograms();
    addEFvsRecHistograms();

    // trigger efficiency histograms
    addHistogram(new TH1F("EF_et_effic_wrt_RecMET", "et_effic_wrt_RecMET_EF ; Et", nbins[0],low[0],high[0]));
    addHistogram(new TH1F("EF_phi_effic_wrt_RecMET", "phi_effic_wrt_RecMET_EF ; Phi", nbins[2],low[2],high[2]));

    if (m_debuglevel) {
      *m_log << MSG::DEBUG << "INSIDE HLTMETMonTool : " << m_expert_path_trigmetmonitoring
          << "/" << " Histograms" << " booked successfully" << endreq;
    }
  }
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::bookEfficHistograms() {

  std::string m_generic_path_trigmetmonitoring = m_mon_path;
  std::map<std::string,int>::const_iterator it_s;
  std::map<std::string,int>::const_iterator it;

  for (it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
    std::string m_expert_path_trigmetmonitoring = m_generic_path_trigmetmonitoring + "/Expert/Efficiency/" + it_s->first;
    //declare a group of histograms
    addMonGroup(new MonGroup(this, m_expert_path_trigmetmonitoring, run));
    setCurrentMonGroup(m_expert_path_trigmetmonitoring);
    addBasicRecHistograms();  

    std::vector<std::map<std::string,int>*> m_met_signatures_tolook;
    m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
    m_met_signatures_tolook.push_back(m_l2_met_signatures_tolook);
    m_met_signatures_tolook.push_back(m_ef_met_signatures_tolook);
    unsigned int size = m_met_signatures_tolook.size();
    std::string levels[3] = {"L1", "L2", "EF"};

    for (unsigned int i=0; i<size; i++) {
      for (it = m_met_signatures_tolook[i]->begin(); it != m_met_signatures_tolook[i]->end(); it++) {
        std::string m_expert_path_trigmetmonitoring = m_generic_path_trigmetmonitoring + "/Expert/Efficiency/" + it_s->first + "/" + it->first;
        //declare a group of histograms
        addMonGroup(new MonGroup (this, m_expert_path_trigmetmonitoring, run));
        setCurrentMonGroup(m_expert_path_trigmetmonitoring);
        addBasicRecHistograms();
   
        // trigger efficiency histograms
        std::string name = levels[i] + "_et_effic_wrt_RecMET";
        std::string title = "et_effic_wrt_RecMET_" + levels[i] + " ; Et";
        addHistogram(new TH1F(name.c_str(), title.c_str(), nbins[0],low[0],high[0]));
        name = levels[i] + "_phi_effic_wrt_RecMET";
        title = "phi_effic_wrt_RecMET_" + levels[i] + " ; Phi";
        addHistogram(new TH1F(name.c_str(), title.c_str(), nbins[2],low[2],high[2]));
    
        if (m_debuglevel) {
          *m_log << MSG::DEBUG << "INSIDE HLTMETMonTool : " << m_expert_path_trigmetmonitoring
              << "/" << " Histograms" << " booked successfully" << endreq;
        }
      } // end of loop over l* met signatures to look
    } // end of loop over levels
  } // end of loop over m_sample_selection_triggers
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addBasicL1Histograms() {
  // L1MET
  addHistogram(new TH1F("L1_MET",     "LVL1 MET (GeV);MET (GeV)",           nbins[0], low[0], high[0]));
  addHistogram(new TH1F("L1_MET_lin1",  "LVL1 |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)",      110,-0.5,10.5));
  addHistogram(new TH1F("L1_SumEt",   "LVL1 SumEt (GeV);SumEt (GeV)",       nbins[1], low[1], high[1]));
  addHistogram(new TH1F("L1_MET_phi", "LVL1 MET #phi (rad);MET #phi (rad)", nbins[2], low[2], high[2]));
  addHistogram(new TH1F("L1_MET_phi1", "LVL1 MET #phi(|Missing E_{T}|);MET #phi (rad)", nbins[2], low[2], high[2]));
  bookXSCalibHistograms(1);
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addBasicL2Histograms() {
  // L2MET
  addHistogram(new TH1F("L2_MET",     "LVL2 MET (GeV);MET (GeV)",           nbins[0], low[0], high[0]));
  addHistogram(new TH1F("L2_MET_lin1",  "LVL2 |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)",      110,-0.5,10.5));
  addHistogram(new TH1F("L2_SumEt",   "LVL2 SumEt ;SumEt (GeV)",       nbins[1], low[1], high[1]));
  addHistogram(new TH1F("L2_MET_phi", "LVL2 MET #phi ;MET #phi (rad)", nbins[2], low[2], high[2]));
  addHistogram(new TH1F("L2_MET_phi1", "LVL2 #phi(|Missing E_{T}|) ;MET #phi (rad)", nbins[2], low[2], high[2]));
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addMoreL2Histograms() {
  addHistogram(new TH1F("L2_MEx_log", "L2 Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("L2_MEy_log", "L2 Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("L2_MET_log", "L2 |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 40, -1.875, 4.125));
  addHistogram(new TH1F("L2_SumEt_log","L2 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",   40, -1.875, 4.125));

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL2StatusHistogram() {
  TH1F *h1i = new TH1F("L2_MET_status", "L2 MET Status",32, -0.5, 31.5);
  for (size_t j = 0; j < m_bitNamesL2.size(); j++) {
    if(j < m_bitNamesL2.size()) h1i->GetXaxis()->SetBinLabel(j+1, m_bitNamesL2[j].c_str());
  }
  addHistogram(h1i);
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addBasicEFHistograms() {
  addHistogram(new TH1F("EF_MET_lin",   "EF |Missing E_{T}|;ME_{T} (GeV)",                 nbins[0], low[0], high[0]));
  addHistogram(new TH1F("EF_MET_lin1",  "EF |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)",      110,-0.5,10.5));
  addHistogram(new TH1F("EF_SumEt_lin", "EF Sum |E_{T}|;SumE_{T} (GeV)",                   nbins[1], low[1], high[1]));
  addHistogram(new TH1F("EF_MET_phi",   "EF MET #phi (rad);#phi (rad)",                  nbins[2], low[2], high[2]));
  addHistogram(new TH1F("EF_MET_phi1",   "EF MET #phi(|Missing E_{T}|);#phi (rad)",                  nbins[2], low[2], high[2]));
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addDQFlagHistograms() {

  float fMaxEFC = float(m_compNamesEF.size());
  fMaxEFC -= 0.5;
  int fBinEFC = int(m_compNamesEF.size());
  TH2F *h2 = new TH2F("compN_compSumEt_lin",   "EF Sum |E_{T}| VS component;;SumE_{T} (GeV)",fBinEFC, -0.5, fMaxEFC, 236, -30,   2802);
  TH2F *h2f = new TH2F("compN_EF_MET_status", "EF MET Status VS component;;",fBinEFC, -0.5, fMaxEFC,32, -0.5, 31.5);
  for (size_t cn = 0; cn < m_compNamesEF.size(); cn++) { // 25 bins
    if(cn < m_compNamesEF.size()) h2->GetXaxis()->SetBinLabel(cn+1, m_compNamesEF[cn].c_str());
    if(cn < m_compNamesEF.size()) h2f->GetXaxis()->SetBinLabel(cn+1, m_compNamesEF[cn].c_str());
  }
  addEFStatusHistogram();
  addL2StatusHistogram();
  addMoreL2Histograms();
  addBasicEFHistograms();
  
  addHistogram( new TH1F("EF_MEx_log",   "EF Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
  addHistogram( new TH1F("EF_MEy_log",   "EF Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
  for (size_t j = 0; j < 32; j++) {
    if(j < m_bitNamesEF.size()) h2f->GetYaxis()->SetBinLabel(j+1, m_bitNamesEF[j].c_str());
  }
  addHistogram(h2f); 
  addHistogram(h2);

  //TH2I *h2i = new TH2I("trmet_lbn_flag",   "Lumi-block based flags;lbn;lbn",101,-0.5,100.5, 101, -0.5, 100.5);
  //h2i->SetBit(TH1::kCanRebin);
  //addHistogram(h2i);
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL2FEBHistograms() {
 
  // histogram definitions
  //signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
  //central bin is +/- 0.075 and contains values whose abs < 1.189 GeV
  //limits are +/- 1.189 TeV
  addHistogram( new TH1F("L2FEB_MEx_lin",   "L2FEB Missing E_{x};E_{x} (GeV)",                    100, -298.5,  298.5));
  addHistogram( new TH1F("L2FEB_MEx_log",   "L2FEB Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 27, -4.125, 4.125));  
  
  addHistogram(new TH1F("L2FEB_MEy_lin",   "L2FEB Missing E_{y};E_{y} (GeV)",                    100, -298.5,  298.5));
  addHistogram( new TH1F("L2FEB_MEy_log",   "L2FEB Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 27, -4.125, 4.125));

  addHistogram( new TH1F("L2FEB_MEz_lin",    "L2FEB Missing E_{z};ME_{z} (GeV)",100, -298.5,298.5));
  addHistogram( new TH1F("L2FEB_MEz_log",    "L2FEB Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)",27, -4.125, 4.125));

  addHistogram( new TH1F("L2FEB_ME_lin",     "L2FEB Missing Energy;ME (GeV)",103, -13.5,601.5));
  addHistogram( new TH1F("L2FEB_ME_log",     "L2FEB Missing Energy;log_{10}(ME/GeV)", 24, -0.15, 7.05));

  addHistogram(new TH1F("L2FEB_MET_lin",   "L2FEB |Missing E_{T}|;ME_{T} (GeV)",                 nbins[0], low[0], high[0]));
  addHistogram(new TH1F("L2FEB_MET_lin1",  "L2FEB |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)",      55,-0.5,10.5));  
  addHistogram(new TH1F("L2FEB_SumEt_lin", "L2FEB Sum |E_{T}|;SumE_{T} (GeV)",                   nbins[1], low[1], high[1]));
  addHistogram(new TH1F("L2FEB_MET_phi",   "L2FEB MET #phi (rad);#phi (rad)",                  nbins[2], low[2], high[2]));  
  addHistogram(new TH1F("L2FEB_MET_phi1",   "L2FEB MET #phi(|Missing E_{T}|);#phi (rad)",                  nbins[2], low[2], high[2])); 

  float fMaxL2C = float(m_compNamesL2.size());
  fMaxL2C -= 0.5;
  int fBinL2C = int(m_compNamesL2.size());

  addHistogram(new TH1F("L2FEB_MET_log",   "L2FEB |Missing E_{T}|;log_{10}(ME_{T}/GeV)",         20, -1.875, 4.125));
  addHistogram(new TH1F("L2FEB_SumEt_log", "L2FEB Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",           20, -1.875, 4.125));

  addHistogram( new TH1F("L2FEB_SumE_lin",   "L2FEB Sum |E|;SumE (GeV)",152, -27.,1803.));
  addHistogram( new TH1F("L2FEB_SumE_log",   "L2FEB Sum |E|;log_{10}(SumE/GeV)",20, -1.875, 4.125));

  // L2FEB MET all components in one 2d histogram
  TH2F *h2f[10] = {0};
  h2f[0] = new TH2F("compN_compEx",  "L2FEB MissingE_{x} VS component;;sgn(ME_{x}) log_{10}(E_{x}/GeV)", fBinL2C, -0.5, fMaxL2C, 23, -4.125, 4.125);
  h2f[1] = new TH2F("compN_compEy",  "L2FEB Missing E_{y} VS component;;sgn(ME_{y}) log_{10}(E_{y}/GeV)",fBinL2C, -0.5, fMaxL2C, 23, -4.125, 4.125);
  h2f[2] = new TH2F("compN_compEz",  "L2FEB Missing E_{z} VS component;;sgn(ME_{z}) log_{10}(E_{z}/GeV)",fBinL2C, -0.5, fMaxL2C, 23, -4.125, 4.125);
  h2f[3] = new TH2F("compN_compEt",  "L2FEB MissingE_{T} VS component;;sgn(ME_{T}) log_{10}(ME_{T}/GeV)",fBinL2C, -0.5, fMaxL2C, 20, -1.875, 4.125);
  h2f[4] = new TH2F("compN_compSumEt",   "L2FEB Sum |E_{T}| VS component;;log_{10}(SumE_{T}/GeV)",fBinL2C, -0.5, fMaxL2C, 20, -1.875, 4.125);
  h2f[5] = new TH2F("compN_compSumE",    "L2FEB Sum |E| VS component;;log_{10}(SumE/GeV)",fBinL2C, -0.5, fMaxL2C, 20, -1.875, 4.125);
  h2f[6] = new TH2F("compN_compEt_lin",  "L2FEB MissingE_{T} VS component;;ME_{T} (GeV)",fBinL2C, -0.5, fMaxL2C, 102, -13.5, 601.5);
  h2f[7] = new TH2F("compN_compSumEt_lin",   "L2FEB Sum |E_{T}| VS component;;SumE_{T} (GeV)",fBinL2C, -0.5, fMaxL2C, 236, -30,   2802); 
  h2f[8] = new TH2F("compN_L2FEB_MET_status", "L2FEB MET Status VS component;;",fBinL2C, -0.5, fMaxL2C,32, -0.5, 31.5);
  h2f[9] = new TH2F("compN_L2FEB_usedChannels", "L2FEB Used Channels VS component;;log_{10}(N > 0)",fBinL2C, -0.5, fMaxL2C,13,-0.5,6.0);

  for (size_t k = 0; k < 10; k++) { // 10 hists
    for (size_t cn = 0; cn < m_compNamesL2.size(); cn++) { // 15 bins
      if(cn < m_compNamesL2.size()) h2f[k]->GetXaxis()->SetBinLabel(cn+1, m_compNamesL2[cn].c_str());
    }
    // for status v. component, set bin labels for Y axis
    if(k == 8) {
      for (size_t j = 0; j < m_bitNamesEF.size(); j++) { // 32 bins
        if(j < m_bitNamesEF.size()) h2f[k]->GetYaxis()->SetBinLabel(j+1, m_bitNamesEF[j].c_str());
      }
      h2f[k]->GetYaxis()->SetLabelFont(42);
      h2f[k]->GetYaxis()->SetLabelOffset(0.002);
      h2f[k]->GetYaxis()->SetLabelSize(0.03);
    }
    if(k==9){
      // 10^x (x = 0.5, 1.0 1.5 ....)
      /*std::string binlb_nc[13] = { "none", "[0-3]", "[4-9]", "[10-31]", 
        "[32-99]", "[100-316]", "[317-999]", "[1-3.16]10^{3}", "[0.32 - 1]10^{4}", 
        "[1 - 3.2]10^{4}", "[0.32 - 1]10^{5}", "[1 - 3.2]10^{5}", "[0.32 - 1]10^{6}" };
      for(size_t l = 0; l < 13; l++) {*/
        h2f[k]->GetYaxis()->SetBinLabel(1,"none");
      //}

    }
    addHistogram(h2f[k]);
  }

  int phibins = 16; double phimin = low[2]; double phimax = high[2];
  int etabins = 24; double etamin = -4.8;   double etamax =  4.8;



  for (unsigned int j = 0; j < m_compNamesL2.size(); j++) {
    std::string name = Form("compN_EtaPhi_%02d", j);
    std::string title = m_compNamesL2[j];
    title += ": N(#eta, #phi);#eta;#phi [rad];";
    TH2F* h = new TH2F(name.c_str(), title.c_str(), etabins,etamin,etamax, phibins,phimin,phimax);
    addHistogram(h);

    name = Form("compEt_lin_EtaPhi_%02d", j);
    title = m_compNamesL2[j];
    title += ": MissingE_{T}(#eta, #phi); #eta; #phi [rad];ME_{T}/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins,etamin,etamax, phibins,phimin,phimax);
    addHistogram(h);
    
    name = Form("compSumEt_lin_EtaPhi_%02d", j);
    title = m_compNamesL2[j];
    title += ": Sum |E_{T}|(#eta, #phi); #eta; #phi [rad];Sum|E_{T}|/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins,etamin,etamax, phibins,phimin,phimax);
    addHistogram(h);
    
    name = Form("compSumE_lin_EtaPhi_%02d", j);
    title = m_compNamesL2[j];
    title += ": Sum |E|(#eta, #phi); #eta; #phi [rad];Sum|E|/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins,etamin,etamax, phibins,phimin,phimax);
    addHistogram(h);
    
  }
  bookXSCalibHistograms(2);

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addMoreEFHistograms() {
  
  // histogram definitions
  //signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
  //central bin is +/- 0.075 and contains values whose abs < 1.189 GeV
  //limits are +/- 1.189 TeV
  addHistogram( new TH1F("EF_MEx_log",   "EF Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 27, -4.125, 4.125));
  addHistogram( new TH1F("EF_MEy_log",   "EF Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 27, -4.125, 4.125));
  
  addHistogram( new TH1F("EF_MEz_lin",    "EF Missing E_{z};ME_{z} (GeV)",100, -298.5,298.5));
  addHistogram( new TH1F("EF_MEz_log",    "EF Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)",27, -4.125, 4.125));

  addHistogram( new TH1F("EF_ME_lin",     "EF Missing Energy;ME (GeV)",102, -13.5,601.5));
  addHistogram( new TH1F("EF_ME_log",     "EF Missing Energy;log_{10}(ME/GeV)", 24, -0.15, 7.05));

  // signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
  // bin centered on zero is +/- 0.075 and contains values whose abs < 1.189 GeV
  // limits are -74.99 GeV and 2.371 TeV
  addHistogram(new TH1F("EF_MET_log",   "EF |Missing E_{T}|;log_{10}(ME_{T}/GeV)",         20, -1.875, 4.125));
  addHistogram(new TH1F("EF_SumEt_log", "EF Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",           20, -1.875, 4.125));

  addHistogram( new TH1F("EF_SumE_lin",   "EF Sum |E|;SumE (GeV)",153, -27.,1803.));
  addHistogram( new TH1F("EF_SumE_log",   "EF Sum |E|;log_{10}(SumE/GeV)",20, -1.875, 4.125));

  // EF MET all components in one 2d histogram
  float fMaxEFC = float(m_compNamesEF.size());
  fMaxEFC -= 0.5;
  int fBinEFC = int(m_compNamesEF.size());
  TH2F *h2f[10] = {0};
  h2f[0] = new TH2F("compN_compEx",  "EF MissingE_{x} VS component;;sgn(ME_{x}) log_{10}(E_{x}/GeV)", fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[1] = new TH2F("compN_compEy",  "EF Missing E_{y} VS component;;sgn(ME_{y}) log_{10}(E_{y}/GeV)",fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[2] = new TH2F("compN_compEz",  "EF Missing E_{z} VS component;;sgn(ME_{z}) log_{10}(E_{z}/GeV)",fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[3] = new TH2F("compN_compEt",  "EF MissingE_{T} VS component;;sgn(ME_{T}) log_{10}(ME_{T}/GeV)",fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[4] = new TH2F("compN_compSumEt",   "EF Sum |E_{T}| VS component;;log_{10}(SumE_{T}/GeV)",fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[5] = new TH2F("compN_compSumE",    "EF Sum |E| VS component;;log_{10}(SumE/GeV)",fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[6] = new TH2F("compN_compEt_lin",  "EF MissingE_{T} VS component;;ME_{T} (GeV)",fBinEFC, -0.5, fMaxEFC, 102, -13.5, 601.5);
  h2f[7] = new TH2F("compN_compSumEt_lin",   "EF Sum |E_{T}| VS component;;SumE_{T} (GeV)",fBinEFC, -0.5, fMaxEFC, 236, -30,   2802); 
  h2f[8] = new TH2F("compN_EF_MET_status", "EF MET Status VS component;;",fBinEFC, -0.5, fMaxEFC,32, -0.5, 31.5);
  h2f[9] = new TH2F("compN_EF_usedChannels", "EF Used Channels VS component;;log_{10}(N > 0)",fBinEFC, -0.5, fMaxEFC,13,-0.5,6.0);


  for (size_t k = 0; k < 10; k++) { // 10 hists
    for (size_t cn = 0; cn < m_compNamesEF.size(); cn++) { // 25 bins
      if(cn < m_compNamesEF.size()) h2f[k]->GetXaxis()->SetBinLabel(cn+1, m_compNamesEF[cn].c_str());
    }
    // for status v. component, set bin labels for Y axis
    if(k == 8) {
      for (size_t j = 0; j < m_bitNamesEF.size(); j++) {
        if(j < m_bitNamesEF.size()) h2f[k]->GetYaxis()->SetBinLabel(j+1, m_bitNamesEF[j].c_str());
      }
      h2f[k]->GetYaxis()->SetLabelFont(42);
      h2f[k]->GetYaxis()->SetLabelOffset(0.002);
      h2f[k]->GetYaxis()->SetLabelSize(0.03);
    }
    if(k==9){
      // 10^x (x = 0.5, 1.0 1.5 ....)
      /*std::string binlb_nc[13] = { "none", "[0-3]", "[4-9]", "[10-31]", 
        "[32-99]", "[100-316]", "[317-999]", "[1-3.16]10^{3}", "[0.32 - 1]10^{4}", 
        "[1 - 3.2]10^{4}", "[0.32 - 1]10^{5}", "[1 - 3.2]10^{5}", "[0.32 - 1]10^{6}" };
      for(size_t l = 0; l < 13; l++) {*/
        h2f[k]->GetYaxis()->SetBinLabel(1,"none");
      //}

    }
    addHistogram(h2f[k]);
  }

  int phibins = 16; double phimin = low[2]; double phimax = high[2];
  int etabins = 24; double etamin = -4.8;   double etamax =  4.8;



  for (unsigned int j = 0; j < m_compNamesEF.size(); j++) {
    std::string name = Form("compN_EtaPhi_%02d", j);
    std::string title = m_compNamesEF[j];
    title += ": N(#eta, #phi);#eta;#phi [rad];";
    TH2F* h = new TH2F(name.c_str(), title.c_str(), etabins,etamin,etamax, phibins,phimin,phimax);
    addHistogram(h);

    name = Form("compEt_lin_EtaPhi_%02d", j);
    title = m_compNamesEF[j];
    title += ": MissingE_{T}(#eta, #phi); #eta; #phi [rad];ME_{T}/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins,etamin,etamax, phibins,phimin,phimax);
    addHistogram(h);
    
    name = Form("compSumEt_lin_EtaPhi_%02d", j);
    title = m_compNamesEF[j];
    title += ": Sum |E_{T}|(#eta, #phi); #eta; #phi [rad];Sum|E_{T}|/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins,etamin,etamax, phibins,phimin,phimax);
    addHistogram(h);
    
    name = Form("compSumE_lin_EtaPhi_%02d", j);
    title = m_compNamesEF[j];
    title += ": Sum |E|(#eta, #phi); #eta; #phi [rad];Sum|E|/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins,etamin,etamax, phibins,phimin,phimax);
    addHistogram(h);
    
  }
  bookXSCalibHistograms(3);

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addEFStatusHistogram() {
  TH1F *h1i = new TH1F("EF_MET_status", "EF MET Status",32, -0.5, 31.5);
  for (size_t j = 0; j < m_bitNamesEF.size(); j++) {
   if(j < m_bitNamesEF.size()) h1i->GetXaxis()->SetBinLabel(j+1, m_bitNamesEF[j].c_str());
  }
  addHistogram(h1i);
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addBasicRecHistograms() {
  // RecMET
  // et sumet phi histograms
  addHistogram(new TH1F("RecMET_et", "et_RecMET ; Et", nbins[0],low[0],high[0]));
  addHistogram(new TH1F("RecMET_sumet", "sumet_RecMET ; sumEt", nbins[1],low[1],high[1]));
  addHistogram(new TH1F("RecMET_phi", "phi_RecMET ; Phi", nbins[2],low[2],high[2]));
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1vsL2Histograms() {

  addHistogram(new TH2F("L1MET_L2MET_EtCor", "LVL1 vs LVL2 Missing E_{T} ; L2ME_{T} (GeV); L1ME_{T} (GeV)", nbins[0],low[0],high[0], nbins[0],low[0],high[0]));
  addHistogram(new TH2F("L1MET_L2MET_SumEtCor", "LVL1 vs LVL2 Sum |E_{T}| ; L2Sum|E_{T}| (GeV); L1Sum|E_{T}| (GeV)", nbins[1],low[1],high[1], nbins[1],low[1],high[1]));
  addHistogram(new TH2F("L1MET_L2MET_PhiCor", "LVL1 vs LVL2 #phi ; LVL2 #phi (rad); LVL1 #phi (rad)", nbins[2],low[2],high[2], nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L1MET_L2MET_dEt", "#DeltaE_{T}(LVL1-LVL2) ; #DeltaE_{T} (GeV)", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L1MET_L2MET_dPhi", "#Delta#phi(LVL1-LVL2) ; #Delta#phi (GeV)", nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L1MET_L2MET_dEx", "#DeltaE_{x}(LVL1-LVL2) ; #DeltaE_{x} (GeV)", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L1MET_L2MET_dEy", "#DeltaE_{y}(LVL1-LVL2) ; #DeltaE_{y} (GeV)", nbins[3],low[3],high[3]));
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1vsEFHistograms() {

  addHistogram(new TH2F("L1MET_EFMET_EtCor", "LVL1 vs EF Missing E_{T} ; EFME_{T} (GeV); L1ME_{T} (GeV)", nbins[0],low[0],high[0], nbins[0],low[0],high[0]));
  addHistogram(new TH2F("L1MET_EFMET_SumEtCor", "LVL1 vs EF Sum |E_{T}| ; EFSum|E_{T}| (GeV); L1Sum|E_{T}| (GeV)", nbins[1],low[1],high[1], nbins[1],low[1],high[1]));
  addHistogram(new TH2F("L1MET_EFMET_PhiCor", "LVL1 vs EF #phi ; EF #phi (rad); LVL1 #phi (rad)", nbins[2],low[2],high[2], nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L1MET_EFMET_dEt", "#DeltaE_{T}(LVL1-EF) ; #DeltaE_{T} (GeV)", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L1MET_EFMET_dPhi", "#Delta#phi(LVL1-EF) ; #Delta#phi (GeV)", nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L1MET_EFMET_dEx", "#DeltaE_{x}(LVL1-EF) ; #DeltaE_{x} (GeV)", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L1MET_EFMET_dEy", "#DeltaE_{y}(LVL1-EF) ; #DeltaE_{y} (GeV)", nbins[3],low[3],high[3]));

}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addL2vsEFHistograms() {

  addHistogram(new TH2F("L2MET_EFMET_EtCor", "LVL2 vs EF Missing E_{T} ; EFME_{T} (GeV); L2ME_{T} (GeV)", nbins[0],low[0],high[0], nbins[0],low[0],high[0]));
  addHistogram(new TH2F("L2MET_EFMET_SumEtCor", "LVL2 vs EF Sum |E_{T}| ; EFSum|E_{T}| (GeV); L2Sum|E_{T}| (GeV)", nbins[1],low[1],high[1], nbins[1],low[1],high[1]));
  addHistogram(new TH2F("L2MET_EFMET_PhiCor", "LVL2 vs EF #phi ; EF #phi (rad); LVL2 #phi (rad)", nbins[2],low[2],high[2], nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L2MET_EFMET_dEt", "#DeltaE_{T}(LVL2-EF) ; #DeltaE_{T} (GeV)", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L2MET_EFMET_dPhi", "#Delta#phi(LVL2-EF) ; #Delta#phi (GeV)", nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L2MET_EFMET_dEx", "#DeltaE_{x}(LVL2-EF) ; #DeltaE_{x} (GeV)", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L2MET_EFMET_dEy", "#DeltaE_{y}(LVL2-EF) ; #DeltaE_{y} (GeV)", nbins[3],low[3],high[3]));
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1vsRecHistograms() {

  addHistogram(new TH2F("L1MET_RecMET_EtCor", "EtCor_L1MET_RecMET ; RecMET_Et ; L1MET_Et", nbins[0],low[0],high[0], nbins[0],low[0],high[0]));
  addHistogram(new TH2F("L1MET_RecMET_SumEtCor", "SumEtCor_L1MET_RecMET ; RecMET_SumEt ; L1MET_SumEt", nbins[1],low[1],high[1], nbins[1],low[1],high[1]));
  addHistogram(new TH2F("L1MET_RecMET_PhiCor", "PhiCor_L1MET_RecMET ; RecMET_Phi ; L1MET_Phi", nbins[2],low[2],high[2], nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L1MET_RecMET_dEt", "dEt_L1MET_RecMET ; dEt", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L1MET_RecMET_dPhi", "dPhi_L1MET_RecMET ; dPhi", nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L1MET_RecMET_dEx", "dEx_L1MET_RecMET ; dEx", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L1MET_RecMET_dEy", "dEy_L1MET_RecMET ; dEy", nbins[3],low[3],high[3]));
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addL2vsRecHistograms() {

  addHistogram(new TH2F("L2MET_RecMET_EtCor", "EtCor_L2MET_RecMET ; RecMET_Et ; L2MET_Et", nbins[0],low[0],high[0], nbins[0],low[0],high[0]));
  addHistogram(new TH2F("L2MET_RecMET_SumEtCor", "SumEtCor_L2MET_RecMET ; RecMET_SumEt ; L2MET_SumEt", nbins[1],low[1],high[1], nbins[1],low[1],high[1]));
  addHistogram(new TH2F("L2MET_RecMET_PhiCor", "PhiCor_L2MET_RecMET ; RecMET_Phi ; L2MET_Phi", nbins[2],low[2],high[2], nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L2MET_RecMET_dEt", "dEt_L2MET_RecMET ; dEt", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L2MET_RecMET_dPhi", "dPhi_L2MET_RecMET ; dPhi", nbins[2],low[2],high[2]));
  addHistogram(new TH1F("L2MET_RecMET_dEx", "dEx_L2MET_RecMET ; dEx", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("L2MET_RecMET_dEy", "dEy_L2MET_RecMET ; dEy", nbins[3],low[3],high[3]));
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addEFvsRecHistograms() {

  addHistogram(new TH2F("EFMET_RecMET_EtCor", "EtCor_EFMET_RecMET ; RecMET_Et ; EFMET_Et", nbins[0],low[0],high[0], nbins[0],low[0],high[0]));
  addHistogram(new TH2F("EFMET_RecMET_SumEtCor", "SumEtCor_EFMET_RecMET ; RecMET_SumEt ; EFMET_SumEt", nbins[1],low[1],high[1], nbins[1],low[1],high[1]));
  addHistogram(new TH2F("EFMET_RecMET_PhiCor", "PhiCor_EFMET_RecMET ; RecMET_Phi ; EFMET_Phi", nbins[2],low[2],high[2], nbins[2],low[2],high[2]));
  addHistogram(new TH1F("EFMET_RecMET_dEt", "dEt_EFMET_RecMET ; dEt", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("EFMET_RecMET_dPhi", "dPhi_EFMET_RecMET ; dPhi", nbins[2],low[2],high[2]));
  addHistogram(new TH1F("EFMET_RecMET_dEx", "dEx_EFMET_RecMET ; dEx", nbins[3],low[3],high[3]));
  addHistogram(new TH1F("EFMET_RecMET_dEy", "dEy_EFMET_RecMET ; dEy", nbins[3],low[3],high[3]));   
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::trigger_decision() {

  std::vector<std::map<std::string,int>*> m_met_signatures_tolook;
  m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_l2_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_ef_met_signatures_tolook);

  if(m_is_print_met_trig_stats) {
    std::map<std::string,int>::iterator iter;
    for (unsigned int i = 0; i<m_met_signatures_tolook.size(); i++) {
      for ( iter = m_met_signatures_tolook[i]->begin(); iter != m_met_signatures_tolook[i]->end(); ++iter) {
        std::string name = iter->first;
        if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
          iter->second +=1;
        }
      }
    } 
  }
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillMETHistos() {

  if (m_debuglevel) *m_log << MSG::DEBUG << "in fillMETHistos()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // get LVL1 ROIs
  m_lvl1_roi = 0;
  sc = m_storeGate->retrieve(m_lvl1_roi, m_lvl1_roi_key);
  if( sc.isFailure() || !m_lvl1_roi ) {
    *m_log << MSG::DEBUG
       << "No LVL1 ROI with key \"" << m_lvl1_roi_key << "\" found in TDS"
       << endreq;
  }

  // get LVL1 MET ROI
  LVL1_ROI::energysums_type L1METROI; 
  if (m_lvl1_roi) {
    L1METROI = m_lvl1_roi->getEnergySumROIs();
    int l1metroi_size = L1METROI.size();
    if (l1metroi_size > 1) {
      *m_log << MSG::DEBUG << "Number of L1METROI's " << l1metroi_size << ", should be 1!" << endreq;
    }
    if (m_debuglevel) {
      *m_log << MSG::DEBUG << " Number of L1METROI's " << l1metroi_size
        << endreq;
    }
    if (l1metroi_size == 0) { 
      *m_log << MSG::DEBUG << "Could not retrieve L1 term" << endreq;
    }
  }

  // get L2 TrigMissingETContainer
  bool doL2MET = (m_l2met_key != "NoL2METKey");
  const TrigMissingETContainer *trigL2METcont = 0;
  if(doL2MET) {
    sc = m_storeGate->retrieve(trigL2METcont, m_l2met_key);
    if (sc.isFailure() || !trigL2METcont) {
      *m_log << MSG::DEBUG << "Could not retrieve TrigMissingETContainter with Key \""
        << m_l2met_key << "\" : trigL2METcont = 0" << endreq;
    } 
    else if (m_debuglevel) {
      *m_log << MSG::DEBUG << "Got TrigMissingETContainer with key \"" << m_l2met_key << "\"" << endreq;
      *m_log << MSG::DEBUG << "trigL2METcont = " << trigL2METcont << endreq;
    }
  }
  // get L2 MET
  const TrigMissingET *missETL2 = 0;
  if(doL2MET) {
    if (trigL2METcont) {
      TrigMissingETContainer::const_iterator trigMETfirst  = trigL2METcont->begin();
      TrigMissingETContainer::const_iterator trigMETlast = trigL2METcont->end();
      if (trigMETfirst != trigMETlast) {
        missETL2 = *trigMETfirst;
      }
    }
  }
 
  // get L2FEB TrigMissingETContainer
  bool doL2FEB = (m_l2feb_key != "NoL2FEBKey");
  const TrigMissingETContainer *trigL2FEBcont = 0;
  if(doL2FEB) {
    sc = m_storeGate->retrieve(trigL2FEBcont, m_l2feb_key);
    if (sc.isFailure() || !trigL2FEBcont) {
      *m_log << MSG::DEBUG << "Could not retrieve TrigMissingETContainter with Key \""
        << m_l2feb_key << "\" : trigL2FEBcont = 0" << endreq;
    } 
    else if (m_debuglevel) {
      *m_log << MSG::DEBUG << "Got TrigMissingETContainer with key \"" << m_l2feb_key << "\"" << endreq;
      *m_log << MSG::DEBUG << "trigL2FEBcont = " << trigL2FEBcont << endreq;
    }
  }
  // get L2FEB MET
  const TrigMissingET *missL2FEB = 0;
  if(doL2FEB) {
    if (trigL2FEBcont) {
      TrigMissingETContainer::const_iterator trigMETfirst  = trigL2FEBcont->begin();
      TrigMissingETContainer::const_iterator trigMETlast = trigL2FEBcont->end();
      if (trigMETfirst != trigMETlast) {
        missL2FEB = *trigMETfirst;
      }
    }
  }
  // get EF TrigMissingETContainer
  const TrigMissingETContainer *trigEFMETcont = 0;
  sc = m_storeGate->retrieve(trigEFMETcont, m_efmet_key);
  if (sc.isFailure() || !trigEFMETcont) {
    *m_log << MSG::DEBUG << "Could not retrieve TrigMissingETContainer with Key \""
      << m_efmet_key << "\" : trigEFMETcont = 0" << endreq;
  }
  else if (m_debuglevel) {
    *m_log << MSG::DEBUG << "Got TrigMissingETContainer with key \"" << m_efmet_key << "\"" << endreq;
    *m_log << MSG::DEBUG << "trigEFMETcont = " << trigEFMETcont << endreq;
  }

  // get EF MET
  const TrigMissingET *missETEF = 0;
  if (trigEFMETcont) {
    TrigMissingETContainer::const_iterator trigMETfirst  = trigEFMETcont->begin();
    TrigMissingETContainer::const_iterator trigMETlast = trigEFMETcont->end();
    if (trigMETfirst != trigMETlast) { 
      missETEF = *(trigEFMETcont->begin());
    }
  }

  const MissingET *missETRec = 0;
  if(m_doRecMET) {
    // get offline MET
    sc = m_storeGate->retrieve(missETRec, m_recmet_key);
    if (sc.isFailure() || !missETRec) {
      *m_log << MSG::DEBUG << "Could not retrieve Reconstructed MET term with Key \"" << m_recmet_key << "\" : missETRec = 0" << endreq;
    }
    else if (m_debuglevel) {
      *m_log << MSG::DEBUG << "Got Reconstructed MET term with key \"" << m_recmet_key << "\"" << endreq;
      *m_log << MSG::DEBUG << "missETRec = " << missETRec << endreq;
    } 
  }

  // fill shift histograms
  if(doL2MET) sc = fillL1ShiftHistos(L1METROI);
  if(doL2MET) sc = fillL2ShiftHistos(L1METROI, missETL2);

  if(doL2FEB) sc = fillL2FEBShiftHistos(missL2FEB);
  sc = fillEFShiftHistos(L1METROI, missETL2, missETEF);
  if(m_doRecMET) sc = fillRecShiftHistos(L1METROI, missETL2, missETEF, missETRec);

  // fill expert histograms
  if (m_is_make_expert_histos) {
    sc = fillExpertHistos(L1METROI, missETL2, missETEF, missETRec);
  }

  return sc;
}


StatusCode HLTMETMonTool::fillL1ShiftHistos(const LVL1_ROI::energysums_type & L1METROI) {

  if (m_debuglevel) *m_log << MSG::DEBUG << "HLTMETMonTool : in fillL1ShiftHistos()" << endreq;

  if(!m_lvl1_roi) return StatusCode::SUCCESS;

  std::string L2mongroupName = m_mon_path + "/L2MissingET_Fex";
  setCurrentMonGroup(L2mongroupName.c_str());

  LVL1_ROI::energysums_type::const_iterator it_L1 = L1METROI.begin();
  LVL1_ROI::energysums_type::const_iterator it_e_L1 = L1METROI.end();

  TH1 *h(0);
  TH2 *h2(0);
  float epsilon = 1e-6;  // 1 keV
  float epsilon_l1met = 1.e-3; // 1 MeV


  for( ; it_L1 != it_e_L1; it_L1++) {
    //setCurrentMonGroup(L2mongroupName.c_str());
    float m_lvl1_mex = -9e9;
    float m_lvl1_mey = -9e9;
    float m_lvl1_met = -9e9;
    float m_lvl1_set = -9e9;
    float m_lvl1_phi = -9e9;
    float m_lvl1_mex_log = -9e9;
    float m_lvl1_mey_log = -9e9;
    float m_lvl1_met_log = -9e9;
    float m_lvl1_set_log = -9e9;

    m_lvl1_mex = - (it_L1->getEnergyX())/CLHEP::GeV;
    m_lvl1_mey = - (it_L1->getEnergyY())/CLHEP::GeV;
    m_lvl1_met = sqrt(m_lvl1_mex*m_lvl1_mex + m_lvl1_mey*m_lvl1_mey);
    m_lvl1_phi = atan2f(m_lvl1_mey,m_lvl1_mex);
    m_lvl1_set = (it_L1->getEnergyT())/CLHEP::GeV; 

    if(m_lvl1_met > epsilon_l1met) {
      if((h = hist("L1_METx")))   h->Fill(m_lvl1_mex);
      if((h = hist("L1_METy")))   h->Fill(m_lvl1_mey);
      if((h = hist("L1_MET")))   h->Fill(m_lvl1_met);
      if((h = hist("L1_MET_lin1")))   h->Fill(m_lvl1_met);
      if((h = hist("L1_SumEt")))   h->Fill(m_lvl1_set);
      if((h = hist("L1_MET_phi")))   h->Fill(m_lvl1_phi);
      if((h = hist("L1_MET_phi1")))   h->Fill(m_lvl1_phi,m_lvl1_met);
    }
    if (fabsf(m_lvl1_mex)>epsilon)
      m_lvl1_mex_log = copysign(log10(fabsf(m_lvl1_mex)), m_lvl1_mex);
    else
      m_lvl1_mex_log = 0.01;

    if (fabsf(m_lvl1_mey)>epsilon)
      m_lvl1_mey_log = copysign(log10(fabsf(m_lvl1_mey)), m_lvl1_mey);
    else
      m_lvl1_mey_log = 0.01;

    if (fabsf(m_lvl1_met)>epsilon)
      m_lvl1_met_log = copysign(log10(fabsf(m_lvl1_met)), m_lvl1_met);
    else
      m_lvl1_met_log = 0.01;

    if (fabsf(m_lvl1_set)>epsilon)
      m_lvl1_set_log = copysign(log10(fabsf(m_lvl1_set)), m_lvl1_set);
    else
      m_lvl1_set_log = 0.01;

    if(m_lvl1_met > epsilon_l1met) {
      if((h = hist("L1_MEx_log"))) h->Fill(m_lvl1_mex_log);
      if((h = hist("L1_MEy_log"))) h->Fill(m_lvl1_mey_log);
      if((h = hist("L1_MET_log"))) h->Fill(m_lvl1_met_log);
      if((h = hist("L1_SumEt_log"))) h->Fill(m_lvl1_set_log);
    }
    float m_sqrt_set = sqrt(m_lvl1_set > epsilon ? m_lvl1_set:0.);
    if((h2 = hist2("L1MET_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_lvl1_met);
    if((h2 = hist2("L1MEx_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_lvl1_mex);
    if((h2 = hist2("L1MEy_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_lvl1_mey);
    bool pass_zb = getTDT()->isPassed("L1_RD0_FILLED", TrigDefs::eventAccepted);
    if(pass_zb) {
      if((h2 = hist2("L1MET_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_lvl1_met);
      if((h2 = hist2("L1MEx_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_lvl1_mex);
      if((h2 = hist2("L1MEy_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_lvl1_mey);
    }
  }

  return StatusCode::SUCCESS; 
}

//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillL2FEBShiftHistos(const TrigMissingET *& missL2FEB) {

  //*m_log << MSG::INFO << "HLTMETMonTool : in fillL2FEBShiftHistos()" << missL2FEB << endreq;
  if (m_debuglevel) *m_log << MSG::DEBUG << "HLTMETMonTool : in fillL2FEBShiftHistos()" << endreq;
  
  if(!missL2FEB) return StatusCode::SUCCESS;
  TH1 *h(0);
  TH2 *h2(0);

  std::string L2mongroupName = m_mon_path + "/L2MissingET_FEB";
  setCurrentMonGroup(L2mongroupName.c_str());
  //___________________________________________________________________________
  float m_etx_log = -9e9;
  float m_ety_log = -9e9;
  float m_etz_log = -9e9;
  float m_met_log = -9e9;
  float m_set_log = -9e9;
  float m_me_log = -9e9;
  float m_se_log = -9e9;
  float m_etx_lin = -9e9;
  float m_ety_lin = -9e9;
  float m_etz_lin = -9e9;
  float m_met_lin = -9e9;
  float m_set_lin = -9e9;
  float m_se_lin = -9e9;
  float m_me_lin = -9e9;
  float m_phi = -9e9;

  float m_ETx     = missL2FEB->ex();
  float m_ETy     = missL2FEB->ey();
  float m_ETz     = missL2FEB->ez();
  float m_MET     = missL2FEB->et();
  float m_SumEt   = missL2FEB->sumEt();
  float m_SumE    = missL2FEB->sumE();

  // monitoring values (in CLHEP::GeV)
  m_etx_lin = m_ETx/CLHEP::GeV; 
  m_ety_lin = m_ETy/CLHEP::GeV; 
  m_etz_lin = m_ETz/CLHEP::GeV;
  m_me_lin  = sqrt(m_etx_lin*m_etx_lin+m_ety_lin*m_ety_lin+m_etz_lin*m_etz_lin);
  m_met_lin = m_MET/CLHEP::GeV; 
  m_set_lin = m_SumEt/CLHEP::GeV;
  m_se_lin  = m_SumE/CLHEP::GeV; 
  m_phi = atan2f(m_ETy, m_ETx);

  float epsilon = 1e-6;  // 1 keV
  if (m_me_lin>epsilon)  m_me_log  = log10(fabsf(m_me_lin)); // underflow otherwise

  epsilon=1.189;
  if (fabsf(m_etx_lin)>epsilon)
    m_etx_log = copysign(log10(fabsf(m_etx_lin)), m_etx_lin);
  else
    m_etx_log = 0.01;
  if (fabsf(m_ety_lin)>epsilon)
    m_ety_log = copysign(log10(fabsf(m_ety_lin)), m_ety_lin);
  else
    m_ety_log = 0.01;
  if (fabsf(m_etz_lin)>epsilon)
    m_etz_log = copysign(log10(fabsf(m_etz_lin)), m_etz_lin);
  else
    m_etz_log = 0.01;

  if (fabsf(m_met_lin)>epsilon)
    m_met_log = copysign(log10(fabsf(m_met_lin)), m_met_lin);
  else
    m_met_log = 0.01;
  if (fabsf(m_set_lin)>epsilon)
    m_set_log = copysign(log10(fabsf(m_set_lin)), m_set_lin);
  else
    m_set_log = 0.01;
  if (fabsf(m_se_lin)>epsilon)
    m_se_log  = copysign(log10(fabsf(m_se_lin)), m_se_lin);
  else
    m_se_log = 0.01;



  if((h = hist("L2FEB_MEx_lin")     ))   h->Fill(m_etx_lin);
  if((h = hist("L2FEB_MEy_lin")     ))   h->Fill(m_ety_lin);
  if((h = hist("L2FEB_MEz_lin")     ))   h->Fill(m_etz_lin);
  if((h = hist("L2FEB_ME_lin")      ))   h->Fill(m_me_lin);
  if((h = hist("L2FEB_MET_lin")      ))  h->Fill(m_met_lin);
  if((h = hist("L2FEB_MET_lin1")     ))  h->Fill(m_met_lin);
  if((h = hist("L2FEB_SumEt_lin")    ))  h->Fill(m_set_lin);
  if((h = hist("L2FEB_SumE_lin")     ))  h->Fill(m_se_lin);
  if((h = hist("L2FEB_MET_phi")      ))  h->Fill(m_phi);
  if((h = hist("L2FEB_MET_phi1")     ))  h->Fill(m_phi,m_met_lin);

  if((h = hist("L2FEB_MEx_log")     ))   h->Fill(m_etx_log);
  if((h = hist("L2FEB_MEy_log")     ))   h->Fill(m_ety_log);
  if((h = hist("L2FEB_MEz_log")     ))   h->Fill(m_etz_log);
  if((h = hist("L2FEB_ME_log")      ))   h->Fill(m_me_log);
  if((h = hist("L2FEB_MET_log")      ))  h->Fill(m_met_log);
  if((h = hist("L2FEB_SumEt_log")    ))  h->Fill(m_set_log);
  if((h = hist("L2FEB_SumE_log")     ))  h->Fill(m_se_log);

  float m_sqrt_set = sqrt(m_set_lin > epsilon ? m_set_lin : 0.);
  if((h2 = hist2("L2MET_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_met_lin);
  if((h2 = hist2("L2MEx_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_etx_lin);
  if((h2 = hist2("L2MEy_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_ety_lin);
  bool pass_zb = getTDT()->isPassed("L1_RD0_FILLED", TrigDefs::eventAccepted);
  if(pass_zb) {
    if((h2 = hist2("L2MET_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_met_lin);
    if((h2 = hist2("L2MEx_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_etx_lin);
    if((h2 = hist2("L2MEy_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_ety_lin);
  }
  
  //___________________________________________________________________________
  unsigned int Nc = missL2FEB->getNumOfComponents();
  unsigned int compNc = m_compNamesL2.size();
  
  //*m_log << MSG::INFO << "Nc = " << Nc << "\tcompNc = " << compNc << endreq;

  Nc = Nc > compNc ? compNc : Nc;

  if (Nc > 0) {

    for (unsigned int i = 0; i < Nc; ++i) { // loop over components
      float ex =                missL2FEB->getExComponent(i)/CLHEP::GeV;
      float ey =                missL2FEB->getEyComponent(i)/CLHEP::GeV;
      float ez =                missL2FEB->getEzComponent(i)/CLHEP::GeV;
      float et =                sqrt(ex*ex+ey*ey);
      float sumE =              missL2FEB->getSumEComponent(i)/CLHEP::GeV;
      float sumEt =             missL2FEB->getSumEtComponent(i)/CLHEP::GeV;

      float m_comp_ex_log = -9e9;
      float m_comp_ey_log = -9e9;
      float m_comp_ez_log = -9e9;
      float m_comp_et_log = -9e9;
      float m_comp_sum_et_log = -9e9;
      float m_comp_sum_e_log = -9e9;
      float m_comp_sum_et_lin = -9e9;
      float m_comp_sum_e_lin = -9e9;
      float m_comp_et_lin = -9e9;

      if (fabsf(ex)>epsilon)
        m_comp_ex_log = copysign(log10f(fabsf(ex)), ex);
      else
        m_comp_ex_log = 0.01;

      if (fabsf(ey)>epsilon)
        m_comp_ey_log = copysign(log10f(fabsf(ey)), ey);
      else
        m_comp_ey_log = 0.01;

      if (fabsf(ez)>epsilon)
        m_comp_ez_log = copysign(log10f(fabsf(ez)), ez);
      else
        m_comp_ez_log = 0.01;

      m_comp_et_lin = et;
      if (et>1e-6) // i.e. et > 0
        m_comp_et_log = log10f(et);
      else
        m_comp_et_log = -100; // underflow

      m_comp_sum_et_lin = sumEt;
      m_comp_sum_e_lin = sumE;
      
      if (fabsf(sumEt)>epsilon)
        m_comp_sum_et_log = copysign(log10f(fabsf(sumEt)), sumEt);
      else
        m_comp_sum_et_log = 0.01;
      if (fabsf(sumE)>epsilon)
        m_comp_sum_e_log = copysign(log10f(fabsf(sumE)), sumE);
      else
        m_comp_sum_e_log = 0.01;

      if((h2 = hist2("compN_compEx")))      h2->Fill(i,m_comp_ex_log);
      if((h2 = hist2("compN_compEy")))      h2->Fill(i,m_comp_ey_log);
      if((h2 = hist2("compN_compEz")))      h2->Fill(i,m_comp_ez_log);
      if((h2 = hist2("compN_compEt")))      h2->Fill(i,m_comp_et_log);
      if((h2 = hist2("compN_compSumEt")))      h2->Fill(i,m_comp_sum_et_log);
      if((h2 = hist2("compN_compSumE")))      h2->Fill(i,m_comp_sum_e_log);
      if((h2 = hist2("compN_compEt_lin")))      h2->Fill(i,m_comp_et_lin);
      if((h2 = hist2("compN_compSumEt_lin")))      h2->Fill(i,m_comp_sum_et_lin);
      
      bool m_fill_stat = false;
      if((h2 = hist2("compN_L2FEB_MET_status"))) m_fill_stat = true;
      // loop over status flags
      for (int j=0; j<32; ++j) {
        unsigned mask = (1<<j);
        if (missL2FEB->getStatus(i) & mask) {
          if(m_fill_stat && h2) h2->Fill(i,j);
        }
      }
     
      float nchan_used = fabsf(missL2FEB->getUsedChannels(i));
      if(nchan_used > epsilon) nchan_used = log10f(nchan_used);
      else nchan_used = -1.;
      if((h2 = hist2("compN_L2FEB_usedChannels"))) h2->Fill(i,nchan_used);

      CLHEP::Hep3Vector v(ex,ey,ez);
      double eta = v.eta();
      double phi = v.phi();

      if((h2 = hist2(Form("compN_EtaPhi_%02d", i))))      h2->Fill(eta, phi);
      if((h2 = hist2(Form("compEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_et_lin);
      if((h2 = hist2(Form("compSumEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_sum_et_lin);
      if((h2 = hist2(Form("compSumE_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_sum_e_lin);



    } // loop over components
  }
  return StatusCode::SUCCESS; 
}

//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillL2ShiftHistos(const LVL1_ROI::energysums_type & L1METROI,
					    const TrigMissingET *& missETL2) {

  if (m_debuglevel) *m_log << MSG::DEBUG << "HLTMETMonTool : in fillL2ShiftHistos()" << endreq;
  
  if(!missETL2) return StatusCode::SUCCESS;
  TH1 *h(0);
  TH2 *h2(0);

  std::string L2mongroupName = m_mon_path + "/L2MissingET_Fex";
  setCurrentMonGroup(L2mongroupName.c_str());

  float m_lvl2_mex = -9e9;
  float m_lvl2_mey = -9e9;
  float m_lvl2_met = -9e9;
  float m_lvl2_set = -9e9;
  float m_lvl2_phi = -9e9;
  float m_lvl2_mex_log = -9e9;
  float m_lvl2_mey_log = -9e9;
  float m_lvl2_met_log = -9e9;
  float m_lvl2_set_log = -9e9;

  float m_ETx     = missETL2->ex();
  float m_ETy     = missETL2->ey();
  float m_MET     = missETL2->et();
  float m_SumEt   = missETL2->sumEt();
  
  m_lvl2_mex = m_ETx/CLHEP::GeV;
  m_lvl2_mey = m_ETy/CLHEP::GeV;
  m_lvl2_met = m_MET/CLHEP::GeV;
  m_lvl2_set = m_SumEt/CLHEP::GeV;
  m_lvl2_phi = atan2f(m_ETy, m_ETx);

  float epsilon = 1e-6;  // 1 keV
  if (fabsf(m_lvl2_mex)>epsilon)
    m_lvl2_mex_log = copysign(log10(fabsf(m_lvl2_mex)), m_lvl2_mex);
  else
    m_lvl2_mex_log = 0.01;

  if (fabsf(m_lvl2_mey)>epsilon)
    m_lvl2_mey_log = copysign(log10(fabsf(m_lvl2_mey)), m_lvl2_mey);
  else
    m_lvl2_mey_log = 0.01;

  if (fabsf(m_lvl2_met)>epsilon)
    m_lvl2_met_log = copysign(log10(fabsf(m_lvl2_met)), m_lvl2_met);
  else
    m_lvl2_met_log = 0.01;

  if (fabsf(m_lvl2_set)>epsilon)
    m_lvl2_set_log = copysign(log10(fabsf(m_lvl2_set)), m_lvl2_set);
  else
    m_lvl2_set_log = 0.01;


  if((h = hist("L2_METx")     ))   h->Fill(m_lvl2_mex);
  if((h = hist("L2_METy")     ))   h->Fill(m_lvl2_mey);
  if((h = hist("L2_MET")      ))   h->Fill(m_lvl2_met);
  if((h = hist("L2_MET_lin1") ))   h->Fill(m_lvl2_met);
  if((h = hist("L2_SumEt")    ))   h->Fill(m_lvl2_set);
  if((h = hist("L2_MET_phi")  ))   h->Fill(m_lvl2_phi);
  if((h = hist("L2_MET_phi1")  ))   h->Fill(m_lvl2_phi, m_lvl2_met);
  if((h = hist("L2_MEx_log")     ))   h->Fill(m_lvl2_mex_log);
  if((h = hist("L2_MEy_log")     ))   h->Fill(m_lvl2_mey_log);
  if((h = hist("L2_MET_log")      ))   h->Fill(m_lvl2_met_log);
  if((h = hist("L2_SumEt_log")    ))   h->Fill(m_lvl2_set_log);

  // L2 components -- begin
  unsigned int Nc = missETL2->getNumOfComponents();
  unsigned int compNc = m_compNamesEF.size();
  Nc = Nc > compNc ? compNc : Nc;

  if (Nc > 0) {

    for (unsigned int i = 0; i < Nc; ++i) { // loop over components
      float m_l2mu_set = -9e9;
      float m_l2mu_set_log = -9e9;
      m_l2mu_set =                missETL2->getSumEtComponent(i)/CLHEP::GeV;
      if((h = hist("L2_mu_SumEt"))) h->Fill(m_l2mu_set);
      if (fabsf(m_l2mu_set)>epsilon)
        m_l2mu_set_log = copysign(log10(fabsf(m_l2mu_set)), m_l2mu_set);
      else
        m_l2mu_set_log = 0.01;
      if((h = hist("L2_mu_SumEt_log"))) h->Fill(m_l2mu_set_log);

    }
  }
  // L2 components -- end



  TH1F *h1i(0);
  bool m_fill_stat = false;
  if((h1i = (TH1F *) hist("L2_MET_status"))) m_fill_stat = true;
  for (int i=0; i<32; ++i) {
    unsigned mask = (1<<i);
    if (missETL2->getFlag() & mask) {
      if(m_fill_stat && h1i) h1i->Fill(i,1.);
    } else {
      if(m_fill_stat && h1i) h1i->Fill(i,0);
    }
  }

  /*
  std::string lbnMETMonGroup = m_mon_path + "/lbnstatus";
  setCurrentMonGroup(lbnMETMonGroup);
  m_fill_stat = false;
  h1i = NULL;
  if((h1i = (TH1F *) hist("L2_MET_status"))) m_fill_stat = true;
  for (int i=0; i<32; ++i) {
    unsigned mask = (1<<i);
    if (missETL2->getFlag() & mask) {
      if(m_fill_stat && h1i) h1i->Fill(i,1.);
    } else {
      if(m_fill_stat && h1i) h1i->Fill(i,0);
    }
  }
  setCurrentMonGroup(L2mongroupName.c_str());
  */


  // fill correlation histograms -- L1MET vs L2MET
  if (m_lvl1_roi) {
    LVL1_ROI::energysums_type::const_iterator it_L1 = L1METROI.begin();
    LVL1_ROI::energysums_type::const_iterator it_e_L1 = L1METROI.end();

    for( ; it_L1 != it_e_L1; it_L1++) {
      float sumet = (it_L1->getEnergyT())/CLHEP::GeV;
      float ex = - (it_L1->getExMiss())/CLHEP::GeV;
      float ey = - (it_L1->getEyMiss())/CLHEP::GeV;
      float et = sqrt(ex*ex + ey*ey);
      float phi = atan2f(ey, ex);
      float dphi = signed_delta_phi(phi, m_lvl2_phi);

      if((h2 = hist2("L1MET_L2MET_EtCor")))   h2->Fill(m_lvl2_met, et);
      if((h2 = hist2("L1MET_L2MET_SumEtCor")))   h2->Fill(m_lvl2_set, sumet);
      if((h2 = hist2("L1MET_L2MET_PhiCor")))   h2->Fill(m_lvl2_phi, phi);

      if((h = hist ("L1MET_L2MET_dEt")))   h->Fill(et - m_lvl2_met);
      if((h = hist ("L1MET_L2MET_dPhi")))   h->Fill(dphi);
      if((h = hist ("L1MET_L2MET_dEx")))   h->Fill(ex - m_lvl2_mex);
      if((h = hist ("L1MET_L2MET_dEy")))   h->Fill(ey - m_lvl2_mey); 
    }
  }

  return StatusCode::SUCCESS; 
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillEFShiftHistos(const LVL1_ROI::energysums_type & L1METROI,
					    const TrigMissingET *& missETL2,
					    const TrigMissingET *& missETEF) {

  if (m_debuglevel) *m_log << MSG::DEBUG << "HLTMETMonTool : in fillEFShiftHistos()" << endreq;

  if(!missETEF) return StatusCode::SUCCESS;

  TH1 *h(0);
  TH2 *h2(0);
  std::string EFmongroupName = m_mon_path + "/EFMissingET_Fex";
  setCurrentMonGroup(EFmongroupName.c_str());	

  float m_etx_log = -9e9;
  float m_ety_log = -9e9;
  float m_etz_log = -9e9;
  float m_met_log = -9e9;
  float m_set_log = -9e9;
  float m_me_log = -9e9;
  float m_se_log = -9e9;
  float m_etx_lin = -9e9;
  float m_ety_lin = -9e9;
  float m_etz_lin = -9e9;
  float m_met_lin = -9e9;
  float m_set_lin = -9e9;
  float m_se_lin = -9e9;
  float m_me_lin = -9e9;
  float m_phi = -9e9;

  float m_ETx     = missETEF->ex();
  float m_ETy     = missETEF->ey();
  float m_ETz     = missETEF->ez();
  float m_MET     = missETEF->et();
  float m_SumEt   = missETEF->sumEt();
  float m_SumE    = missETEF->sumE();

  // monitoring values (in CLHEP::GeV)
  m_etx_lin = m_ETx/CLHEP::GeV; 
  m_ety_lin = m_ETy/CLHEP::GeV; 
  m_etz_lin = m_ETz/CLHEP::GeV;
  m_me_lin  = sqrt(m_etx_lin*m_etx_lin+m_ety_lin*m_ety_lin+m_etz_lin*m_etz_lin);
  m_met_lin = m_MET/CLHEP::GeV; 
  m_set_lin = m_SumEt/CLHEP::GeV;
  m_se_lin  = m_SumE/CLHEP::GeV; 
  m_phi = atan2f(m_ETy, m_ETx);

  float epsilon = 1e-6;  // 1 keV
  if (m_me_lin>epsilon)  m_me_log  = log10(fabsf(m_me_lin)); // underflow otherwise

  epsilon=1.189;
  if (fabsf(m_etx_lin)>epsilon)
    m_etx_log = copysign(log10(fabsf(m_etx_lin)), m_etx_lin);
  else
    m_etx_log = 0.01;
  if (fabsf(m_ety_lin)>epsilon)
    m_ety_log = copysign(log10(fabsf(m_ety_lin)), m_ety_lin);
  else
    m_ety_log = 0.01;
  if (fabsf(m_etz_lin)>epsilon)
    m_etz_log = copysign(log10(fabsf(m_etz_lin)), m_etz_lin);
  else
    m_etz_log = 0.01;

  if (fabsf(m_met_lin)>epsilon)
    m_met_log = copysign(log10(fabsf(m_met_lin)), m_met_lin);
  else
    m_met_log = 0.01;
  if (fabsf(m_set_lin)>epsilon)
    m_set_log = copysign(log10(fabsf(m_set_lin)), m_set_lin);
  else
    m_set_log = 0.01;
  if (fabsf(m_se_lin)>epsilon)
    m_se_log  = copysign(log10(fabsf(m_se_lin)), m_se_lin);
  else
    m_se_log = 0.01;



  if((h = hist("EF_MEx_lin")     ))   h->Fill(m_etx_lin);
  if((h = hist("EF_MEy_lin")     ))   h->Fill(m_ety_lin);
  if((h = hist("EF_MEz_lin")     ))   h->Fill(m_etz_lin);
  if((h = hist("EF_ME_lin")      ))   h->Fill(m_me_lin);
  if((h = hist("EF_MET_lin")      ))  h->Fill(m_met_lin);
  if((h = hist("EF_MET_lin1")     ))  h->Fill(m_met_lin);
  if((h = hist("EF_SumEt_lin")    ))  h->Fill(m_set_lin);
  if((h = hist("EF_SumE_lin")     ))  h->Fill(m_se_lin);
  if((h = hist("EF_MET_phi")      ))  h->Fill(m_phi);
  if((h = hist("EF_MET_phi1")     ))  h->Fill(m_phi,m_met_lin);

  if((h = hist("EF_MEx_log")     ))   h->Fill(m_etx_log);
  if((h = hist("EF_MEy_log")     ))   h->Fill(m_ety_log);
  if((h = hist("EF_MEz_log")     ))   h->Fill(m_etz_log);
  if((h = hist("EF_ME_log")      ))   h->Fill(m_me_log);
  if((h = hist("EF_MET_log")      ))  h->Fill(m_met_log);
  if((h = hist("EF_SumEt_log")    ))  h->Fill(m_set_log);
  if((h = hist("EF_SumE_log")     ))  h->Fill(m_se_log);
  
  float m_sqrt_set = sqrt(m_set_lin > epsilon ? m_set_lin : 0.);
  if((h2 = hist2("EFMET_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_met_lin);
  if((h2 = hist2("EFMEx_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_etx_lin);
  if((h2 = hist2("EFMEy_VS_SqrtSET_All")      ))  h2->Fill(m_sqrt_set, m_ety_lin);
  bool pass_zb = getTDT()->isPassed("L1_RD0_FILLED", TrigDefs::eventAccepted);
  if(pass_zb) {
    if((h2 = hist2("EFMET_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_met_lin);
    if((h2 = hist2("EFMEx_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_etx_lin);
    if((h2 = hist2("EFMEy_VS_SqrtSET_RD0")      ))  h2->Fill(m_sqrt_set, m_ety_lin);
  }


  //std::vector<int> m_status_flag(32,0);
  TH1F *h1i(0);
  bool m_fill_stat = false;
  if((h1i = (TH1F *) hist("EF_MET_status"))) m_fill_stat = true;
  for (int i=0; i<32; ++i) {
    unsigned mask = (1<<i);
    if (missETEF->getFlag() & mask) {
      if(m_fill_stat && h1i) h1i->Fill(i,1.);
      //m_status_flag[i] = 1;
    } else {
      if(m_fill_stat && h1i) h1i->Fill(i,0);
      //m_status_flag[i] = 0;
    }
  }

  /*
  std::string lbnMETMonGroup = m_mon_path + "/lbnstatus";
  setCurrentMonGroup(lbnMETMonGroup);
  m_fill_stat = false;
  h1i = NULL;
  if((h1i = (TH1F *) hist("EF_MET_status"))) m_fill_stat = true;
  for (int i=0; i<32; ++i) {
    unsigned mask = (1<<i);
    if (missETEF->getFlag() & mask) {
      if(m_fill_stat && h1i) h1i->Fill(i,1.);
    } else {
      if(m_fill_stat && h1i) h1i->Fill(i,0);
    }
  }
  setCurrentMonGroup(EFmongroupName.c_str());
  */

  // fill correlation histograms -- L1MET vs EFMET
  if (m_lvl1_roi) {
    LVL1_ROI::energysums_type::const_iterator it_L1 = L1METROI.begin();
    LVL1_ROI::energysums_type::const_iterator it_e_L1 = L1METROI.end();

    for( ; it_L1 != it_e_L1; it_L1++) {
      float sumet = (it_L1->getEnergyT())/CLHEP::GeV;
      float ex = - (it_L1->getExMiss())/CLHEP::GeV;
      float ey = - (it_L1->getEyMiss())/CLHEP::GeV;
      float et = sqrt(ex*ex + ey*ey);
      float phi = atan2f(ey, ex);
      float dphi = signed_delta_phi(phi, m_phi);

      if((h2 = hist2("L1MET_EFMET_EtCor")))   h2->Fill(m_met_lin, et);
      if((h2 = hist2("L1MET_EFMET_SumEtCor")))   h2->Fill(m_set_lin, sumet);
      if((h2 = hist2("L1MET_EFMET_PhiCor")))   h2->Fill(m_phi, phi);

      if((h = hist("L1MET_EFMET_dEt")))   h->Fill(et - m_met_lin);
      if((h = hist("L1MET_EFMET_dPhi")))   h->Fill(dphi);
      if((h = hist("L1MET_EFMET_dEx")))   h->Fill(ex - m_etx_lin);
      if((h = hist("L1MET_EFMET_dEy")))   h->Fill(ey - m_ety_lin); 
    }
  }

  // fill correlation histograms -- L2MET vs EFMET
  if (missETL2) {
    float sumet = (missETL2->sumEt())/CLHEP::GeV;
    float et = (missETL2->et())/CLHEP::GeV;
    float ex = (missETL2->ex())/CLHEP::GeV;
    float ey = (missETL2->ey())/CLHEP::GeV;
    float phi = atan2f(ey, ex);
    float dphi = signed_delta_phi(phi, m_phi);

    if((h2 = hist2("L2MET_EFMET_EtCor")))   h2->Fill(m_met_lin, et);
    if((h2 = hist2("L2MET_EFMET_SumEtCor")))   h2->Fill(m_set_lin, sumet);
    if((h2 = hist2("L2MET_EFMET_PhiCor")))   h2->Fill(m_phi, phi);

    if((h = hist("L2MET_EFMET_dEt")))   h->Fill(et - m_met_lin);
    if((h = hist("L2MET_EFMET_dPhi")))   h->Fill(dphi);
    if((h = hist("L2MET_EFMET_dEx")))   h->Fill(ex - m_etx_lin);
    if((h = hist("L2MET_EFMET_dEy")))   h->Fill(ey - m_ety_lin);
  }

  unsigned int Nc = missETEF->getNumOfComponents();
  unsigned int compNc = m_compNamesEF.size();
  Nc = Nc > compNc ? compNc : Nc;

  if (Nc > 0) {

    for (unsigned int i = 0; i < Nc; ++i) { // loop over components
      float ex =                missETEF->getExComponent(i)/CLHEP::GeV;
      float ey =                missETEF->getEyComponent(i)/CLHEP::GeV;
      float ez =                missETEF->getEzComponent(i)/CLHEP::GeV;
      float et =                sqrt(ex*ex+ey*ey);
      float sumE =              missETEF->getSumEComponent(i)/CLHEP::GeV;
      float sumEt =             missETEF->getSumEtComponent(i)/CLHEP::GeV;

      float m_comp_ex_log = -9e9;
      float m_comp_ey_log = -9e9;
      float m_comp_ez_log = -9e9;
      float m_comp_et_log = -9e9;
      float m_comp_sum_et_log = -9e9;
      float m_comp_sum_e_log = -9e9;
      float m_comp_sum_et_lin = -9e9;
      float m_comp_sum_e_lin = -9e9;
      float m_comp_et_lin = -9e9;

      if (fabsf(ex)>epsilon)
        m_comp_ex_log = copysign(log10f(fabsf(ex)), ex);
      else
        m_comp_ex_log = 0.01;

      if (fabsf(ey)>epsilon)
        m_comp_ey_log = copysign(log10f(fabsf(ey)), ey);
      else
        m_comp_ey_log = 0.01;

      if (fabsf(ez)>epsilon)
        m_comp_ez_log = copysign(log10f(fabsf(ez)), ez);
      else
        m_comp_ez_log = 0.01;

      m_comp_et_lin = et;
      if (et>1e-6) // i.e. et > 0
        m_comp_et_log = log10f(et);
      else
        m_comp_et_log = -100; // underflow

      m_comp_sum_et_lin = sumEt;
      m_comp_sum_e_lin = sumE;
      
      if (fabsf(sumEt)>epsilon)
        m_comp_sum_et_log = copysign(log10f(fabsf(sumEt)), sumEt);
      else
        m_comp_sum_et_log = 0.01;
      if (fabsf(sumE)>epsilon)
        m_comp_sum_e_log = copysign(log10f(fabsf(sumE)), sumE);
      else
        m_comp_sum_e_log = 0.01;

      if((h2 = hist2("compN_compEx")))      h2->Fill(i,m_comp_ex_log);
      if((h2 = hist2("compN_compEy")))      h2->Fill(i,m_comp_ey_log);
      if((h2 = hist2("compN_compEz")))      h2->Fill(i,m_comp_ez_log);
      if((h2 = hist2("compN_compEt")))      h2->Fill(i,m_comp_et_log);
      if((h2 = hist2("compN_compSumEt")))      h2->Fill(i,m_comp_sum_et_log);
      if((h2 = hist2("compN_compSumE")))      h2->Fill(i,m_comp_sum_e_log);
      if((h2 = hist2("compN_compEt_lin")))      h2->Fill(i,m_comp_et_lin);
      if((h2 = hist2("compN_compSumEt_lin")))      h2->Fill(i,m_comp_sum_et_lin);
      
      bool m_fill_stat = false;
      if((h2 = hist2("compN_EF_MET_status"))) m_fill_stat = true;
      // loop over status flags
      for (int j=0; j<32; ++j) {
        unsigned mask = (1<<j);
        if (missETEF->getStatus(i) & mask) {
          if(m_fill_stat && h2) h2->Fill(i,j);
        }
      }
     
      float nchan_used = fabsf(missETEF->getUsedChannels(i));
      if(nchan_used > epsilon) nchan_used = log10f(nchan_used);
      else nchan_used = -1.;
      if((h2 = hist2("compN_EF_usedChannels"))) h2->Fill(i,nchan_used);

      CLHEP::Hep3Vector v(ex,ey,ez);
      double eta = v.eta();
      double phi = v.phi();

      if((h2 = hist2(Form("compN_EtaPhi_%02d", i))))      h2->Fill(eta, phi);
      if((h2 = hist2(Form("compEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_et_lin);
      if((h2 = hist2(Form("compSumEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_sum_et_lin);
      if((h2 = hist2(Form("compSumE_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_sum_e_lin);



    } // loop over components
  }

  return StatusCode::SUCCESS; 
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillRecShiftHistos(const LVL1_ROI::energysums_type & /*L1METROI*/,
    const TrigMissingET *& /*missETL2*/,  const TrigMissingET *& /*missETEF*/,    
    const MissingET *& /*missETRec*/) {

  if (m_debuglevel) *m_log << MSG::DEBUG << "HLTMETMonTool : in fillRecShiftHistos()" << endreq;

  //if(!missETRec) return StatusCode::SUCCESS;

  //TH1 *h(0);
  //TH2 *h2(0);
  /*
  std::string RecmongroupName = m_mon_path + "/RecMissingET_allStats";
  setCurrentMonGroup(RecmongroupName.c_str());	

  float et_RecMET = (missETRec->et())/CLHEP::GeV;
  float sumet_RecMET = (missETRec->sumet())/CLHEP::GeV;
  float ex_RecMET = (missETRec->etx())/CLHEP::GeV;
  float ey_RecMET = (missETRec->ety())/CLHEP::GeV;
  float phi_RecMET = missETRec->phi();

  if((h = hist("RecMET_et")))   h->Fill(et_RecMET);
  if((h = hist("RecMET_sumet")))   h->Fill(sumet_RecMET);
  if((h = hist("RecMET_phi")))   h->Fill(phi_RecMET);

  // fill correlation histograms -- L1MET vs RecMET
  if (m_lvl1_roi) {
    LVL1_ROI::energysums_type::const_iterator it_L1 = L1METROI.begin();
    LVL1_ROI::energysums_type::const_iterator it_e_L1 = L1METROI.end();

    for( ; it_L1 != it_e_L1; it_L1++) {
      float sumet = (it_L1->getEnergyT())/CLHEP::GeV;
      float ex = - (it_L1->getExMiss())/CLHEP::GeV;
      float ey = - (it_L1->getEyMiss())/CLHEP::GeV;
      float et = sqrt(ex*ex + ey*ey);
      float phi = atan2f(ey, ex);
      float dphi = signed_delta_phi(phi, phi_RecMET);

      if((h2 = hist2("L1MET_RecMET_EtCor")))   h2->Fill(et_RecMET, et);
      if((h2 = hist2("L1MET_RecMET_SumEtCor")))   h2->Fill(sumet_RecMET, sumet);
      if((h2 = hist2("L1MET_RecMET_PhiCor")))   h2->Fill(phi_RecMET, phi);

      if((h = hist("L1MET_RecMET_dEt")))   h->Fill(et - et_RecMET);
      if((h = hist("L1MET_RecMET_dPhi")))   h->Fill(dphi);
      if((h = hist("L1MET_RecMET_dEx")))   h->Fill(ex - ex_RecMET);
      if((h = hist("L1MET_RecMET_dEy")))   h->Fill(ey - ey_RecMET); 
    }
  }

  // fill correlation histograms -- L2MET vs RecMET
  if (missETL2) {
    float sumet = (missETL2->sumEt())/CLHEP::GeV;
    float et = (missETL2->et())/CLHEP::GeV;
    float ex = (missETL2->ex())/CLHEP::GeV;
    float ey = (missETL2->ey())/CLHEP::GeV;
    float phi = atan2f(ey, ex);
    float dphi = signed_delta_phi(phi, phi_RecMET);

    if((h2 = hist2("L2MET_RecMET_EtCor")))   h2->Fill(et_RecMET, et);
    if((h2 = hist2("L2MET_RecMET_SumEtCor")))   h2->Fill(sumet_RecMET, sumet);
    if((h2 = hist2("L2MET_RecMET_PhiCor")))   h2->Fill(phi_RecMET, phi);

    if((h = hist("L2MET_RecMET_dEt")))   h->Fill(et - et_RecMET);
    if((h = hist("L2MET_RecMET_dPhi")))   h->Fill(dphi);
    if((h = hist("L2MET_RecMET_dEx")))   h->Fill(ex - ex_RecMET);
    if((h = hist("L2MET_RecMET_dEy")))   h->Fill(ey - ey_RecMET);
  }

  // fill correlation histograms -- EFMET vs RecMET
  if (missETEF) {
    float sumet = (missETEF->sumEt())/CLHEP::GeV;
    float et = (missETEF->et())/CLHEP::GeV;
    float ex = (missETEF->ex())/CLHEP::GeV;
    float ey = (missETEF->ey())/CLHEP::GeV;
    float phi = atan2f(ey, ex);
    float dphi = signed_delta_phi(phi, phi_RecMET);

    if((h2 = hist2("EFMET_RecMET_EtCor")))   h2->Fill(et_RecMET, et);
    if((h2 = hist2("EFMET_RecMET_SumEtCor")))   h2->Fill(sumet_RecMET, sumet);
    if((h2 = hist2("EFMET_RecMET_PhiCor")))   h2->Fill(phi_RecMET, phi);

    if((h = hist("EFMET_RecMET_dEt")))   h->Fill(et - et_RecMET);
    if((h = hist("EFMET_RecMET_dPhi")))   h->Fill(dphi);
    if((h = hist("EFMET_RecMET_dEx")))   h->Fill(ex - ex_RecMET);
    if((h = hist("EFMET_RecMET_dEy")))   h->Fill(ey - ey_RecMET);
  }
  */
  return StatusCode::SUCCESS; 
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillExpertHistos(const LVL1_ROI::energysums_type & L1METROI,
    const TrigMissingET *& missETL2,  const TrigMissingET *& missETEF,  const MissingET *& missETRec) {

  if (m_debuglevel) *m_log << MSG::DEBUG << "in fillExpertHistos()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  TH1 *h(0);
  TH2 *h2(0);
  // first store the values so that we don't need to retrieve/ealculate them again and again  

  std::vector<double> et_L1MET, sumet_L1MET, phi_L1MET, ex_L1MET, ey_L1MET;
  et_L1MET.clear(); sumet_L1MET.clear(); phi_L1MET.clear(); ex_L1MET.clear(); ey_L1MET.clear();
  double et_L2MET=0., sumet_L2MET=0., phi_L2MET=0., ex_L2MET=0., ey_L2MET=0.;
  double et_EFMET=0., sumet_EFMET=0., phi_EFMET=0., ex_EFMET=0., ey_EFMET=0.;
  double et_RecMET=0., sumet_RecMET=0., phi_RecMET=0., ex_RecMET=0., ey_RecMET=0.;

  // L1 MET
  if (m_lvl1_roi) {
    LVL1_ROI::energysums_type::const_iterator it_L1 = L1METROI.begin();
    LVL1_ROI::energysums_type::const_iterator it_e_L1 = L1METROI.end();
    for( ; it_L1 != it_e_L1; it_L1++) {
      float sumet = (it_L1->getEnergyT())/CLHEP::GeV;
      float ex = - (it_L1->getExMiss())/CLHEP::GeV;
      float ey = - (it_L1->getEyMiss())/CLHEP::GeV;
      float et = sqrt(ex*ex + ey*ey);
      float phi = atan2(ey, ex);
      *m_log << MSG::DEBUG << "L1 MET: " << et << " L1 SumEt: " << sumet << endreq;
      et_L1MET.push_back(et);
      sumet_L1MET.push_back(sumet);
      phi_L1MET.push_back(phi);
      ex_L1MET.push_back(ex);
      ey_L1MET.push_back(ey);
    }
  }

  // L2 MET
  if (missETL2) {
    et_L2MET = (missETL2->et())/CLHEP::GeV;
    sumet_L2MET = (missETL2->sumEt())/CLHEP::GeV;
    ex_L2MET = (missETL2->ex())/CLHEP::GeV;
    ey_L2MET = (missETL2->ey())/CLHEP::GeV;
    phi_L2MET = atan2(ey_L2MET, ex_L2MET);
    *m_log << MSG::DEBUG << "HLT L2 MET: "<< et_L2MET << " HLT L2 SumEt: " << sumet_L2MET << endreq;
  }

  // EF MET
  if (missETEF) {  
    et_EFMET = (missETEF->et())/CLHEP::GeV;
    sumet_EFMET = (missETEF->sumEt())/CLHEP::GeV;
    ex_EFMET = (missETEF->ex())/CLHEP::GeV;
    ey_EFMET = (missETEF->ey())/CLHEP::GeV;
    phi_EFMET = atan2(ey_EFMET,ex_EFMET);
    *m_log << MSG::DEBUG << "HLT EF MET: "<< et_L2MET << " HLT EF SumEt: " << sumet_EFMET << endreq;
  }

  // Rec MET
  if (missETRec) {
    et_RecMET = (missETRec->et())/CLHEP::GeV;
    sumet_RecMET = (missETRec->sumet())/CLHEP::GeV;
    ex_RecMET = (missETRec->etx())/CLHEP::GeV;
    ey_RecMET = (missETRec->ety())/CLHEP::GeV;
    phi_RecMET = missETRec->phi();
    *m_log << MSG::DEBUG << "Rec MET: "<< et_RecMET << " Rec SumEt: " << sumet_RecMET << endreq;  
  }


  // now fill the histograms

  std::string expertpartialMonGroup = m_mon_path + "/Expert/L1METTrigger/";

  std::map<std::string,int>::const_iterator it;

  // fill expert histograms for L1 MET Triggers
  for (it = m_l1_met_signatures_tolook->begin(); it != m_l1_met_signatures_tolook->end(); it++) {
    std::string name = it->first;
    if(getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      setCurrentMonGroup(expertpartialMonGroup+name);
      // L1 MET
      if (m_lvl1_roi) {
        for (unsigned int i=0; i<et_L1MET.size(); i++) { 
          if((h = hist("L1_MET")))   h->   Fill(et_L1MET[i]);
          if((h = hist("L1_SumEt")))   h-> Fill(sumet_L1MET[i]);
          if((h = hist("L1_MET_phi")))   h-> Fill(phi_L1MET[i]);
          // fill correlation if((h = histograms -- L1MET vs RecMET
          if (missETRec) {
            if((h2 = hist2("L1MET_RecMET_EtCor")))   h2->Fill(et_RecMET, et_L1MET[i]);
            if((h2 = hist2("L1MET_RecMET_SumEtCor")))   h2->Fill(sumet_RecMET, sumet_L1MET[i]);
            if((h2 = hist2("L1MET_RecMET_PhiCor")))   h2->Fill(phi_RecMET, phi_L1MET[i]);
            if((h = hist("L1MET_RecMET_dEt")))   h->Fill(et_L1MET[i] - et_RecMET);
            double dphi = signed_delta_phi(phi_L1MET[i], phi_RecMET);
            if((h = hist("L1MET_RecMET_dPhi")))   h->Fill(dphi);
            if((h = hist("L1MET_RecMET_dEx")))   h->Fill(ex_L1MET[i] - ex_RecMET);
            if((h = hist("L1MET_RecMET_dEy")))   h->Fill(ey_L1MET[i] - ey_RecMET); 
          }
        }
      }
      // EF MET
      if (missETEF) {
        if((h = hist("EF_MET_lin")      ))   h->Fill(et_EFMET);
        if((h = hist("EF_SumEt_lin")    ))   h->Fill(sumet_EFMET);
        if((h = hist("EF_MET_phi")      ))   h->Fill(phi_EFMET);
      }
      // Rec MET
      if (missETRec) {
        if((h = hist("RecMET_et")))   h->Fill(et_RecMET);
        if((h = hist("RecMET_sumet")))   h->Fill(sumet_RecMET);
        if((h = hist("RecMET_phi")))   h->Fill(phi_RecMET);
      }
    }
  }


  // fill expert histograms for L2 MET
  expertpartialMonGroup = m_mon_path + "/Expert/L2METTrigger/";

  for (it = m_l2_met_signatures_tolook->begin(); it != m_l2_met_signatures_tolook->end(); it++) {
    std::string name = it->first;
    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      setCurrentMonGroup(expertpartialMonGroup+name);
      // L2 MET
      if (missETL2) {
        if((h = hist("L2_MET")      ))   h->Fill(et_L2MET);
        if((h = hist("L2_SumEt")    ))   h->Fill(sumet_L2MET);
        if((h = hist("L2_MET_phi")  ))   h->Fill(phi_L2MET);
        // correlation histograms -- L1MET vs L2MET
        if (m_lvl1_roi) {
          for (unsigned int i=0; i<et_L1MET.size(); i++) {
            if((h2 = hist2("L1MET_L2MET_EtCor")))   h2->Fill(et_L2MET, et_L1MET[i]);
            if((h2 = hist2("L1MET_L2MET_SumEtCor")))   h2->Fill(sumet_L2MET, sumet_L1MET[i]);
            if((h2 = hist2("L1MET_L2MET_PhiCor")))   h2->Fill(phi_L2MET, phi_L1MET[i]);
            if((h = hist("L1MET_L2MET_dEt")))   h->Fill(et_L1MET[i] - et_L2MET);
            double dphi = signed_delta_phi(phi_L1MET[i], phi_L2MET);
            if((h = hist("L1MET_L2MET_dPhi")))   h->Fill(dphi);
            if((h = hist("L1MET_L2MET_dEx")))   h->Fill(ex_L1MET[i] - ex_L2MET);
            if((h = hist("L1MET_L2MET_dEy")))   h->Fill(ey_L1MET[i] - ey_L2MET); 
          }
        }
        // fill correlation histograms -- L2MET vs RecMET
        if (missETRec) {
          if((h2 = hist2("L2MET_RecMET_EtCor")))   h2->Fill(et_RecMET, et_L2MET);
          if((h2 = hist2("L2MET_RecMET_SumEtCor")))   h2->Fill(sumet_RecMET, sumet_L2MET);
          if((h2 = hist2("L2MET_RecMET_PhiCor")))   h2->Fill(phi_RecMET, phi_L2MET);
          if((h = hist("L2MET_RecMET_dEt")))   h->Fill(et_L2MET - et_RecMET);
          double dphi = signed_delta_phi(phi_L2MET, phi_RecMET);
          if((h = hist("L2MET_RecMET_dPhi")))   h->Fill(dphi);
          if((h = hist("L2MET_RecMET_dEx")))   h->Fill(ex_L2MET - ex_RecMET);
          if((h = hist("L2MET_RecMET_dEy")))   h->Fill(ey_L2MET - ey_RecMET); 
        }
      }
      // EF MET
      if (missETEF) {
        if((h = hist("EF_MET_lin")      ))   h->Fill(et_EFMET);
        if((h = hist("EF_SumEt_lin")    ))   h->Fill(sumet_EFMET);
        if((h = hist("EF_MET_phi")      ))   h->Fill(phi_EFMET);
      }
      // Rec MET
      if (missETRec) {
        if((h = hist("RecMET_et")))   h->Fill(et_RecMET);
        if((h = hist("RecMET_sumet")))   h->Fill(sumet_RecMET);
        if((h = hist("RecMET_phi")))   h->Fill(phi_RecMET);
      }
    }
  }


  // fill expert histograms for EF
  expertpartialMonGroup = m_mon_path + "/Expert/EFMETTrigger/";

  for (it = m_ef_met_signatures_tolook->begin(); it != m_ef_met_signatures_tolook->end(); it++) {
    std::string name = it->first;
    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      setCurrentMonGroup(expertpartialMonGroup+name);
      // EF MET
      if (missETEF) {
        if((h = hist("EF_MET_lin")      ))   h->Fill(et_EFMET);
        if((h = hist("EF_SumEt_lin")    ))   h->Fill(sumet_EFMET);
        if((h = hist("EF_MET_phi")      ))   h->Fill(phi_EFMET);
        // correlation histograms -- L1MET vs EFMET
        if (m_lvl1_roi) {
          for (unsigned int i=0; i<et_L1MET.size(); i++) {
            if((h2 = hist2("L1MET_EFMET_EtCor")))   h2->Fill(et_EFMET, et_L1MET[i]);
            if((h2 = hist2("L1MET_EFMET_SumEtCor")))   h2->Fill(sumet_EFMET, sumet_L1MET[i]);
            if((h2 = hist2("L1MET_EFMET_PhiCor")))   h2->Fill(phi_EFMET, phi_L1MET[i]);
            if((h = hist("L1MET_EFMET_dEt")))   h->Fill(et_L1MET[i] - et_EFMET);
            double dphi = signed_delta_phi(phi_L1MET[i], phi_EFMET);
            if((h = hist("L1MET_EFMET_dPhi")))   h->Fill(dphi);
            if((h = hist("L1MET_EFMET_dEx")))   h->Fill(ex_L1MET[i] - ex_EFMET);
            if((h = hist("L1MET_EFMET_dEy")))   h->Fill(ey_L1MET[i] - ey_L2MET); 
          }
        }
        if (missETL2) {
          if((h2 = hist2("L2MET_EFMET_EtCor")))   h2->Fill(et_EFMET, et_L2MET);
          if((h2 = hist2("L2MET_EFMET_SumEtCor")))   h2->Fill(sumet_EFMET, sumet_L2MET);
          if((h2 = hist2("L2MET_EFMET_PhiCor")))   h2->Fill(phi_EFMET, phi_L2MET);
          if((h = hist("L2MET_EFMET_dEt")))   h->Fill(et_L2MET - et_EFMET);
          double dphi = signed_delta_phi(phi_L2MET, phi_EFMET);
          if((h = hist("L2MET_EFMET_dPhi")))   h->Fill(dphi);
          if((h = hist("L2MET_EFMET_dEx")))   h->Fill(ex_L2MET - ex_EFMET);
          if((h = hist("L2MET_EFMET_dEy")))   h->Fill(ey_L2MET - ey_L2MET); 
        }
        // fill correlation histograms -- EFMET vs RecMET
        if (missETRec) {
          if((h2 = hist2("EFMET_RecMET_EtCor")))   h2->Fill(et_RecMET, et_EFMET);
          if((h2 = hist2("EFMET_RecMET_SumEtCor")))   h2->Fill(sumet_RecMET, sumet_EFMET);
          if((h2 = hist2("EFMET_RecMET_PhiCor")))   h2->Fill(phi_RecMET, phi_EFMET);
          if((h = hist("EFMET_RecMET_dEt")))   h->Fill(et_EFMET - et_RecMET);
          double dphi = signed_delta_phi(phi_EFMET, phi_RecMET);
          if((h = hist("EFMET_RecMET_dPhi")))   h->Fill(dphi);
          if((h = hist("EFMET_RecMET_dEx")))   h->Fill(ex_EFMET - ex_RecMET);
          if((h = hist("EFMET_RecMET_dEy")))   h->Fill(ey_EFMET - ey_RecMET);
        }
      }
      // Rec MET
      if (missETRec) {
        if((h = hist("RecMET_et")))   h->Fill(et_RecMET);
        if((h = hist("RecMET_sumet")))   h->Fill(sumet_RecMET);
        if((h = hist("RecMET_phi")))   h->Fill(phi_RecMET);
      }
    }
  }


  // fill efficiency histograms

  if (!m_is_do_trigger_effic) return sc;

  if (m_debuglevel) *m_log << MSG::DEBUG << "filling efficiency histograms()" << endreq;

  if (!missETRec) return sc;

  expertpartialMonGroup = m_mon_path + "/Expert/Efficiency/";

  std::map<std::string,int>::const_iterator it_s;
  std::vector<std::map<std::string,int>*> m_met_signatures_tolook;
  m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_l2_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_ef_met_signatures_tolook);
  unsigned int size = m_met_signatures_tolook.size();

  for (it_s = m_sample_selection_signatures.begin(); it_s != m_sample_selection_signatures.end(); it_s++) {
    if (getTDT()->isPassed(it_s->first, TrigDefs::eventAccepted)) {
      std::string tmp_MonGroup = expertpartialMonGroup + it_s->first;
      std::string tmp_partialMonGroup = tmp_MonGroup + "/";
      setCurrentMonGroup(tmp_MonGroup);
      if((h = hist("RecMET_et")))   h->Fill(et_RecMET);
      if((h = hist("RecMET_sumet")))   h->Fill(sumet_RecMET);
      if((h = hist("RecMET_phi")))   h->Fill(phi_RecMET);

      for (unsigned int i=0; i<size; i++) {
        for (it = m_met_signatures_tolook[i]->begin(); it != m_met_signatures_tolook[i]->end(); it++) {
          std::string name = it->first;
          if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
            setCurrentMonGroup(tmp_partialMonGroup + name);
            if((h = hist("RecMET_et")))   h->Fill(et_RecMET);
            if((h = hist("RecMET_sumet")))   h->Fill(sumet_RecMET);
            if((h = hist("RecMET_phi")))   h->Fill(phi_RecMET);
          }
        }
      }
    }
  }

  return sc;
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::getAllMETTriggers() {

  // clear the list from previous run
  m_all_l1_met_triggers.clear();
  m_all_l2_met_triggers.clear();
  m_all_ef_met_triggers.clear();

  const std::vector<std::string> TrigL1Items = getTDT()->getListOfTriggers("L1_.*");
  std::vector<std::string>::const_iterator iter;

  for (iter = TrigL1Items.begin(); iter != TrigL1Items.end(); ++iter) {
    std::string L1name = *iter;
    std::string tmp_trigType = L1name.substr(0,5);
    *m_log << MSG::INFO  << "Trigger Item " << L1name  << " defined " << endreq;
    // store L1 MET trigger item names
    if (tmp_trigType=="L1_TE" || tmp_trigType=="L1_XE")
      m_all_l1_met_triggers.insert(std::map<std::string,int>::value_type(L1name,0));
  }

  const std::vector<std::string> TrigHLTItems = getTDT()->getListOfTriggers("L2_.*|EF_.*");
  std::vector<std::string>::const_iterator chain;

  for (chain= TrigHLTItems.begin(); chain!= TrigHLTItems.end(); ++chain) {
    std::string HLTname = *chain;
    //float prescale = (*chain)->prescale();
    float prescale = getTDT()->getPrescale(*chain);
    // store HLT MET trigger item names divided in L2 and EF
    std::string tmp_trigType =  HLTname.substr(0,5);
    *m_log << MSG::INFO  << "Trigger Item " << HLTname  << " defined with prescale " << prescale << " , " << tmp_trigType<< endreq;
    if (tmp_trigType=="L2_te" || tmp_trigType=="L2_xe") {
      m_all_l2_met_triggers.insert(std::map<std::string,int>::value_type(HLTname,0));
    } else if (tmp_trigType=="EF_te" || tmp_trigType=="EF_xe") {
      m_all_ef_met_triggers.insert(std::map<std::string,int>::value_type(HLTname,0));
    }
  }
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::checkTriggers(std::vector<std::string>& m_triggers,
    bool isInputMETTriggers) {

  const std::vector<std::string> TrigL1Items = getTDT()->getListOfTriggers("L1_.*");
  std::vector<std::string>::const_iterator iter;
  const std::vector<std::string> TrigHLTItems = getTDT()->getListOfTriggers("L2_.*|EF_.*");
  std::vector<std::string>::const_iterator chain;

  for(unsigned int it = 0; it < m_triggers.size(); ++it) { // Loop over trigger items
    std::string item = m_triggers[it];
    std::string tmp_trigLevel =  item.substr(0,3);
    if(m_debuglevel)
      *m_log << MSG::DEBUG  << "signature " << item << " requested" << endreq;
    // check your trigger is defined in the menu
    bool triggerFound = false;
    if (tmp_trigLevel == "L1_") {
      for (iter=TrigL1Items.begin(); iter!=TrigL1Items.end(); ++iter) {
        std::string L1name = *iter;
        if (L1name == item) {
          triggerFound=true;
          if (isInputMETTriggers)
            m_l1_met_signatures.insert(std::map<std::string,int>::value_type(item,0));
        }
      }
      if (!triggerFound) {
        *m_log << MSG::DEBUG  << "L1 signature " << item << " requested but not found in the menu" << endreq;
      }
    } 
    else if (tmp_trigLevel == "L2_" || tmp_trigLevel == "EF_") {
      for (chain=TrigHLTItems.begin(); chain!=TrigHLTItems.end(); ++chain) {
        std::string HLTname = *chain;
        if (HLTname == item) {
          triggerFound=true;
          if (isInputMETTriggers) {
            if (tmp_trigLevel == "L2_") {
              m_l2_met_signatures.insert(std::map<std::string,int>::value_type(item,0));
            }
            else if (tmp_trigLevel == "EF_") {
              m_ef_met_signatures.insert(std::map<std::string,int>::value_type(item,0));
            }
          }
        }
      }
      if (!triggerFound)
        *m_log << MSG::DEBUG << "HLT signature " << item << " requested but not found in the menu" << endreq;
    }
    if(triggerFound && !isInputMETTriggers)
      m_sample_selection_signatures.insert(std::map<std::string,int>::value_type(item,0));
  }
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::printMETTriggerStats() {

  // L1
  *m_log << MSG::INFO  << "Level-1 Met Triggers to look: " << m_l1_met_signatures_tolook->size() << endreq;
  std::map<std::string,int>::iterator iter;
  for (iter = m_l1_met_signatures_tolook->begin(); iter!= m_l1_met_signatures_tolook->end(); ++iter) {
    *m_log << MSG::INFO  << "Events passing " << iter->first << ": " << iter->second << endreq;
  }

  // L2
  *m_log << MSG::INFO  << "Level-2 Met Triggers to look: " << m_l2_met_signatures_tolook->size()<<endreq;
  for (iter = m_l2_met_signatures_tolook->begin(); iter != m_l2_met_signatures_tolook->end(); ++iter) {
    *m_log << MSG::INFO  << "Events passing " << iter->first << ": " << iter->second << endreq;
  }

  // EF
  *m_log << MSG::INFO  << "EF Met Triggers to look: "  << m_ef_met_signatures_tolook->size() << endreq;
  for (iter = m_ef_met_signatures_tolook->begin(); iter != m_ef_met_signatures_tolook->end(); ++iter) {
    *m_log << MSG::INFO  << "Events passing " << iter->first << ": " << iter->second << endreq;
  }
}


//___________________________________________________________________________________________________________
double HLTMETMonTool::signed_delta_phi(double phi1, double phi2) {

  double phia=phi1;
  double phib=phi2;
  if(phi1>M_PI) phia=phi1-2* M_PI;
  if(phi2>M_PI) phib=phi2-2*M_PI;
  double dphi=phia-phib;
  if(dphi>M_PI) dphi-=2*M_PI;
  if(dphi<(-1.*M_PI)) dphi+=2*M_PI;
  return dphi;
}
