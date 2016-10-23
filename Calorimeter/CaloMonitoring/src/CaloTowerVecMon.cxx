/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloTowerVecMon.cxx
//
// AUTHOR:   Lee Sawyer
// CREATED:  1 Feb 2009
// MODIFIED:
//         L. Sawyer  12/2013 Modified booking for new managed histograms 
// Manager Anirvan Sircar June 1st 2010 
//         D. Hu (Jun 2011 - May 2012)
//         L. Sawyer (Dec 2013 - )
// ********************************************************************
#include "CaloMonitoring/CaloTowerVecMon.h"

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "CaloEvent/CaloCell.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;

CaloTowerVecMon::CaloTowerVecMon(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent) :
  ManagedMonitorToolBase(type, name, parent),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_Trigger1(nullptr),
  m_Trigger2(nullptr),
  m_Trigger3(nullptr),
  m_isTrigDefined1(false),
  m_isTrigDefined2(false),
  m_isTrigDefined3(false),
  m_isTrigEvent1(false),
  m_isTrigEvent2(false),
  m_isTrigEvent3(false),
  m_Fcaltower_sumEt(0),
  m_energy_most(0),
  m_eta_most(0),
  m_phi_most (0),
  m_ncells_most(0),
  m_twr_etaphi_Et_thresh(),
  m_twr_e_phi_thresh(),
  m_twr_e_eta_thresh(),
  m_twr_e_phiall_thresh(),
  m_twr_e_etaphi_thresh(),
  m_twr_e_trans_phi_thresh(),
  m_twr_e_trans_eta_thresh(),
  m_twr_e_trans_phiall_thresh(),
  m_twr_e_trans_etaphi_thresh(),
  m_twr_e_total_phi_thresh(),
  m_twr_e_total_eta_thresh(),
  m_twr_e_total_phiall_thresh(),
  m_twr_e_total_etaphi_thresh(),
  m_twr_nCells_phi_thresh(),
  m_twr_nCells_eta_thresh(),
  m_twr_nCells_phiall_thresh(),
  m_twr_nCells_etaphi_thresh(),
  m_twr_phi_thresh(),
  m_twr_eta_thresh(),
  m_twr_phiall_thresh(),
  m_twr_etaphi_thresh()
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("CaloTowerContainer", m_towerContainerName="CombinedTower");

  declareProperty("TimeGran",m_timeGran="run");

  // tools
  declareProperty("useBadLBTool", m_useBadLBTool=false);
  declareProperty("BadLBTool", m_BadLBTool);
  declareProperty("useReadyFilterTool",m_useReadyFilterTool=true);
  declareProperty("ReadyFilterTool",m_ReadyFilterTool);
  declareProperty("useLArNoisyAlg",m_useLArNoisyAlg=false);

  // Trigger Awareness:
  declareProperty("useTrigger",m_useTrigger);
  declareProperty("TriggerNames1",m_TriggerNames1);
  declareProperty("TriggerNames2",m_TriggerNames2);
  declareProperty("TriggerNames3",m_TriggerNames3);
  
  // energy threshold
  declareProperty("NoOfThreshTests", m_threshtests=10);

  declareProperty("noEThresh", m_Ethresh[NO_E]=-999999.0);
  declareProperty("lowEThresh", m_Ethresh[LOW_E]=0.0);
  declareProperty("lowmedEThresh", m_Ethresh[LOWMED_E]=5.0);
  declareProperty("medEThresh", m_Ethresh[MED_E]=10);
  declareProperty("medhiEThresh", m_Ethresh[MEDHIGH_E]=15);
  declareProperty("hiEThresh", m_Ethresh[HIGH_E]=20);
   
  declareProperty("fill_TransverseE", m_filltranse=true);
  declareProperty("fill_TotalE", m_filltotal=true);
  
  initHists();
}

void CaloTowerVecMon::initHists() {
   // twr pre hists
   m_n_calos=nullptr;
   m_n_twrs=nullptr;

   // cell hists 
   m_ratioE_leadcell_tower=nullptr;
   m_etaphi_hotrat_tower=nullptr;
   m_etaphi_avgenergy_hotrat_tower=nullptr;

   //No threshold energy hists
   m_twr_e=nullptr;
   m_postwr_e_thresh=nullptr;
   m_negtwr_e_thresh=nullptr;
   m_twr_et_FcalA0=nullptr;
   m_twr_et_FcalA1=nullptr;
   m_twr_et_FcalA2=nullptr;
   m_twr_et_FcalC0=nullptr;
   m_twr_et_FcalC1=nullptr;
   m_twr_et_FcalC2=nullptr;

   // multi-threshold hists

   for (int i=0; i<m_threshtests; i++) { //loop over thresholds
    m_twr_etaphi_Et_thresh[i]=nullptr;

    for (int j=0; j<3;j++){
     m_twr_e_phi_thresh[i][j]=nullptr;
     m_twr_e_trans_phi_thresh[i][j]=nullptr;
     m_twr_e_total_phi_thresh[i][j]=nullptr;
     m_twr_nCells_phi_thresh[i][j]=nullptr;
     m_twr_phi_thresh[i][j]=nullptr;
    }

    m_twr_e_eta_thresh[i]=nullptr;
    m_twr_e_phiall_thresh[i]=nullptr;
    m_twr_e_etaphi_thresh[i]=nullptr;

    m_twr_e_trans_eta_thresh[i]=nullptr;
    m_twr_e_trans_phiall_thresh[i]=nullptr;
    m_twr_e_trans_etaphi_thresh[i]=nullptr;

    m_twr_e_total_eta_thresh[i]=nullptr;
    m_twr_e_total_phiall_thresh[i]=nullptr;
    m_twr_e_total_etaphi_thresh[i]=nullptr;

    m_twr_nCells_eta_thresh[i]=nullptr;
    m_twr_nCells_phiall_thresh[i]=nullptr;
    m_twr_nCells_etaphi_thresh[i]=nullptr;

    m_twr_eta_thresh[i]=nullptr;
    m_twr_phiall_thresh[i]=nullptr;
    m_twr_etaphi_thresh[i]=nullptr;
   }
   // twr stat hists
   m_twr_et_Fcal0=nullptr;
   m_twr_et_Fcal1=nullptr;
   m_twr_et_Fcal2=nullptr;

   m_twrMost_e=nullptr;
   m_twrMost_etaphi=nullptr;
   m_twrMost_nCells=nullptr;

}

/////////////////////////////////////////////////////////////////////////////////////
CaloTowerVecMon::~CaloTowerVecMon() {}

////////////////////////////////////////////////////////////////////////////////////
StatusCode CaloTowerVecMon::initialize() {
  StatusCode sc = StatusCode::SUCCESS;
  sc = retrieveTools();

  sc = ManagedMonitorToolBase::initialize();
  if(sc.isFailure()){
    ATH_MSG_ERROR("Could not initialize ManagedMonitorToolBase");
    return sc;
  }

  return sc;
}

StatusCode CaloTowerVecMon::retrieveTools(){
  StatusCode sc = StatusCode::SUCCESS;

  // retrieve BadLBFilter tool 
  if(m_useBadLBTool){
   sc = m_BadLBTool.retrieve();
   if(sc.isFailure()){
    ATH_MSG_ERROR("Unable to retrieve the DQBadLBFilterTool");
    m_useBadLBTool = false;
    return sc;
   }
   ATH_MSG_INFO("DQBadLBFilterTool retrieved");
  }

  // retrieve AtlasReadyFilter tool
  if(m_useReadyFilterTool){
   sc = m_ReadyFilterTool.retrieve();
   if(sc.isFailure()) {
     ATH_MSG_ERROR("Could Not Retrieve AtlasReadyFilterTool " << m_ReadyFilterTool);
     m_useReadyFilterTool = false;
     return sc;
   }
   ATH_MSG_INFO("AtlasReadyFilterTool retrieved");
  }

  if( m_useTrigger) {
    if( (!m_TriggerNames1.empty()) || (!m_TriggerNames2.empty()) || (!m_TriggerNames3.empty()) ) {
      sc = m_trigDec.retrieve();
      if( !sc.isSuccess() ) {
        ATH_MSG_ERROR("Unable to retrieve the TrigDecisionTool");
        m_useTrigger = false;
        return sc;
      }
      ATH_MSG_INFO("TrigDecisionTool retrieved");
      if( !m_TriggerNames1.empty() ){
        m_Trigger1 = m_trigDec->getChainGroup(m_TriggerNames1.c_str());
        m_isTrigDefined1 = true;
      }
      else {
        m_isTrigDefined1 = false;
      }
      if( !m_TriggerNames2.empty() ){
        m_Trigger2 = m_trigDec->getChainGroup(m_TriggerNames2.c_str());
        m_isTrigDefined2 = true;
      }
      else {
        m_isTrigDefined2 = false;
      }
      if( !m_TriggerNames3.empty() ){
        m_Trigger3 = m_trigDec->getChainGroup(m_TriggerNames3.c_str());
        m_isTrigDefined3 = true;
      }
      else {
        m_isTrigDefined3 = false;
      }
    }
    else {
      m_useTrigger = false;
    }
  }

  return sc;
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloTowerVecMon::bookHistogramsRecurrent(){
//  Book old-style (unmanaged) histograms. Not used.

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloTowerVecMon::bookHistograms(){
// Book managed histograms. 

  ATH_MSG_DEBUG(" CaloTowerVecMon:in bookHistograms() ");

  Interval_t theinterval = run;
// No longer needed for managed histograms - LS 12/2013
//  bool testTimeGran = checkTimeGran( isNewEventsBlock, isNewLumiBlock, isNewRun, theinterval);

//  if( testTimeGran ) {
   bookTwrPreHists(theinterval);   
   bookCellHists(theinterval);
   bookNothrEnHists(theinterval);
   bookMuthrHists(theinterval);
   bookTwrStatHists(theinterval); 
//  }
  
  return StatusCode::SUCCESS;
}		 

StatusCode CaloTowerVecMon::checkTimeGran(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, Interval_t& theinterval){
  bool isNewTimeGran=isNewRun;
  theinterval=run;
  // but if the choice is different
  if (m_timeGran.compare("run") !=0) {
    if (m_timeGran.compare("eventsblock") ==0) {
      isNewTimeGran=isNewEventsBlock;
      theinterval= eventsBlock;
    }
    else if (m_timeGran.compare("lumiblock") ==0) {
      isNewTimeGran=isNewLumiBlock;
      theinterval=lumiBlock;
    }
    //... check if it is low statistics interval
    else if (m_timeGran.compare("lowStat") ==0){
      isNewTimeGran=newLowStatIntervalFlag();
      theinterval=lowStat;
    }
    //... or medium statistics
    else if (m_timeGran.compare("medStat") ==0) {
      isNewTimeGran=newMedStatIntervalFlag();
      theinterval=medStat;
    }
    else if (m_timeGran.compare("higStat") ==0){
      isNewTimeGran=newHigStatIntervalFlag();
      theinterval=higStat;
    }
    else if (m_timeGran.compare("fill") ==0){
      isNewTimeGran=newLowStatIntervalFlag();
      theinterval=fill;
    }
    else if (m_timeGran.compare("all") ==0){
      isNewTimeGran=newLowStatIntervalFlag();
      theinterval=all;
    }
    // if it is not global, but it is something else stick to medium stat
    else  {
      isNewTimeGran=newLowStatIntervalFlag();
      theinterval=medStat;
    }
  }

  return isNewTimeGran;
}

void CaloTowerVecMon::bookTwrPreHists(const Interval_t theinterval){
  std::string TheTrigger;
  if (m_triggerChainProp == "")  TheTrigger="";
  else TheTrigger = m_triggerChainProp;

  std::string TheTowerDir("/CaloMonitoring/");

  MonGroup calo_towers_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/General", theinterval );
  m_n_calos = new TH1F("NCalos","Number of Calorimeters in Tower", 10, 0., 10.);
  calo_towers_mongroupExpert.regHist( m_n_calos ).ignore();

  m_n_twrs = new TH1F("NTwr","Number of Towers",150,0.,7500.);
  m_n_twrs->GetXaxis()->SetTitle("Number");
  m_n_twrs->GetYaxis()->SetTitle("Events");
  calo_towers_mongroupExpert.regHist( m_n_twrs ).ignore();
}

void CaloTowerVecMon::bookCellHists(const Interval_t theinterval){
  std::string TheTrigger;
  if (m_triggerChainProp == "")  TheTrigger="";
  else TheTrigger = m_triggerChainProp;

  std::string TheTowerDir("/CaloMonitoring/");

  MonGroup towers_leadcell_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/LeadCell", theinterval);    

  m_ratioE_leadcell_tower= new TH1F("ratioE_leadcell_tower","Ratio(E_lead_cell/E_tower)",10000,-100.0,100.0);
  towers_leadcell_mongroupExpert.regHist( m_ratioE_leadcell_tower ).ignore();
  m_ratioE_leadcell_tower->GetXaxis()->SetTitle("Ratio(E_lead_cell/E_tower)");

  m_etaphi_hotrat_tower  = new TH2F("etaphi_hotrat","Hit map of towers with E_Cell/E_tower > 0.9 and E_Tower>0.0 GeV",98,-4.9,4.9,64,-3.15,3.15);
  towers_leadcell_mongroupExpert.regHist( m_etaphi_hotrat_tower).ignore();
  m_etaphi_hotrat_tower->GetXaxis()->SetTitle("#eta");
  m_etaphi_hotrat_tower->GetYaxis()->SetTitle("#phi");
  m_etaphi_hotrat_tower->GetZaxis()->SetMoreLogLabels();

  m_etaphi_avgenergy_hotrat_tower  = new TProfile2D("etaphi_avgenergy_hotrat","Average energy of Towers with E_Cell/E_tower>0.9 and E_Tower>0.0 GeV",98,-4.9,4.9,64,-3.15,3.15);
  towers_leadcell_mongroupExpert.regHist( m_etaphi_avgenergy_hotrat_tower ).ignore();
  m_etaphi_avgenergy_hotrat_tower->GetXaxis()->SetTitle("#eta");
  m_etaphi_avgenergy_hotrat_tower->GetYaxis()->SetTitle("#phi");
}

void CaloTowerVecMon::bookNothrEnHists(const Interval_t theinterval){
  std::string TheTrigger;
  if (m_triggerChainProp == "")  TheTrigger="";
  else TheTrigger = m_triggerChainProp;

  std::string TheTowerDir("/CaloMonitoring/");

  MonGroup calo_towers_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/General", theinterval );  
  MonGroup towers_EtFcal_mongroupShift ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/Et_Fcal", theinterval);    


  m_twr_e = new TH1F("EnergyOfTwr","Tower Energy Distribution", 100,-500.0,500.0);
  m_twr_e->GetXaxis()->SetTitle("Energy");
  m_twr_e->GetYaxis()->SetTitle("Events");
  calo_towers_mongroupExpert.regHist( m_twr_e ).ignore();

  m_postwr_e_thresh = new TH1F("EnergyOfPosTwr","Tower Energy distribution with E_Twr>0.0GeV", 100,0.,500.0);
  m_postwr_e_thresh->GetXaxis()->SetTitle("Energy");
  m_postwr_e_thresh->GetYaxis()->SetTitle("Events");
  calo_towers_mongroupExpert.regHist( m_postwr_e_thresh ).ignore();

  m_negtwr_e_thresh = new TH1F("EnergyOfNegTwr","Tower Energy distribution with E_Twr<0.0GeV", 100,-500.0,0.);
  m_negtwr_e_thresh->GetXaxis()->SetTitle("Energy");
  m_negtwr_e_thresh->GetYaxis()->SetTitle("Events");
  calo_towers_mongroupExpert.regHist( m_negtwr_e_thresh ).ignore();

  std::string title_FcalA0 = "Tower in FcalA Et Distribution without trigger cut";
  m_twr_et_FcalA0 = new TH1F("EtOfTwrInFcalA0",title_FcalA0.c_str(), 60,-10.0,50.0);
  m_twr_et_FcalA0->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_FcalA0->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_FcalA0 ).ignore();

  std::string title_FcalA1 = "Tower in FcalA Et Distribution passing"+ m_TriggerNames1+" or "+m_TriggerNames2;
  m_twr_et_FcalA1 = new TH1F("EtOfTwrInFcalA1",title_FcalA1.c_str(), 60,-10.0,50.0);
  m_twr_et_FcalA1->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_FcalA1->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_FcalA1 ).ignore();

  std::string title_FcalA2 = "Tower in FcalA Et Distribution passing"+ m_TriggerNames3;
  m_twr_et_FcalA2 = new TH1F("EtOfTwrInFcalA2",title_FcalA2.c_str(), 60,-10.0,50.0);
  m_twr_et_FcalA2->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_FcalA2->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_FcalA2 ).ignore();

  std::string title_FcalC0 = "Tower in FcalC Et Distribution without trigger cut";
  m_twr_et_FcalC0 = new TH1F("EtOfTwrInFcalC0",title_FcalC0.c_str(), 60,-10.0,50.0);
  m_twr_et_FcalC0->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_FcalC0->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_FcalC0 ).ignore();

  std::string title_FcalC1 = "Tower in FcalC Et Distribution passing"+ m_TriggerNames1+" or "+m_TriggerNames2;
  m_twr_et_FcalC1 = new TH1F("EtOfTwrInFcalC1",title_FcalC1.c_str(), 60,-10.0,50.0);
  m_twr_et_FcalC1->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_FcalC1->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_FcalC1 ).ignore();

  std::string title_FcalC2 = "Tower in FcalC Et Distribution passing"+ m_TriggerNames3;
  m_twr_et_FcalC2 = new TH1F("EtOfTwrInFcalC2",title_FcalC2.c_str(), 60,-10.0,50.0);
  m_twr_et_FcalC2->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_FcalC2->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_FcalC2 ).ignore();
}

void CaloTowerVecMon::bookMuthrHists(const Interval_t theinterval){
  std::string TheTrigger;
  if (m_triggerChainProp == "")  TheTrigger="";
  else TheTrigger = m_triggerChainProp;

  std::string TheTowerDir("/CaloMonitoring/");

  MonGroup towers_1davge_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/1d_AvEnergy", theinterval);    //EXPERT
  MonGroup towers_2davge_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/2d_AvEnergy", theinterval);    //EXPERT
  MonGroup towers_2davge_mongroupShift ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/2d_AvEnergy", theinterval);    //SHIFT
  MonGroup towers_1drates_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/1d_Rates", theinterval);    //EXPERT 
  MonGroup towers_2drates_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/2d_Rates", theinterval);    //SHIFT
  MonGroup towers_2drates_mongroupShift ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/2d_Rates", theinterval);    //SHIF
  MonGroup towers_total_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/TotalEnergy", theinterval);    //EXPERT
  MonGroup towers_transe_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/TransEnergy", theinterval);    //EXPERT
  MonGroup towers_ncells_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/NCells", theinterval);    //EXPERT

  for (int i=0; i<m_threshtests; i++) { //loop over thresholds
    char bname[256];
    char btitle[256];

    sprintf(bname, "TwrOccupancyVsEtaPhithresh_Et_%d",i);
    sprintf(btitle, "Tower Occupancy Vs #eta - #phi with Et_Twr> %4.1f GeV", m_Ethresh[i]);
    m_twr_etaphi_Et_thresh[i] =  new TH2F(bname,btitle,98,-4.9,4.9, 64,-3.15,3.15);
    m_twr_etaphi_Et_thresh[i]->GetXaxis()->SetTitle("#eta");
    m_twr_etaphi_Et_thresh[i]->GetYaxis()->SetTitle("#phi");
    towers_2drates_mongroupExpert.regHist(m_twr_etaphi_Et_thresh[i] ).ignore();

    std::string Region;
    for (int j=0; j<3;j++){
      switch(j){
        case 0:
          Region="Barrel";
          break;
        case 1:
          Region="EndcapA";
          break;
        case 2:
          Region="EndcapC";
          break;
      }

      sprintf(bname, "AvgEnergyOfTwrVsPhithresh_%s_%d", Region.c_str(),i);
      sprintf(btitle, "AvgEnergy of Tower %s Vs #phi with E_Twr> %4.1f GeV", Region.c_str(),m_Ethresh[i]);
      m_twr_e_phi_thresh[i][j] = new TProfile(bname, btitle, 64,-3.15,3.15);
      m_twr_e_phi_thresh[i][j]->GetXaxis()->SetTitle("#phi");
      m_twr_e_phi_thresh[i][j]->GetYaxis()->SetTitle("Average Energy");
      m_twr_e_phi_thresh[i][j]->GetYaxis()->SetMoreLogLabels();
      towers_1davge_mongroupExpert.regHist( m_twr_e_phi_thresh[i][j] ).ignore();

      if (m_filltranse) {
       sprintf(bname, "TransEnergyOfTwrVsPhithresh_%s_%d", Region.c_str(),i);
       sprintf(btitle, "TransEnergy of Tower %s Vs #phi with E_Twr> %4.1f GeV", Region.c_str(),m_Ethresh[i]);
       m_twr_e_trans_phi_thresh[i][j] = new TProfile(bname, btitle, 64,-3.15,3.15);
       m_twr_e_trans_phi_thresh[i][j]->GetXaxis()->SetTitle("#phi");
       m_twr_e_trans_phi_thresh[i][j]->GetYaxis()->SetTitle("Average Energy");
       m_twr_e_trans_phi_thresh[i][j]->GetYaxis()->SetMoreLogLabels();
       towers_transe_mongroupExpert.regHist( m_twr_e_trans_phi_thresh[i][j] ).ignore();
      }

      if (m_filltotal) {
       sprintf(bname, "TotalEnergyOfTwrVsPhithresh_%s_%d", Region.c_str(),i);
       sprintf(btitle, "TotalEnergy of Tower %s Vs #phi with E_Twr> %4.1f GeV", Region.c_str(),m_Ethresh[i]);
       m_twr_e_total_phi_thresh[i][j] = new TH1F(bname, btitle,64,-3.15,3.15);
       m_twr_e_total_phi_thresh[i][j]->GetXaxis()->SetTitle("#phi");
       m_twr_e_total_phi_thresh[i][j]->GetYaxis()->SetTitle("Average Energy");
       m_twr_e_total_phi_thresh[i][j]->GetYaxis()->SetMoreLogLabels();
       towers_total_mongroupExpert.regHist( m_twr_e_total_phi_thresh[i][j] ).ignore();
      }

      sprintf(bname, "TowersAvgnCellsVsPhithresh_%s_%d", Region.c_str(),i);
      sprintf(btitle, "AvgNumber of cells in each %s Tower Vs #phi with E_Twr>%4.1f GeV ", Region.c_str(),m_Ethresh[i]);
      m_twr_nCells_phi_thresh[i][j] =  new TProfile(bname,btitle, 64,-3.15,3.15);
      m_twr_nCells_phi_thresh[i][j]->GetXaxis()->SetTitle("#Phi");
      m_twr_nCells_phi_thresh[i][j]->GetYaxis()->SetTitle("AvgNumber of cells ");
      m_twr_nCells_phi_thresh[i][j]->GetYaxis()->SetMoreLogLabels();
      towers_ncells_mongroupExpert.regHist( m_twr_nCells_phi_thresh[i][j] ).ignore();

      sprintf(bname, "TwrOccupancyVsPhithresh_%s_%d", Region.c_str(),i);
      sprintf(btitle, "Tower %s Occupancy Vs #phi with E_Twr> %4.1f GeV ",Region.c_str(),m_Ethresh[i]);
      m_twr_phi_thresh[i][j] =  new TH1F(bname,btitle, 64,-3.15,3.15);
      m_twr_phi_thresh[i][j]->GetXaxis()->SetTitle("#phi");
      m_twr_phi_thresh[i][j]->GetYaxis()->SetTitle("Events");
      m_twr_phi_thresh[i][j]->GetYaxis()->SetMoreLogLabels();
      towers_1drates_mongroupExpert.regHist(m_twr_phi_thresh[i][j] ).ignore();
    } //end region loop 

    sprintf(bname, "AvgEnergyOfTwrVsEtathresh_%d",i);
    sprintf(btitle, "AvgEnergy of Tower Vs #eta for E_Twr> %4.1f GeV", m_Ethresh[i]);
    m_twr_e_eta_thresh[i] = new TProfile(bname, btitle, 98,-4.9,4.9);
    m_twr_e_eta_thresh[i]->GetXaxis()->SetTitle("#eta");
    m_twr_e_eta_thresh[i]->GetYaxis()->SetTitle("Average Energy");
    m_twr_e_eta_thresh[i]->GetYaxis()->SetMoreLogLabels();
    towers_1davge_mongroupExpert.regHist( m_twr_e_eta_thresh[i] ).ignore();

    sprintf(bname, "AvgEnergyOfTwrVsFullPhithresh_%d",i);
    sprintf(btitle, "AvgEnergy of Tower Vs #phi E_Twr > %4.1f GeV", m_Ethresh[i]);
    m_twr_e_phiall_thresh[i] = new TProfile(bname, btitle, 64,-3.15,3.15);
    m_twr_e_phiall_thresh[i]->GetXaxis()->SetTitle("#phi");
    m_twr_e_phiall_thresh[i]->GetYaxis()->SetTitle("Average Energy");
    m_twr_e_phiall_thresh[i]->GetYaxis()->SetMoreLogLabels();
    towers_1davge_mongroupExpert.regHist( m_twr_e_phiall_thresh[i] ).ignore();

    sprintf(bname, "AvgEnergyOfTwrVsEtaPhithresh_%d",i);
    sprintf(btitle, "AvgEnergy of Tower Vs #eta-#phi for E_Twr> %4.1f GeV", m_Ethresh[i]);
    m_twr_e_etaphi_thresh[i] = new TProfile2D(bname, btitle,98,-4.9,4.9,64,-3.15,3.15);
    m_twr_e_etaphi_thresh[i]->GetXaxis()->SetTitle("#eta");
    m_twr_e_etaphi_thresh[i]->GetYaxis()->SetTitle("#phi");
    if(i==0 || i==2) towers_2davge_mongroupShift.regHist( m_twr_e_etaphi_thresh[i] ).ignore();
    else towers_2davge_mongroupExpert.regHist( m_twr_e_etaphi_thresh[i] ).ignore();

    if(m_filltranse) {
     sprintf(bname, "TransEnergyOfTwrVsEtathresh_%d",i);
     sprintf(btitle, "TransEnergy of Tower Vs #eta for E_Twr> %4.1f GeV", m_Ethresh[i]);
     m_twr_e_trans_eta_thresh[i] = new TProfile(bname, btitle, 98,-4.9,4.9);
     m_twr_e_trans_eta_thresh[i]->GetXaxis()->SetTitle("#eta");
     m_twr_e_trans_eta_thresh[i]->GetYaxis()->SetTitle("Average Energy");
     towers_transe_mongroupExpert.regHist( m_twr_e_trans_eta_thresh[i] ).ignore();

     sprintf(bname, "TransEnergyOfTwrVsFullPhithresh_%d",i);
     sprintf(btitle, "TransEnergy of Tower Vs #phi E_Twr > %4.1f GeV", m_Ethresh[i]);
     m_twr_e_trans_phiall_thresh[i] = new TProfile(bname, btitle, 64,-3.15,3.15);
     m_twr_e_trans_phiall_thresh[i]->GetXaxis()->SetTitle("#phi");
     m_twr_e_trans_phiall_thresh[i]->GetYaxis()->SetTitle("Average Energy");
     m_twr_e_trans_phiall_thresh[i]->GetYaxis()->SetMoreLogLabels();
     towers_transe_mongroupExpert.regHist( m_twr_e_trans_phiall_thresh[i] ).ignore();

     sprintf(bname, "TransEnergyOfTwrVsEtaPhithresh_%d",i);
     sprintf(btitle, "TransEnergy of Tower Vs #eta-#phi for E_Twr> %4.1f GeV", m_Ethresh[i]);
     m_twr_e_trans_etaphi_thresh[i] = new TProfile2D(bname, btitle,98,-4.9,4.9,64,-3.15,3.15);
     m_twr_e_trans_etaphi_thresh[i]->GetXaxis()->SetTitle("#eta");
     m_twr_e_trans_etaphi_thresh[i]->GetYaxis()->SetTitle("#phi");
     towers_transe_mongroupExpert.regHist( m_twr_e_trans_etaphi_thresh[i] ).ignore();
    }

    if (m_filltotal) {
     sprintf(bname, "TotalEnergyOfTwrVsEtathresh_%d",i);
     sprintf(btitle, "TotalEnergy of Tower Vs #eta for E_Twr> %4.1f GeV", m_Ethresh[i]);
     m_twr_e_total_eta_thresh[i] = new TH1F(bname, btitle, 98,-4.9,4.9);
     m_twr_e_total_eta_thresh[i]->GetXaxis()->SetTitle("#eta");
     m_twr_e_total_eta_thresh[i]->GetYaxis()->SetTitle("Total Energy");
     towers_total_mongroupExpert.regHist( m_twr_e_total_eta_thresh[i] ).ignore();

     sprintf(bname, "TotalEnergyOfTwrVsFullPhithresh_%d",i);
     sprintf(btitle, "TotalEnergy of Tower Vs #phi E_Twr > %4.1f GeV", m_Ethresh[i]);
     m_twr_e_total_phiall_thresh[i] = new TH1F(bname,btitle, 64,-3.15,3.15);
     m_twr_e_total_phiall_thresh[i]->GetXaxis()->SetTitle("#phi");
     m_twr_e_total_phiall_thresh[i]->GetYaxis()->SetTitle("Total Energy");
     m_twr_e_total_phiall_thresh[i]->GetYaxis()->SetMoreLogLabels();
     towers_total_mongroupExpert.regHist( m_twr_e_total_phiall_thresh[i] ).ignore();

     sprintf(bname, "TotalEnergyOfTwrVsEtaPhithresh_%d",i);
     sprintf(btitle, "TotalEnergy of Tower Vs #eta-#phi for E_Twr> %4.1f GeV", m_Ethresh[i]);
     m_twr_e_total_etaphi_thresh[i] = new TH2F(bname, btitle,98,-4.9,4.9,64,-3.15,3.15);
     m_twr_e_total_etaphi_thresh[i]->GetXaxis()->SetTitle("#eta");
     m_twr_e_total_etaphi_thresh[i]->GetYaxis()->SetTitle("#phi");
     towers_total_mongroupExpert.regHist( m_twr_e_total_etaphi_thresh[i] ).ignore();
    }

    sprintf(bname, "TowersAvgnCellsVsEtathresh_%d",i);
    sprintf(btitle, "AvgNumber of cells in each Tower with E_Twr>%4.1f GeV Vs #eta ", m_Ethresh[i]);
    m_twr_nCells_eta_thresh[i] =  new TProfile(bname, btitle, 98,-4.9,4.9);
    m_twr_nCells_eta_thresh[i]->GetXaxis()->SetTitle("#eta");
    m_twr_nCells_eta_thresh[i]->GetYaxis()->SetTitle("AvgNumber of cells ");
    m_twr_nCells_eta_thresh[i]->GetYaxis()->SetMoreLogLabels();
    towers_ncells_mongroupExpert.regHist( m_twr_nCells_eta_thresh[i] ).ignore();

    sprintf(bname, "TowersAvgnCellsVsFullPhithresh_%d",i);
    sprintf(btitle, "AvgNumber of cells in each Tower with E_Twr>%4.1f GeV vs #phi ", m_Ethresh[i]);
    m_twr_nCells_phiall_thresh[i] =  new TProfile(bname,btitle, 64,-3.15,3.15);
    m_twr_nCells_phiall_thresh[i]->GetXaxis()->SetTitle("#phi");
    m_twr_nCells_phiall_thresh[i]->GetYaxis()->SetTitle("AvgNumber of cells ");
    m_twr_nCells_phiall_thresh[i]->GetYaxis()->SetMoreLogLabels();
    towers_ncells_mongroupExpert.regHist( m_twr_nCells_phiall_thresh[i] ).ignore();

    sprintf(bname, "TowersAvgnCellsVsEtaPhithresh_%d",i);
    sprintf(btitle, "AvgNumber of cells in each Tower with E_Twr>%4.1f GeV Vs #eta-#Phi ", m_Ethresh[i]);
    m_twr_nCells_etaphi_thresh[i] =  new TProfile2D(bname,btitle, 98,-4.9,4.9,64,-3.15,3.15);
    m_twr_nCells_etaphi_thresh[i]->GetXaxis()->SetTitle("#eta");
    m_twr_nCells_etaphi_thresh[i]->GetYaxis()->SetTitle("#phi");
    m_twr_nCells_etaphi_thresh[i]->GetYaxis()->SetMoreLogLabels();
    m_twr_nCells_etaphi_thresh[i]->GetZaxis()->SetMoreLogLabels();
    towers_ncells_mongroupExpert.regHist( m_twr_nCells_etaphi_thresh[i] ).ignore();

    sprintf(bname, "TwrOccupancyVsEtathresh_%d",i);
    sprintf(btitle, "Tower Occupancy Vs Eta with E_Twr> %4.1f GeV", m_Ethresh[i]);
    m_twr_eta_thresh[i] =  new TH1F(bname,btitle, 98,-4.9,4.9);
    m_twr_eta_thresh[i]->GetXaxis()->SetTitle("Eta");
    m_twr_eta_thresh[i]->GetYaxis()->SetTitle("Events");
    m_twr_eta_thresh[i]->GetYaxis()->SetMoreLogLabels();
    towers_1drates_mongroupExpert.regHist(m_twr_eta_thresh[i] ).ignore();

    sprintf(bname, "TwrOccupancyVsFullPhithresh_%d",i);
    sprintf(btitle, "Tower Occupancy Vs #phi with E_Twr> %4.1f GeV", m_Ethresh[i]);
    m_twr_phiall_thresh[i] =  new TH1F(bname,btitle, 64,-3.15,3.15);
    m_twr_phiall_thresh[i]->GetXaxis()->SetTitle("#phi");
    m_twr_phiall_thresh[i]->GetYaxis()->SetTitle("Events");
    m_twr_phiall_thresh[i]->GetYaxis()->SetMoreLogLabels();
    towers_1drates_mongroupExpert.regHist(m_twr_phiall_thresh[i] ).ignore();

    sprintf(bname, "TwrOccupancyVsEtaPhithresh_%d",i);
    sprintf(btitle, "Tower Occupancy Vs #eta - #phi with E_Twr> %4.1f GeV", m_Ethresh[i]);
    m_twr_etaphi_thresh[i] =  new TH2F(bname,btitle,98,-4.9,4.9, 64,-3.15,3.15);
    m_twr_etaphi_thresh[i]->GetXaxis()->SetTitle("#eta");
    m_twr_etaphi_thresh[i]->GetYaxis()->SetTitle("#phi");
    m_twr_etaphi_thresh[i]->GetZaxis()->SetLabelSize(0.8);
    m_twr_etaphi_thresh[i]->GetZaxis()->SetMoreLogLabels();
    if(i==0 || i==2) towers_2drates_mongroupShift.regHist(m_twr_etaphi_thresh[i] ).ignore();
    else towers_2drates_mongroupExpert.regHist(m_twr_etaphi_thresh[i] ).ignore();
   }// end thresholds loop
}

void CaloTowerVecMon::bookTwrStatHists(const Interval_t theinterval){
  std::string TheTrigger;
  if (m_triggerChainProp == "")  TheTrigger="";
  else TheTrigger = m_triggerChainProp;

  std::string TheTowerDir("/CaloMonitoring/");

  MonGroup towers_EtFcal_mongroupShift ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/Et_Fcal", theinterval);    
  MonGroup calo_towers_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/General", theinterval );  
  MonGroup towers_2davge_mongroupExpert ( this, TheTowerDir+m_towerContainerName+TheTrigger+"/2d_AvEnergy", theinterval);    //EXPERT

  std::string title_Fcal0 = "sum of Towers in Fcal Et Distribution without trigger cut";
  m_twr_et_Fcal0 = new TH1F("EtOfTwrInFcal0",title_Fcal0.c_str(), 204,-200.0,10000.0);
  m_twr_et_Fcal0->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_Fcal0->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_Fcal0 ).ignore();

  std::string title_Fcal1 = "sum of Towers in Fcal Et Distribution passing"+ m_TriggerNames1+" or "+m_TriggerNames2;
  m_twr_et_Fcal1 = new TH1F("EtOfTwrInFcal1",title_Fcal1.c_str(), 204,-200.0,10000.0);
  m_twr_et_Fcal1->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_Fcal1->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_Fcal1 ).ignore();

  std::string title_Fcal2 = "sum of Tower in Fcal Et Distribution passing"+ m_TriggerNames3;
  m_twr_et_Fcal2 = new TH1F("EtOfTwrInFcal2",title_Fcal2.c_str(), 204,-200.0,10000.0);
  m_twr_et_Fcal2->GetXaxis()->SetTitle("Et(GeV)");
  m_twr_et_Fcal2->GetYaxis()->SetTitle("Events");
  towers_EtFcal_mongroupShift.regHist( m_twr_et_Fcal2 ).ignore();

  m_twrMost_e = new TH1F("TowerMostEnergy","Energy in Most Energetic Tower",100,0., 50000.);
  m_twrMost_e->GetXaxis()->SetTitle("Most Energetic Tower Energy");
  m_twrMost_e->GetYaxis()->SetTitle("Events");
  calo_towers_mongroupExpert.regHist( m_twrMost_e ).ignore();

  m_twrMost_nCells = new TH1F("TowerMostnCells","Number of cells in Most Energetic Tower", 50, 0., 500.);
  m_twrMost_nCells->GetXaxis()->SetTitle("Number of cells in Most Energetic Tower");
  m_twrMost_nCells->GetXaxis()->SetTitle("Events");
  calo_towers_mongroupExpert.regHist( m_twrMost_nCells ).ignore();

  m_twrMost_etaphi = new TH2F("TowerMostEnergyEtaPhi","EtaPhi of Most Energetic Tower", 98, -4.9, 4.9, 64, -3.15, 3.15);
  m_twrMost_etaphi->GetXaxis()->SetTitle("#eta");
  m_twrMost_etaphi->GetYaxis()->SetTitle("#phi");
  towers_2davge_mongroupExpert.regHist( m_twrMost_etaphi ).ignore();
}

//////////////////////////////////////////////////////////////////////////////////
StatusCode CaloTowerVecMon::fillHistograms() {

 StatusCode sc = StatusCode::SUCCESS;

 bool ifPass = 1;
 sc = checkFilters(ifPass);
 if(sc.isFailure() || !ifPass) return StatusCode::SUCCESS;
 
 fillTrigPara();

 initTwrStat(); 

  const CaloTowerContainer* towerCont = nullptr;
  sc=evtStore()->retrieve( towerCont, m_towerContainerName);
  if( sc.isFailure()  ||  !towerCont ) {
    ATH_MSG_WARNING( " No AOD CaloTower container found in TDS "  );
    return sc;
  }  

  fillTwrPreHists(towerCont);

  CaloTowerContainer::const_iterator it = towerCont->begin(); 
  CaloTowerContainer::const_iterator it_e = towerCont->end(); 
  for ( ; it!=it_e;++it) { 
    const CaloTower* twr = *it;

    fillCellHists(twr);

    fillNothrEnHists(twr);
    fillMuthrHists(twr);

    fillTwrStat(twr);
  }

  fillTwrStatHists();

  return sc;
}

StatusCode CaloTowerVecMon::checkFilters(bool& ifPass){
 StatusCode sc = StatusCode::SUCCESS;

 if(m_useLArNoisyAlg) {
   const xAOD::EventInfo* thisEventInfo;
   sc = evtStore()->retrieve(thisEventInfo);
   if (sc!=StatusCode::SUCCESS){
     m_useLArNoisyAlg = false;
     ATH_MSG_WARNING("No EventInfo object found! Can't read run number!");
   }
   else{
     if (thisEventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error) ifPass = 0;
   }
 }

 if(m_useReadyFilterTool){
   if(!m_ReadyFilterTool->accept()) ifPass = 0;
 }

 if(m_useBadLBTool){
   if(!m_BadLBTool->accept()) ifPass = 0;
 }

 if(sc!=StatusCode::SUCCESS){
  ATH_MSG_WARNING("failure in checkFilters");
 }

 return StatusCode::SUCCESS;
}

void CaloTowerVecMon::fillTrigPara(){
 m_isTrigEvent1 = false;
 m_isTrigEvent2 = false;
 m_isTrigEvent3 = false;

 if(m_useTrigger){

    if( m_isTrigDefined1 ) {
      m_isTrigEvent1 = m_Trigger1->isPassed();
    }
    if( m_isTrigDefined2 ) {
      m_isTrigEvent2 = m_Trigger2->isPassed();
    }
    if( m_isTrigDefined3 ) {
      m_isTrigEvent3 = m_Trigger3->isPassed();
    }
 }
 else{
  m_isTrigEvent1 = true;
  m_isTrigEvent2 = true;
  m_isTrigEvent3 = true;
 }
 
}

void CaloTowerVecMon::fillTwrPreHists(const CaloTowerContainer* towerCont ){
  float ncalos = towerCont->getNumberOfCalos();
  m_n_calos->Fill(ncalos);
  float ntwrs = towerCont->size();
  m_n_twrs->Fill(ntwrs);

}

void CaloTowerVecMon::fillCellHists(const CaloTower* twr){
  float etaTwr = twr->eta();
  float phiTwr = twr->phi();
  float eTwr = twr->energy()/GeV;
  CaloTower::cell_iterator cellIter = twr->cell_begin();
  CaloTower::cell_iterator cellIterEnd = twr->cell_end();
  int cellcount=0;
  float maxCellEnergy=0.;

  for ( ;cellIter !=cellIterEnd;cellIter++) {
    ++cellcount;
    const CaloCell* cell= (*cellIter);
    float EnergyCell=cell->energy()/GeV;
    if (cellcount == 1 || fabs(EnergyCell) > fabs(maxCellEnergy)) {
      maxCellEnergy= EnergyCell;
    }
  } 

  float ratio=0;
  if(eTwr > 0.) ratio = maxCellEnergy/eTwr;

  if(fabs(etaTwr) < 5.0) {
    if(eTwr > 0.0) {
      m_ratioE_leadcell_tower->Fill( ratio );
      if( ratio > 0.9 ) {
        m_etaphi_hotrat_tower->Fill(etaTwr,phiTwr);
        m_etaphi_avgenergy_hotrat_tower->Fill(etaTwr,phiTwr,eTwr);
      }
    }
  }

}

void CaloTowerVecMon::fillNothrEnHists(const CaloTower* twr){
    float eTwr = twr->energy()/GeV;
    float etaTwr = twr->eta();
    float etTwr = twr->et()/GeV;

    m_twr_e->Fill(eTwr,1.);
    if (eTwr > 0.)  m_postwr_e_thresh->Fill(eTwr,1.); 
    else m_negtwr_e_thresh->Fill(eTwr,1.); 

    if(etaTwr>3.1 && etaTwr<5.0){
     m_twr_et_FcalA0->Fill(etTwr,1.);
     if(m_isTrigEvent1 || m_isTrigEvent2) m_twr_et_FcalA1->Fill(etTwr,1.);
     if(m_isTrigEvent3) m_twr_et_FcalA2->Fill(etTwr,1.);
    }

    if(etaTwr<(-3.1) && etaTwr>(-5.0)){
     m_twr_et_FcalC0->Fill(etTwr,1.);
     if(m_isTrigEvent1 || m_isTrigEvent2) m_twr_et_FcalC1->Fill(etTwr,1.);
     if(m_isTrigEvent3) m_twr_et_FcalC2->Fill(etTwr,1.);
    }
}

void CaloTowerVecMon::fillMuthrHists(const CaloTower* twr){
    float eTwr = twr->energy()/GeV;
    float etTwr = twr->et()/GeV;
    float etaTwr = twr->eta();
    float phiTwr = twr->phi();
    int ncellsTwr = twr->getNumberOfCells(); 
    unsigned int reg_i=0;
    if (fabs(etaTwr)<1.5) reg_i=0;
    else if (etaTwr > 1.5) reg_i=1;
    else reg_i=2;

    for (int j=0;j<m_threshtests;j++) {
        if( etTwr > m_Ethresh[j]) m_twr_etaphi_Et_thresh[j]->Fill(etaTwr,phiTwr,etTwr); 

        if(eTwr > m_Ethresh[j]) {
          m_twr_e_phi_thresh[j][reg_i]->Fill(phiTwr,eTwr);
          m_twr_e_eta_thresh[j]->Fill(etaTwr,eTwr );
          m_twr_e_phiall_thresh[j]->Fill(phiTwr,eTwr);
          m_twr_e_etaphi_thresh[j]->Fill(etaTwr,phiTwr,eTwr);          
           
          if (m_filltranse) {
           m_twr_e_trans_phi_thresh[j][reg_i]->Fill(phiTwr,etTwr);
           m_twr_e_trans_eta_thresh[j]->Fill(etaTwr,etTwr );
           m_twr_e_trans_phiall_thresh[j]->Fill(phiTwr,etTwr );
           m_twr_e_trans_etaphi_thresh[j]->Fill(etaTwr,phiTwr,etTwr);
          }

          if (m_filltotal) {
           m_twr_e_total_phi_thresh[j][reg_i]->Fill(phiTwr,eTwr);
           m_twr_e_total_eta_thresh[j]->Fill(etaTwr,eTwr );
           m_twr_e_total_phiall_thresh[j]->Fill(phiTwr,eTwr );
           m_twr_e_total_etaphi_thresh[j]->Fill(etaTwr,phiTwr,eTwr);
          }

          m_twr_nCells_phi_thresh[j][reg_i]->Fill(phiTwr,ncellsTwr);
          m_twr_nCells_eta_thresh[j]->Fill(etaTwr,ncellsTwr );
          m_twr_nCells_phiall_thresh[j]->Fill(phiTwr,ncellsTwr );
          m_twr_nCells_etaphi_thresh[j]->Fill(etaTwr,phiTwr,ncellsTwr);

          m_twr_phi_thresh[j][reg_i]->Fill(phiTwr,1.0);
          m_twr_eta_thresh[j]->Fill(etaTwr,1.0);
          m_twr_phiall_thresh[j]->Fill(phiTwr,1.0);
          m_twr_etaphi_thresh[j]->Fill(etaTwr,phiTwr,1.0);

        } // end of (eTwr > m_Ethresh[j]) 
    } // end of j loop 
}

void CaloTowerVecMon::initTwrStat(){
  m_Fcaltower_sumEt = 0;
  m_energy_most =       0.;
  m_eta_most    =       0.;
  m_phi_most    =       0.;
  m_ncells_most =       0 ;
}

void CaloTowerVecMon::fillTwrStat(const CaloTower* twr){
    float etaTwr = twr->eta();
    float phiTwr = twr->phi();
    float eTwr = twr->energy()/GeV;
    float etTwr = eTwr/cosh(etaTwr);
    int ncellsTwr = twr->getNumberOfCells();

    if(fabs(etaTwr)>3.1 && fabs(etaTwr)<5.0){
     m_Fcaltower_sumEt = m_Fcaltower_sumEt+etTwr;
    }

    if ( eTwr > m_energy_most ) {
      m_energy_most = eTwr;
      m_eta_most    = etaTwr;
      m_phi_most    = phiTwr;
      m_ncells_most = ncellsTwr;
    }
}

void CaloTowerVecMon::fillTwrStatHists(){
  m_twr_et_Fcal0->Fill(m_Fcaltower_sumEt,1.);
  if(m_isTrigEvent1 || m_isTrigEvent2) m_twr_et_Fcal1->Fill(m_Fcaltower_sumEt,1.);
  if(m_isTrigEvent3) m_twr_et_Fcal2->Fill(m_Fcaltower_sumEt,1.);

  m_twrMost_e->Fill(m_energy_most,1.);
  m_twrMost_etaphi->Fill(m_eta_most, m_phi_most, m_energy_most);
  m_twrMost_nCells->Fill(m_ncells_most,1.);
}

StatusCode CaloTowerVecMon::procHistograms( ){  
  ATH_MSG_DEBUG( "CaloTowerVecMon default procHistograms() is starting"  );
  return StatusCode::SUCCESS;
}
