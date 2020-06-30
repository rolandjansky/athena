/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloClusterVecMon.cxx
//
// AUTHORS:   R. Kehoe    06/15/2006
//            S.Chekanov (ANL, TileCal part,  08/04/2008
//            S.Chekanov (ANL, ratio for clusters reflecting calibration,  09/03/2008
//            S.Chekanov. Removed extra token
//            S.Chekanov 10/21/2008 Corrected Phi distributions 
//            L. Sawyer  12/2013 Modified booking for new managed histograms 
// MANAGERS:  H. Hadavand   
//            R. Dhullipudi (01/07/2008)
//            D. Hu (Jun 2011 - May 2012)   
//            L. Sawyer (2013 - )
//           
//********************************************************************
              

#include "CaloClusterVecMon.h"

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

#include "RecBackgroundEvent/BeamBackgroundData.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaKernel/Units.h"
#include <math.h>


using xAOD::CaloCluster;
using Athena::Units::GeV;

/* Obsolete with C++11
// sorting alg for clusters 
namespace GetSort{
  class ClusSort{
  public:
    bool operator()(CaloCluster* o1, CaloCluster* o2) {
      return o1->e() > o2->e();
    }
  };
}

*/

CaloClusterVecMon::CaloClusterVecMon(const std::string& type, const std::string& name, const IInterface* parent) :
  CaloMonToolBase(type, name, parent),
  m_caloMgr(nullptr),
  m_caloCellHelper(nullptr),
  m_cluscount(0),
  m_cluscount_top(0),
  m_cluscount_bot(0),
  m_maxclusindex_top(0),
  m_maxclusindex_bot(0),
  m_maxclusene_top(0),
  m_maxclusene_bot(0),
  m_EMenergy(0),
  m_EMenergy_abs(0),
  m_EMet(0),
  m_EMeta(0),
  m_EMphi(0)
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("TimeGran",m_timeGran="lowStat");

  declareProperty("lowEthresh", m_Ethresh[LOW_E]=0.0);
  declareProperty("lowmedEthresh", m_Ethresh[LOWMED_E]=5.0);
  declareProperty("medEthresh", m_Ethresh[MED_E]=10.0);
  declareProperty("medhiEthresh", m_Ethresh[MEDHIGH_E]=15.0);
  declareProperty("hiEthresh", m_Ethresh[HIGH_E]=20.0);

  declareProperty("etaMin1", m_etaMin[REGION1]=0.0);
  declareProperty("etaMin2", m_etaMin[REGION2]=1.4);
  declareProperty("etaMin3", m_etaMin[REGION3]=2.0);
  declareProperty("timeWindowWidth",m_tWidth=0.5);

  declareProperty("energyThreshold",m_Threshold=300.); //Threshold in MeV
  // histogram bins
  declareProperty("binsClustersE", m_binRangeE);
  declareProperty("binsClustersEta", m_binRangeEta);
  declareProperty("binsClustersPhi", m_binRangePhi);
  declareProperty("binsClustersEtaPhi", m_binRangeEtaPhi);

  initHists(); 
}


CaloClusterVecMon::~CaloClusterVecMon() {
}

void CaloClusterVecMon::initHists(){
 // cell hists 
 m_eventsCounter = 0; // km add
 m_nCells=nullptr;
 m_maxEcellToEclusterRatio=nullptr;
 m_dominantCellOccupancy_etaphi=nullptr;
 m_dominantCellAverageEnergy_etaphi=nullptr; 
 m_nCellInCluster_etaphi=nullptr;
 m_clusterTimeVsEnergy=nullptr;
 m_clusterTime=nullptr;
 m_cellTime=nullptr;
 m_cellvsclust_time=nullptr; 

 // mult-threshold cluster hists
 for (int iE=0; iE<MAX_E; iE++){
   m_clus_etaphi_Et_thresh[iE]=nullptr;
   m_etaphi_thresh_avgEt[iE]=nullptr;
//   m_EMclus_etaphi_Et_thresh[iE]=0;
//   m_EMclus_etaVsPhi[iE]=0;
   m_clus_eta[iE]=nullptr;
   m_clus_eta_Et[iE]=nullptr;  // km add
   for(int j_plot=0; j_plot<3; j_plot++){
   m_clus_phi[iE][j_plot]=nullptr;
   m_clus_phi_Et[iE][j_plot]=nullptr;} // km add
   m_etaVsPhi[iE]=nullptr;
   m_etaphi_thresh_avgenergy[iE]=nullptr;
   m_etaphi_thresh_Totalenergy[iE]=nullptr;
  }

 // negative energy cluster hists 
  m_etaVsPhiNegEn=nullptr;
  m_averageNegativeEnergy_etaphi=nullptr;

  // no threshold cluster hists
  m_averageEnergy_phi=nullptr;
  m_averageEnergy_eta=nullptr;

  m_clusterEnergyVsEta_barrel=nullptr;
  m_clusterEnergyVsEta_endcap=nullptr;
  m_clusterEnergyVsEta_hecfcal=nullptr;
  m_clusterEtVsEta_barrel=nullptr; // for non-cosmics
  m_clusterEtVsEta_endcap=nullptr; // for non-cosmics
  m_clusterEtVsEta_hecfcal=nullptr; // for non-cosmics

  // energy > 500GeV cluster hists
  m_averageEtOver500_etaphi=nullptr; // for non-cosmics

  // cluster stat hists
  m_nClusters=nullptr;
  m_nClustersBottomVsTop=nullptr;
  m_averageEnergy_etaphi_maxEclusters=nullptr;
  m_dEtaVsdPhi_maxEclustersTopVsBottom=nullptr; // for cosmics 

  // tile hists 
  m_clustersCellsRatioEta=nullptr;
  m_clustersCellsRatioPhi=nullptr;
  m_clustersCellsRatioE=nullptr;
 
  m_clustersE=nullptr;
  m_clustersEta=nullptr;
  m_clustersPhi=nullptr;
  m_clustersEtaPhi=nullptr;

  m_clustersCellsEta=nullptr;
  m_clustersCellsPhi=nullptr;
  m_clustersCellsE=nullptr;
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloClusterVecMon::initialize() {
  StatusCode sc = StatusCode::SUCCESS;
  sc = retrieveTools();

  //Initialize read handle key
  ATH_CHECK( m_clusterContainerName.initialize() );

  sc = ManagedMonitorToolBase::initialize();
  if(sc.isFailure()){
    ATH_MSG_ERROR("Could not initialize ManagedMonitorToolBase");
    return sc;
  }

  ATH_CHECK( CaloMonToolBase::initialize() );

  return sc;
}

////////////////////////////////////////////////////////////////////////////

StatusCode CaloClusterVecMon::retrieveTools(){
  StatusCode sc = StatusCode::SUCCESS;

  sc = detStore()->retrieve(m_caloMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve CaloIdManager from DetectorStore" );
    return sc;
  }
  else{
   ATH_MSG_INFO("CaloMgr is retrieved");
  }

  m_caloCellHelper = m_caloMgr->getCaloCell_ID();
  if (!m_caloCellHelper) {
    ATH_MSG_ERROR( "Could not access CaloCell_ID helper" );
    return StatusCode::FAILURE;
  }

  return sc;
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloClusterVecMon::bookHistogramsRecurrent(){
//  Book old-style (unmanaged) histograms. Not used.

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloClusterVecMon::bookHistograms(){
// Book managed histograms. 

  ATH_MSG_DEBUG( "in bookHistograms()" );

  Interval_t theinterval = run;
//  No longer needed with managed histograms - LS 12/2013
//  bool testTimeGran = checkTimeGran( isNewEventsBlock, isNewLumiBlock, isNewRun, theinterval);

//  if (testTimeGran){
    bookCellHists(theinterval);
    bookClusterHists(theinterval);
    bookClusterStatHists(theinterval);
    if(  m_clusterContainerName.key() == "CaloCalTopoClusters" ) {
      fillTileHistRange();
      bookTileHists(theinterval);
    }

//  }
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////
bool CaloClusterVecMon::checkTimeGran(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, Interval_t& theinterval){  
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
  
////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::bookCellHists(const Interval_t theinterval){
    std::string TheTrigger;
    if (m_triggerChainProp == "")  TheTrigger="NoTrigSel";
    else TheTrigger = m_triggerChainProp;

    MonGroup  cluster_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/General", theinterval);                
    MonGroup  cluster_energytime_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/Time_Energy", theinterval);
    MonGroup  cluster_leadcell_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/LeadCell", theinterval);      

    if (  m_clusterContainerName.key() == "LArMuClusterCandidates" ) {
      m_nCells = new TH1I("NCells","NCellsinCluster",20,0.,20.);
    }else {
      m_nCells = new TH1I("NCells","NCellsinCluster",200,0.,200.);
    }
    cluster_expert.regHist( m_nCells).ignore();
    m_nCells->GetXaxis()->SetTitle("Number of Cells in Cluster");

    m_maxEcellToEclusterRatio = new TH1F("ratioE_leadcell_clus","Ratio(E_lead_cell/E_cluster)",500,-2.1,2.1);
    cluster_leadcell_expert.regHist(  m_maxEcellToEclusterRatio ).ignore();
    m_maxEcellToEclusterRatio->GetXaxis()->SetTitle("Ratio(E_lead_cell/E_cluster)");

    m_dominantCellOccupancy_etaphi = new TH2F("etaphi_hotrat","Occupancy of Clusters with E_cell/E_cluster > 0.9 &E_cluster>0 GeV",98,-4.9,4.9,64,-3.15,3.15);
    cluster_leadcell_expert.regHist(  m_dominantCellOccupancy_etaphi ).ignore();
    m_dominantCellOccupancy_etaphi->GetXaxis()->SetTitle("#eta");
    m_dominantCellOccupancy_etaphi->GetYaxis()->SetTitle("#phi");
    m_dominantCellOccupancy_etaphi->GetZaxis()->SetLabelSize(0.02);

    m_dominantCellAverageEnergy_etaphi  = new TProfile2D("etaphi_avgenergy_hotrat","Average energy of cells with E_cell/E_cluster > 0.9 & E_Cluster > 0 GeV",98,-4.9,4.9,64,-3.15,3.15);
    cluster_leadcell_expert.regHist(  m_dominantCellAverageEnergy_etaphi ).ignore();
    m_dominantCellAverageEnergy_etaphi->GetXaxis()->SetTitle("#eta");
    m_dominantCellAverageEnergy_etaphi->GetYaxis()->SetTitle("#phi");
    m_dominantCellAverageEnergy_etaphi->GetZaxis()->SetLabelSize(0.02);

    m_nCellInCluster_etaphi  = new TProfile2D("etaphi_ncellinclus","Average number of cells in Clusters",98,-4.9,4.9,64,-3.15,3.15);
    cluster_expert.regHist(  m_nCellInCluster_etaphi ).ignore();
    m_nCellInCluster_etaphi->GetXaxis()->SetTitle("#eta");
    m_nCellInCluster_etaphi->GetYaxis()->SetTitle("#phi");    

    if(m_dataType ==  AthenaMonManager::cosmics ){
      m_clusterTime = new TH1F("m_clus_time","Time of Cluster",32,-m_tWidth/2.0,m_tWidth/2.0);
      cluster_energytime_expert.regHist( m_clusterTime).ignore();
      m_clusterTime->GetXaxis()->SetTitle("Time (ns)");

      m_clusterTimeVsEnergy = new TH2F("m_clus_timevsenergy","Time of Cluster vs Energy",32,-m_tWidth/2.0,m_tWidth/2.0,500,-500,500);
      cluster_energytime_expert.regHist( m_clusterTimeVsEnergy).ignore();
      m_clusterTimeVsEnergy->GetXaxis()->SetTitle("Time (ns)");
      m_clusterTimeVsEnergy->GetYaxis()->SetTitle("Energy (MeV)");

      m_cellTime = new TH1F("m_cell_time","Time of leading cell in Cluster",32,-m_tWidth/2.0,m_tWidth/2.0);
      cluster_expert.regHist( m_cellTime).ignore();
      m_cellTime->GetXaxis()->SetTitle("Time (ns)");

      m_cellvsclust_time = new TH2F("m_cellvsclust_time","Time of leading cell in Cluster vs Cluster time",32,-m_tWidth/2.0,m_tWidth/2.0, 32,-m_tWidth/2.0,m_tWidth/2.0);
      cluster_expert.regHist( m_cellvsclust_time).ignore();
      m_cellvsclust_time->GetXaxis()->SetTitle("Cell time (ns)");
      m_cellvsclust_time->GetYaxis()->SetTitle("Cluster time (ns)");

    } else {
      m_clusterTime = new TH1F("m_clus_time","Time of Cluster",32,-15,15);
      cluster_energytime_expert.regHist( m_clusterTime).ignore();
      m_clusterTime->GetXaxis()->SetTitle("Time (ns)");

      m_clusterTimeVsEnergy = new TH2F("m_clus_timevsenergy","Time of Cluster vs Energy",32,-15,15,500,-100,500);
      cluster_energytime_expert.regHist( m_clusterTimeVsEnergy).ignore();
      m_clusterTimeVsEnergy->GetXaxis()->SetTitle("Time (ns)");
      m_clusterTimeVsEnergy->GetYaxis()->SetTitle("Energy (GeV)");

      m_cellTime = new TH1F("m_cell_time","Time of leading cell in Cluster",32,-15,15);
      cluster_expert.regHist( m_cellTime).ignore();
      m_cellTime->GetXaxis()->SetTitle("Time (ns)");

      m_cellvsclust_time = new TH2F("m_cellvsclust_time","Time of leading cell in Cluster vs Cluster time",32,-15,15,32,-15,15);
      cluster_expert.regHist( m_cellvsclust_time).ignore();
      m_cellvsclust_time->GetXaxis()->SetTitle("Cell time (ns)");
      m_cellvsclust_time->GetYaxis()->SetTitle("Cluster time (ns)");
    }
}

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::bookClusterHists(const Interval_t theinterval){
    std::string TheTrigger;
    if (m_triggerChainProp == "")  TheTrigger="NoTrigSel";
    else TheTrigger = m_triggerChainProp;

    MonGroup  cluster_1drates_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/1d_Rates", theinterval);
    MonGroup  cluster_2drates_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/2d_Rates", theinterval);
    MonGroup  cluster_2drates_shift ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/2d_Rates", theinterval);
    MonGroup  cluster_2davge_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/2d_AvEnergy", theinterval);     
    MonGroup  cluster_2davge_shift  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/2d_AvEnergy", theinterval);       
    MonGroup  cluster_1davge_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/1d_AvEnergy", theinterval);     
    MonGroup  cluster_2davgEt_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/TransEnergy", theinterval);    
    MonGroup  cluster_2dTotale_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/TotalEnergy", theinterval);   
    MonGroup  cluster_energytime_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/Time_Energy", theinterval);
    MonGroup  cluster_SummaryGroup_expert ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/Summary", theinterval); 
    
    bookBaseHists(&cluster_SummaryGroup_expert).ignore(); //from base class

    for (int i=0; i<MAX_E; i++) { //loop over thresholds
      char bname[256];
      char btitle[256];

      sprintf(btitle, "Occupancy of clusters with  Et_clus>%4.1f GeV", m_Ethresh[i]);
      sprintf(bname, "m_clus_etaphi_Et_thresh%d", i);
      m_clus_etaphi_Et_thresh[i] = new TH2F(bname,btitle,98,-4.9,4.9,64,-3.15,3.15);
      cluster_2drates_expert.regHist(m_clus_etaphi_Et_thresh[i]).ignore();
      m_clus_etaphi_Et_thresh[i]->GetXaxis()->SetTitle("#eta");
      m_clus_etaphi_Et_thresh[i]->GetYaxis()->SetTitle("#phi");

      sprintf(btitle, "Avg Et of clusters with Et_clus>%4.1f GeV", m_Ethresh[i]);
      sprintf(bname, "etaphi_thresh_avgEt_%d", i);
      m_etaphi_thresh_avgEt[i] = new TProfile2D(bname, btitle ,98,-4.9,4.9,64,-3.15,3.15);
      cluster_2davgEt_expert.regHist(  m_etaphi_thresh_avgEt[i] ).ignore();
      m_etaphi_thresh_avgEt[i]->GetXaxis()->SetTitle("#eta");
      m_etaphi_thresh_avgEt[i]->GetYaxis()->SetTitle("#phi");

// DEPRECATED -- LS 21 March 2016      
//      sprintf(btitle, "Occupancy of EMclusters with  Et_clus>%4.1f GeV", m_Ethresh[i]);
//      sprintf(bname, "m_EMclus_etaphi_Et_thresh%d", i);
//      m_EMclus_etaphi_Et_thresh[i] = new TH2F(bname,btitle,98,-4.9,4.9,64,-3.15,3.15);
//      cluster_2drates_expert.regHist(m_EMclus_etaphi_Et_thresh[i]).ignore();
//      m_EMclus_etaphi_Et_thresh[i]->GetXaxis()->SetTitle("#eta");
//      m_EMclus_etaphi_Et_thresh[i]->GetYaxis()->SetTitle("#phi");
//
//      sprintf(btitle, "Occupancy of EMclusters with  E_clus>%4.1f GeV", m_Ethresh[i]);
//      sprintf(bname, "m_EMclus_EtavsPhi%d", i);
//      m_EMclus_etaVsPhi[i] = new TH2F(bname,btitle,98,-4.9,4.9,64,-3.15,3.15);
//      if(i==0 || i==2) cluster_2drates_shift.regHist(m_EMclus_etaVsPhi[i]).ignore();
//      else cluster_2drates_expert.regHist(m_EMclus_etaVsPhi[i]).ignore();
//      m_EMclus_etaVsPhi[i]->GetXaxis()->SetTitle("#eta");
//      m_EMclus_etaVsPhi[i]->GetYaxis()->SetTitle("#phi");
      

      sprintf(btitle, "Occupancy of Clusters with E-Clus>%4.1f GeV", m_Ethresh[i]);
      sprintf(bname, "m_clus_eta%d", i);
      m_clus_eta[i] = new TH1F(bname,btitle,64,-5.0,5.0);
      cluster_1drates_expert.regHist( m_clus_eta[i] ).ignore();
      m_clus_eta[i]->GetXaxis()->SetTitle("#eta");
      
      // km add 
      sprintf(btitle, "Occupancy of Clusters with Et-Clus>%4.1f GeV", m_Ethresh[i]);
      sprintf(bname, "m_clus_eta_Et%d", i);
      m_clus_eta_Et[i] = new TH1F(bname,btitle,64,-5.0,5.0);
      cluster_1drates_expert.regHist( m_clus_eta_Et[i]).ignore();
      m_clus_eta_Et[i]->GetXaxis()->SetTitle("#eta");

      for (int j=0; j<3;j++){ //loop over regions
        switch(j){
        case 0:
          sprintf(btitle, "Occupancy of Barrel Clusters Vs Phi E_Clus>%4.1f GeV ", m_Ethresh[i]);
          sprintf(bname, "m_clus_phi%dBarrel", i);
          break;
        case 1:
          sprintf(btitle, "Occupancy of EndCapA Clusters Vs Phi with E_clus>%4.1f GeV ", m_Ethresh[i]);
          sprintf(bname, "m_clus_phi%dEndcapA", i);
          break;
        case 2:
          sprintf(btitle, "Occupancy of EndCapC clusters Vs phi with E_clus>%4.1f GeV Endcap C", m_Ethresh[i]);
          sprintf(bname, "m_clus_phi%dEndcapC", i);
          break;
        }
        m_clus_phi[i][j] =  new TH1F(bname,btitle,64,-3.15,3.15);
        cluster_1drates_expert.regHist( m_clus_phi[i][j] ).ignore();
        m_clus_phi[i][j]->GetXaxis()->SetTitle("#phi");
        m_clus_phi[i][j]->GetYaxis()->SetMoreLogLabels();
        m_clus_phi[i][j]->GetZaxis()->SetMoreLogLabels();
      }      

      // km add
    
       for (int j=0; j<3;j++){ //loop over regions
        switch(j){
        case 0:
          sprintf(btitle, "Occupancy of Barrel Clusters Vs Phi Et_Clus>%4.1f GeV ", m_Ethresh[i]);
          sprintf(bname, "m_clus_phi_Et%dBarrel", i);
          break;
        case 1:
          sprintf(btitle, "Occupancy of EndCapA Clusters Vs Phi with Et_clus>%4.1f GeV ", m_Ethresh[i]);
          sprintf(bname, "m_clus_phi_Et%dEndcapA", i);
          break;
        case 2:
          sprintf(btitle, "Occupancy of EndCapC clusters Vs phi with Et_clus>%4.1f GeV Endcap C", m_Ethresh[i]);
          sprintf(bname, "m_clus_phi_Et%dEndcapC", i);
          break;
        }
        m_clus_phi_Et[i][j] =  new TH1F(bname,btitle,64,-3.15,3.15);
        cluster_1drates_expert.regHist( m_clus_phi_Et[i][j] ).ignore();
        m_clus_phi_Et[i][j]->GetXaxis()->SetTitle("#phi");
        m_clus_phi_Et[i][j]->GetYaxis()->SetMoreLogLabels();
        m_clus_phi_Et[i][j]->GetZaxis()->SetMoreLogLabels();
      }      
      
 
      sprintf(btitle, "Occupancy of clusters with  E_clus>%4.1f GeV", m_Ethresh[i]);
      sprintf(bname, "m_EtavsPhi%d", i);
      m_etaVsPhi[i] = new TH2F(bname,btitle,98,-4.9,4.9,64,-3.15,3.15);
      if(i==0 || i==2) cluster_2drates_shift.regHist(m_etaVsPhi[i]).ignore();
      else cluster_2drates_expert.regHist(m_etaVsPhi[i]).ignore();
      m_etaVsPhi[i]->GetXaxis()->SetTitle("#eta");
      m_etaVsPhi[i]->GetYaxis()->SetTitle("#phi");
      m_etaVsPhi[i]->GetZaxis()->SetMoreLogLabels();  // km add

      sprintf(btitle, "Avg energy of clusters with E_clus>%4.1f GeV", m_Ethresh[i]);
      sprintf(bname, "etaphi_thresh_avgenergy_%d", i);
      m_etaphi_thresh_avgenergy[i] = new TProfile2D(bname, btitle ,98,-4.9,4.9,64,-3.15,3.15);
      if(i==0 || i==2) cluster_2davge_shift.regHist(  m_etaphi_thresh_avgenergy[i] ).ignore();
      else cluster_2davge_expert.regHist(  m_etaphi_thresh_avgenergy[i] ).ignore();
      m_etaphi_thresh_avgenergy[i]->GetXaxis()->SetTitle("#eta");
      m_etaphi_thresh_avgenergy[i]->GetYaxis()->SetTitle("#phi");
      m_etaphi_thresh_avgenergy[i]->GetZaxis()->SetMoreLogLabels();

      sprintf(btitle, "Total energy of clusters with E_clus>%4.1f GeV", m_Ethresh[i]);
      sprintf(bname, "etaphi_thresh_Totalenergy_%d", i);
      m_etaphi_thresh_Totalenergy[i] = new TH2F(bname, btitle ,98,-4.9,4.9,64,-3.15,3.15);
      cluster_2dTotale_expert.regHist(  m_etaphi_thresh_Totalenergy[i] ).ignore();
      m_etaphi_thresh_Totalenergy[i]->GetXaxis()->SetTitle("#eta");
    }

    m_etaVsPhiNegEn = new TH2F("m_EtavsPhiNegEn","Occupancy of cluster with  E_Clus < 0.0 GeV",98,-4.9,4.9,64,-3.15,3.15);
    cluster_2drates_expert.regHist(m_etaVsPhiNegEn).ignore();
    m_etaVsPhiNegEn ->GetXaxis()->SetTitle("#eta");

    m_averageNegativeEnergy_etaphi  = new TProfile2D("etaphi_avgnegenergy","Average energy of negative energy clusters",98,-4.9,4.9,64,-3.15,3.15);
    cluster_2davge_expert.regHist(  m_averageNegativeEnergy_etaphi ).ignore();
    m_averageNegativeEnergy_etaphi->GetXaxis()->SetTitle("#eta");
    m_averageNegativeEnergy_etaphi->GetYaxis()->SetTitle("#phi");    

    m_averageEnergy_phi  = new TProfile("phi_avgenergy","Avg energy of clusters Vs #phi",64,-3.15,3.15);
    cluster_1davge_expert.regHist(  m_averageEnergy_phi ).ignore();
    m_averageEnergy_phi->GetXaxis()->SetTitle("#phi");
    m_averageEnergy_phi->GetYaxis()->SetTitle("Energy (GeV)");

    m_averageEnergy_eta  = new TProfile("eta_avgenergy","Avg energy of cluster Vs #eta",98,-4.9,4.9);
    cluster_1davge_expert.regHist(  m_averageEnergy_eta ).ignore();
    m_averageEnergy_eta->GetXaxis()->SetTitle("#eta");
    m_averageEnergy_eta->GetYaxis()->SetTitle("Energy (GeV)");    

    if (m_dataType ==  AthenaMonManager::cosmics ){
      m_clusterEnergyVsEta_barrel = new TH1F("energy_clus_barrel","Energy of Clusters for 0<|eta|<1.4",500,-500,800);
      cluster_energytime_expert.regHist(  m_clusterEnergyVsEta_barrel ).ignore();
      m_clusterEnergyVsEta_barrel->GetXaxis()->SetTitle("Energy (MeV)");

      m_clusterEnergyVsEta_endcap = new TH1F("energy_clus_endcap","Energy of Clusters for 1.4<|eta|<2.0",500,-500,2000);
      cluster_energytime_expert.regHist(  m_clusterEnergyVsEta_endcap ).ignore();
      m_clusterEnergyVsEta_endcap->GetXaxis()->SetTitle("Energy (MeV)");

      m_clusterEnergyVsEta_hecfcal = new TH1F("energy_clus_hecfcal","Energy of Clusters for |eta|>2.0",500,-2000,2000);
      cluster_energytime_expert.regHist(  m_clusterEnergyVsEta_hecfcal ).ignore();
      m_clusterEnergyVsEta_hecfcal->GetXaxis()->SetTitle("Energy (MeV)");
    } else {
      m_clusterEnergyVsEta_barrel = new TH1F("energy_clus_barrel","Energy of Clusters for 0<eta<1.4",500,-100,500);
      cluster_energytime_expert.regHist(  m_clusterEnergyVsEta_barrel ).ignore();
      m_clusterEnergyVsEta_barrel->GetXaxis()->SetTitle("Energy (GeV)");

      m_clusterEnergyVsEta_endcap = new TH1F("energy_clus_endcap","Energy of Clusters for 1.4<eta<2.0",500,-100,500);
      cluster_energytime_expert.regHist(  m_clusterEnergyVsEta_endcap ).ignore();
      m_clusterEnergyVsEta_endcap->GetXaxis()->SetTitle("Energy (GeV)");

      m_clusterEnergyVsEta_hecfcal = new TH1F("energy_clus_hecfcal","Energy of Clusters for |eta|>2.0",500,-100,500);
      cluster_energytime_expert.regHist(  m_clusterEnergyVsEta_hecfcal ).ignore();
      m_clusterEnergyVsEta_hecfcal->GetXaxis()->SetTitle("Energy (GeV)");

      m_clusterEtVsEta_barrel = new TH1F("et_clus_barrel","Et of Clusters for 0<|eta|<1.4",500,-100,500);
      cluster_energytime_expert.regHist(  m_clusterEtVsEta_barrel ).ignore();
      m_clusterEtVsEta_barrel->GetXaxis()->SetTitle("Et (GeV)");

      m_clusterEtVsEta_endcap = new TH1F("et_clus_endcap","Et of Clusters for 1.4<|eta|<2.0",500,-100,500);
      cluster_energytime_expert.regHist(  m_clusterEtVsEta_endcap ).ignore();
      m_clusterEtVsEta_endcap->GetXaxis()->SetTitle("Et (GeV)");

      m_clusterEtVsEta_hecfcal = new TH1F("et_clus_hecfcal","Et of Clusters for |eta|>2.0",500,-100,500);
      cluster_energytime_expert.regHist(  m_clusterEtVsEta_hecfcal ).ignore();
      m_clusterEtVsEta_hecfcal->GetXaxis()->SetTitle("Energy (GeV)");

      m_averageEtOver500_etaphi  = new TProfile2D("Max500_etaphi_avget","Avg Et of cluster with Et > 500 GeV" ,98,-4.9,4.9,64,-3.15,3.15);
      cluster_2davge_expert.regHist(  m_averageEtOver500_etaphi ).ignore();
      m_averageEtOver500_etaphi->GetXaxis()->SetTitle("#eta");
      m_averageEtOver500_etaphi->GetYaxis()->SetTitle("#phi");
    }
} 

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::bookClusterStatHists(const Interval_t theinterval){
    std::string TheTrigger;
    if (m_triggerChainProp == "")  TheTrigger="NoTrigSel";
    else TheTrigger = m_triggerChainProp;
 
    MonGroup  cluster_2davgEt_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/TransEnergy", theinterval);   
    MonGroup  cluster_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/General", theinterval);                
    MonGroup  cluster_2davge_expert  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/2d_AvEnergy", theinterval);     

    if (m_dataType ==  AthenaMonManager::cosmics ){
      if (  m_clusterContainerName.key() == "LArMuClusterCandidates" ) {
        m_nClusters = new TH1I("NClus","NCluster",200,0,200);
      }else  {
        m_nClusters = new TH1I("NClus","NCluster",50,0,50);
      }
    }else {
      if ( m_clusterContainerName.key() == "EMTopoCluster" ){
        m_nClusters = new TH1I("NClus","NCluster",300,0,300);
      }else  {
        m_nClusters = new TH1I("NClus","NCluster",600,0,600);
      }
    }
    cluster_expert.regHist(  m_nClusters ).ignore();
    m_nClusters->GetXaxis()->SetTitle("Number of Clusters");

    m_nClustersBottomVsTop = new TH2I("NClus_botvstop","Number of Cluster in bottomvstop", 80, 0, 80, 80, 0,80);
    cluster_expert.regHist(  m_nClustersBottomVsTop ).ignore();
    m_nClustersBottomVsTop ->GetXaxis()->SetTitle("Number of Clusters in bottom");
    m_nClustersBottomVsTop ->GetYaxis()->SetTitle("Number of Clusters in top");

    if (m_dataType ==  AthenaMonManager::cosmics ){
      m_averageEnergy_etaphi_maxEclusters  = new TProfile2D("MaxEnergy_etaphi_avgenergy","max avg energy cluster from top and bottom",98,-4.9,4.9,64,-3.15,3.15);
      cluster_2davge_expert.regHist(  m_averageEnergy_etaphi_maxEclusters ).ignore();
      m_averageEnergy_etaphi_maxEclusters->GetXaxis()->SetTitle("#eta");
      m_averageEnergy_etaphi_maxEclusters->GetYaxis()->SetTitle("#phi");

      m_dEtaVsdPhi_maxEclustersTopVsBottom  = new TH2F("Deltaphi_Deltaeta_top_bot_Maxenergy","#Delta Eta and #Delta phi of max avg energy cluster from top and bottom",98,-4.9,4.9,64,0,6.3);
      cluster_expert.regHist(  m_dEtaVsdPhi_maxEclustersTopVsBottom ).ignore();
      m_dEtaVsdPhi_maxEclustersTopVsBottom->GetXaxis()->SetTitle("#eta");
      m_dEtaVsdPhi_maxEclustersTopVsBottom->GetYaxis()->SetTitle("#phi");
    }
    else{
      m_averageEnergy_etaphi_maxEclusters  = new TProfile2D("MaxEnergy_etaphi_avgenergy","Avg energy of most energetic cluster",98,-4.9,4.9,64,-3.15,3.15);
      cluster_2davge_expert.regHist(  m_averageEnergy_etaphi_maxEclusters ).ignore();
      m_averageEnergy_etaphi_maxEclusters->GetXaxis()->SetTitle("#eta");
      m_averageEnergy_etaphi_maxEclusters->GetYaxis()->SetTitle("#phi");
    }

}

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::fillTileHistRange(){
    m_binRangeE.reserve(3);
    m_binRangeE[0]=200;
    m_binRangeE[1]=0;
    m_binRangeE[2]=150; // changed the energy from 100 to 150 

    m_binRangeEta.reserve(3);
    m_binRangeEta[0]=16;
    m_binRangeEta[1]=-1.6;
    m_binRangeEta[2]=1.6;

    m_binRangePhi.reserve(3);
    m_binRangePhi[0]=64;
    m_binRangePhi[1]=-M_PI;
    m_binRangePhi[2]=M_PI;

    m_binRangeEtaPhi.reserve(6);
    m_binRangeEtaPhi[0]=16;
    m_binRangeEtaPhi[1]=-1.6;
    m_binRangeEtaPhi[2]=1.6;
    m_binRangeEtaPhi[3]=64;
    m_binRangeEtaPhi[4]=-M_PI;
    m_binRangeEtaPhi[5]=M_PI;
}

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::bookTileHists(const Interval_t theinterval){
      std::string TheTrigger;
      if (m_triggerChainProp == "")  TheTrigger="NoTrigSel";
      else TheTrigger = m_triggerChainProp;

      MonGroup  tile_cluster_shift  ( this, "/CaloMonitoring/ClusterMon/"+m_clusterContainerName.key()+TheTrigger+"/General", theinterval);    //SHIFT

      m_clustersCellsRatioEta    = new TProfile("TileCalXRatioXEta","Ratio: cluster E/ cluster cells",(int)m_binRangeEta[0],m_binRangeEta[1], m_binRangeEta[2]);
      m_clustersCellsRatioEta->GetXaxis()->SetTitle("Eta");
      m_clustersCellsRatioEta->GetYaxis()->SetTitle("Cluster Energy / Sum over cluster cells");
      tile_cluster_shift.regHist(  m_clustersCellsRatioEta  ).ignore(); 

      m_clustersCellsRatioPhi    = new TProfile("ClustersXRatioXPhi","Ratio: cluster E/ cluster cells",(int)m_binRangePhi[0],m_binRangePhi[1], m_binRangePhi[2]);
      m_clustersCellsRatioPhi->GetXaxis()->SetTitle("Phi [rad]");
      m_clustersCellsRatioPhi->GetYaxis()->SetTitle("Cluster Energy / Sum over cluster cells");
      tile_cluster_shift.regHist(  m_clustersCellsRatioPhi  ).ignore();

      m_clustersCellsRatioE    = new TProfile("TileCalXRatioXE","Ratio: cluster E/ cluster cells",(int)m_binRangeE[0],m_binRangeE[1], m_binRangeE[2]);
      m_clustersCellsRatioE->GetXaxis()->SetTitle("Energy [GeV]");
      m_clustersCellsRatioE->GetYaxis()->SetTitle("Cluster Energy / Sum over cluster cells");
      tile_cluster_shift.regHist(  m_clustersCellsRatioE  ).ignore();

      m_clustersE    = new TH1F("TileCalXclustersXE","Energy of TileCal clusters",(int)m_binRangeE[0],m_binRangeE[1], m_binRangeE[2]);
      m_clustersE->GetXaxis()->SetTitle("Energy [GeV]");
      m_clustersE->GetYaxis()->SetTitle("Events");
      tile_cluster_shift.regHist(  m_clustersE  ).ignore();

      m_clustersEta    = new TProfile("TileCalXclustersXEta","Eta of TileCal clusters",(int)m_binRangeEta[0],m_binRangeEta[1], m_binRangeEta[2]);
      m_clustersEta->GetXaxis()->SetTitle("Eta");
      m_clustersEta->GetYaxis()->SetTitle("Mean energy [GeV]");
      tile_cluster_shift.regHist(  m_clustersEta  ).ignore();

      m_clustersPhi    = new TProfile("TileCalXclustersXPhi","Phi Tile clusters",(int)m_binRangePhi[0],m_binRangePhi[1], m_binRangePhi[2]);
      m_clustersPhi->GetXaxis()->SetTitle("Phi [rad]");
      m_clustersPhi->GetYaxis()->SetTitle("Mean energy [GeV]");
      tile_cluster_shift.regHist(  m_clustersPhi  ).ignore();

      m_clustersEtaPhi    = new TH2F("TileCalXclustersXEtaVSPhi","Tile clusters",(int)m_binRangeEtaPhi[0],m_binRangeEtaPhi[1], m_binRangeEtaPhi[2], (int)m_binRangeEtaPhi[3],m_binRangeEtaPhi[4], m_binRangeEtaPhi[5]);
      m_clustersEtaPhi->GetXaxis()->SetTitle("Eta");
      m_clustersEtaPhi->GetYaxis()->SetTitle("Phi [rad]");
      tile_cluster_shift.regHist(  m_clustersEtaPhi  ).ignore();

      m_clustersCellsE    = new TProfile("TileCalXRXE","Tile clusters",(int)m_binRangeE[0],m_binRangeE[1], m_binRangeE[2]);
      m_clustersCellsE->GetXaxis()->SetTitle("Energy [GeV]");
      m_clustersCellsE->GetYaxis()->SetTitle("Energy fraction from Tile Cal");
      tile_cluster_shift.regHist(  m_clustersCellsE  ).ignore();

      m_clustersCellsEta    = new TProfile("TileCalXRXEta","Tile clusters",(int)m_binRangeEta[0],m_binRangeEta[1], m_binRangeEta[2]);
      m_clustersCellsEta->GetXaxis()->SetTitle("Eta");
      m_clustersCellsEta->GetYaxis()->SetTitle("Energy fraction from TileCal");
      tile_cluster_shift.regHist(  m_clustersCellsEta  ).ignore();

      m_clustersCellsPhi    = new TProfile("ClustersXRXPhi","Tile clusters",(int)m_binRangePhi[0],m_binRangePhi[1], m_binRangePhi[2]);
      m_clustersCellsPhi->GetXaxis()->SetTitle("Phi [rad]");
      m_clustersCellsPhi->GetYaxis()->SetTitle("Energy fraction from TileCal");
      tile_cluster_shift.regHist(  m_clustersCellsPhi  ).ignore();

}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloClusterVecMon::fillHistograms() {
  
  StatusCode sc = StatusCode::SUCCESS;

  bool ifPass = 1;
  sc = checkFilters(ifPass);
  if(sc.isFailure() || !ifPass) return StatusCode::SUCCESS;

  SG::ReadHandle<xAOD::CaloClusterContainer> clusterContHandle{m_clusterContainerName};
  if (! clusterContHandle.isValid()) { ATH_MSG_WARNING("No CaloCluster container found in TDS"); return StatusCode::FAILURE; }
  const xAOD::CaloClusterContainer* clusterCont = clusterContHandle.get();

  initCounter(); 

  xAOD::CaloClusterContainer::const_iterator it = clusterCont->begin(); 
  xAOD::CaloClusterContainer::const_iterator it_e = clusterCont->end(); 

  //count events
  m_eventsCounter++; // km add

  for ( ; it!=it_e;++it) { 
    const CaloCluster* clus = *it; 

    fillCellHist(clus);
    //fillClusterEMvar(clus);

    fillClusterStat(clus); 
    fillClusterHist(clus); 
  } // cluster iter cut

  fillClusterStatHist(clusterCont);

  if(  m_clusterContainerName.key() == "CaloCalTopoClusters" ) fillTileHist(clusterCont);

  return sc;
}


////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::initCounter(){

  m_cluscount=0;
  m_cluscount_top=0;
  m_cluscount_bot=0;
  m_maxclusindex_top=0;
  m_maxclusindex_bot=0;
  m_maxclusene_top=0;
  m_maxclusene_bot=0;
}

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::fillCellHist(const CaloCluster* clus){
  int cellcount=0;
  //int maxcellindex=0;
  float maxcellene=0;
  float maxcelltime=0;

  const CaloClusterCellLink* cellLinks=clus->getCellLinks();
  if (!cellLinks) {
    ATH_MSG_DEBUG( "No cell links for this cluster"  );
    return;
  }

  const CaloCellContainer* cellCont=cellLinks->getCellContainer();
  if (!cellCont) {
    ATH_MSG_DEBUG( "DataLink to cell container is broken"  );
    return;
  }


  xAOD::CaloCluster::const_cell_iterator cellIter =clus->cell_begin();
  xAOD::CaloCluster::const_cell_iterator cellIterEnd =clus->cell_end();
  for ( ;cellIter !=cellIterEnd;cellIter++) {
      ++cellcount;
      const CaloCell* cell= (*cellIter);
      float EnergyCell=cell->energy();
      float TimeCell=cell->time();
      if ( cellcount == 1 || fabs(EnergyCell) > fabs(maxcellene) ) {
        maxcellene=EnergyCell;
        //maxcellindex=cellcount;
        maxcelltime=TimeCell;  
      }
  }
  m_nCells->Fill(cellcount);

  float EnergyClus = clus->e();
  float EtaClus = clus->eta(); 
  float PhiClus = clus->phi(); 
  float TimeClus = clus->time();
  float ratio=0; 
  if(EnergyClus > 0.0) ratio = maxcellene/EnergyClus;

  if ( fabs(EtaClus) < 5.0) {

   if(EnergyClus > 0.0) {
    m_maxEcellToEclusterRatio->Fill(ratio );
    if( ratio > 0.9 ) {
      m_dominantCellOccupancy_etaphi->Fill(EtaClus,PhiClus);
      m_dominantCellAverageEnergy_etaphi->Fill(EtaClus,PhiClus,EnergyClus/GeV);
    }
   }

   m_nCellInCluster_etaphi->Fill(EtaClus,PhiClus,cellcount);
   m_clusterTimeVsEnergy->Fill(TimeClus,EnergyClus/GeV);
   m_clusterTime->Fill(TimeClus);
   m_cellTime->Fill(maxcelltime);
   m_cellvsclust_time->Fill(maxcelltime,TimeClus);
  }

}

////////////////////////////////////////////////////////////////////////////
// THIS ROUTINE IS DEPRECATED - LS 21 March 2016
//void CaloClusterVecMon::fillClusterEMvar(const xAOD::CaloCluster* clus){
// 
//  m_EMenergy=0.;
//  m_EMenergy_abs=0.;
//  m_EMet=0.;
//  m_EMeta=0.;
//  m_EMphi=0.;
//
//  std::cout << m_clusterContainerName << std::endl;
//  if(  m_clusterContainerName == "CaloCalTopoClusters" ) {
//
//    //Loop over samplings
//    for (unsigned iS=0;iS<CaloSampling::HEC0;++iS) {
//      const CaloSampling::CaloSample iSamp=(CaloSampling::CaloSample)iS;
//      if (clus->hasSampling(iSamp)) {
//	const float sampler_energy = clus->eSample(iSamp);
//	const float sampler_eta = clus->etaSample(iSamp);//samplers_eta.at(sampler_id);
//	const float sampler_phi = clus->phiSample(iSamp);//samplers_phi.at(sampler_id);
//	m_EMenergy+=sampler_energy;
//	m_EMenergy_abs+=fabs(sampler_energy);
//	m_EMeta+=sampler_eta*fabs(sampler_energy);
//	m_EMphi+=sampler_phi*fabs(sampler_energy);
//      }
//    }
//
//    if(m_EMenergy_abs>0.01){
//      m_EMeta=m_EMeta/m_EMenergy_abs;
//      m_EMphi=m_EMphi/m_EMenergy_abs;
//      if(fabs(m_EMeta)<20.)m_EMet=m_EMenergy/cosh(m_EMeta);
//    }
//    else{
//      m_EMenergy=0.;
//      m_EMenergy_abs=0.;
//      m_EMet=0.;
//      m_EMeta=0.;
//      m_EMphi=0.;
//    }
//  }
//  else {
//   
//   // Use energy and eta/phi from the EM clusters 
//    m_EMeta = clus->eta();
//    m_EMphi = clus->phi();
//    m_EMet  = clus->et();
//    m_EMenergy = clus->e();
//    m_EMenergy_abs = fabs(m_EMenergy);
//  }
//
//}

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::fillClusterStat(const CaloCluster* clus){
   ++m_cluscount;
   float EnergyClus=clus->e();
   float PhiClus=clus->phi();

   if(PhiClus > 0.) {
      ++m_cluscount_top;
      if(PhiClus < 5.0){
        if(m_cluscount_top == 1 || m_maxclusene_top<EnergyClus){
          m_maxclusene_top=EnergyClus;
          m_maxclusindex_top=m_cluscount;
        }
      }
   } 
   else {
      ++m_cluscount_bot;
      if(PhiClus > (-5.0) ){
        if(m_cluscount_top == 1 || m_maxclusene_bot<EnergyClus){
          m_maxclusene_bot=EnergyClus;
          m_maxclusindex_bot=m_cluscount;
        }
      }
   }

}

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::fillClusterHist(const CaloCluster* clus){
    float EtaClus = clus->eta();
    float PhiClus = clus->phi();
    float EtClus = clus->et();
    float EClus = clus->e();

    TH1F* tmp_clusterEtVsEta_sub=nullptr;
    TH1F* tmp_clusterEnergyVsEta_sub=nullptr;
    if( (fabs(EtaClus)<m_etaMin[1]) && (fabs(EtaClus)>=m_etaMin[0]) ){
      tmp_clusterEtVsEta_sub = m_clusterEtVsEta_barrel;
      tmp_clusterEnergyVsEta_sub = m_clusterEnergyVsEta_barrel;
    }
    else if( (fabs(EtaClus)<m_etaMin[2]) && (fabs(EtaClus)>=m_etaMin[1]) ){
      tmp_clusterEtVsEta_sub = m_clusterEtVsEta_endcap;
      tmp_clusterEnergyVsEta_sub = m_clusterEnergyVsEta_endcap;
    }
    else if( fabs(EtaClus)>=m_etaMin[2] ){
      tmp_clusterEtVsEta_sub = m_clusterEtVsEta_hecfcal;
      tmp_clusterEnergyVsEta_sub = m_clusterEnergyVsEta_hecfcal;
    }
    
    if(fabs(EtaClus) < 5.0) {

      for (int j=0;j<MAX_E;j++) {
        if(EtClus > m_Ethresh[j]*GeV) {
          m_clus_etaphi_Et_thresh[j]->Fill(EtaClus, PhiClus);
          m_etaphi_thresh_avgEt[j]->Fill(EtaClus, PhiClus,EtClus/GeV);
        }

//        if(m_EMet>m_Ethresh[j]*GeV)  m_EMclus_etaphi_Et_thresh[j]->Fill(m_EMeta,m_EMphi);
//        if(m_EMenergy>m_Ethresh[j]*GeV)  m_EMclus_etaVsPhi[j]->Fill(m_EMeta,m_EMphi);

         // km add
          if (EtClus > m_Ethresh[j]*GeV ) {
           m_clus_eta_Et[j]->Fill(EtaClus);
           if (fabs(EtaClus)<1.5) {
            m_clus_phi_Et[j][0]->Fill(PhiClus);
          }else if (EtaClus > 1.5) {
            m_clus_phi_Et[j][1]->Fill(PhiClus);
          }else {
            m_clus_phi_Et[j][2]->Fill(PhiClus);
          }
        }    

        if(EClus/GeV > m_Ethresh[j]) {
          m_clus_eta[j]->Fill(EtaClus);
          if (fabs(EtaClus)<1.5) {
            m_clus_phi[j][0]->Fill(PhiClus);
          }else if (EtaClus > 1.5) {
            m_clus_phi[j][1]->Fill(PhiClus);
          }else {
            m_clus_phi[j][2]->Fill(PhiClus);
          }
          m_etaVsPhi[j]->Fill(EtaClus, PhiClus);
          m_etaphi_thresh_avgenergy[j]->Fill(EtaClus, PhiClus,EClus/GeV);
          m_etaphi_thresh_Totalenergy[j]->Fill(EtaClus, PhiClus,EClus/GeV);
        }
      }

      if( EClus < 0.0 ){
        m_etaVsPhiNegEn->Fill(EtaClus, PhiClus);
        m_averageNegativeEnergy_etaphi->Fill(EtaClus, PhiClus,EClus/GeV);
      }

      m_averageEnergy_eta->Fill(EtaClus,EClus/GeV);
      m_averageEnergy_phi->Fill(PhiClus,EClus/GeV);

      if(m_dataType !=  AthenaMonManager::cosmics){
        if(EtClus>500000.) m_averageEtOver500_etaphi->Fill(EtaClus, PhiClus,EtClus/GeV);
        if(tmp_clusterEtVsEta_sub) tmp_clusterEtVsEta_sub->Fill(EtClus/GeV); 
        if(tmp_clusterEnergyVsEta_sub) tmp_clusterEnergyVsEta_sub->Fill(EClus/GeV); 
      }
      else{
        if(tmp_clusterEnergyVsEta_sub) tmp_clusterEnergyVsEta_sub->Fill(EClus/GeV);  
      }

    } 
}

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::fillClusterStatHist(const xAOD::CaloClusterContainer* clusterCont){
  m_nClusters->Fill(m_cluscount);
  m_nClustersBottomVsTop->Fill(m_cluscount_bot,m_cluscount_top);

  float phi_top=0,eta_top=0;
  float phi_bot=0,eta_bot=0;
  if (m_maxclusindex_top > 0){
    phi_top=(*clusterCont)[m_maxclusindex_top-1]->phi();
    eta_top=(*clusterCont)[m_maxclusindex_top-1]->eta();
  }
  if (m_maxclusindex_bot > 0){
    phi_bot=(*clusterCont)[m_maxclusindex_bot-1]->phi();
    eta_bot=(*clusterCont)[m_maxclusindex_bot-1]->eta();
  }

  if (m_dataType ==  AthenaMonManager::cosmics ){

    if (m_maxclusindex_top > 0){
      m_averageEnergy_etaphi_maxEclusters->Fill(eta_top,phi_top,m_maxclusene_top/GeV);
      //m_averageEnergy_etaphi_maxEclusters->Fill(eta_top,phi_top,1.);
    }

    if (m_maxclusindex_bot > 0){
      m_averageEnergy_etaphi_maxEclusters->Fill(eta_bot,phi_bot,m_maxclusene_bot/GeV);
      //m_averageEnergy_etaphi_maxEclusters->Fill(eta_bot,phi_bot,1.);
    }

    if ((m_maxclusindex_top > 0) && (m_maxclusindex_bot > 0)) {
      m_dEtaVsdPhi_maxEclustersTopVsBottom->Fill((eta_top-eta_bot),(phi_top-phi_bot));
    }

  } else  {
    if ( m_maxclusene_top > m_maxclusene_bot ) {
      m_averageEnergy_etaphi_maxEclusters->Fill(eta_top,phi_top,m_maxclusene_top/GeV);
      //m_averageEnergy_etaphi_maxEclusters->Fill(eta_top,phi_top,1.);
    } else {
      m_averageEnergy_etaphi_maxEclusters->Fill(eta_bot,phi_bot,m_maxclusene_bot/GeV);
      //m_averageEnergy_etaphi_maxEclusters->Fill(eta_bot,phi_bot,1.);
    }
  }

}

////////////////////////////////////////////////////////////////////////////
void CaloClusterVecMon::fillTileHist(const xAOD::CaloClusterContainer* clusterCont){

  //xAOD::CaloClusterContainer clusColl(SG::VIEW_ELEMENTS);
  std::vector<const CaloCluster*> clusColl;
  for (auto clu : *clusterCont) {
    if ( fabs( clu->eta() ) < 1.6 ) clusColl.push_back(clu);
  }
  /*
  for(xAOD::CaloClusterContainer::const_iterator clusIter = clusterCont->begin(); clusIter != clusterCont->end(); ++clusIter) {
    //CaloCluster* cluster_ptr = const_cast<CaloCluster*>(*clusIter); 
    const CaloCluster* cluster_ptr = *clusIter; 
    if ( fabs( cluster_ptr->eta() ) < 1.6 ) clusColl.push_back(cluster_ptr);
  }
  */
  //std::sort(clusColl.begin(),clusColl.end(),GetSort::ClusSort());
  std::sort(clusColl.begin(),clusColl.end(),[](const xAOD::CaloCluster* o1, const xAOD::CaloCluster* o2) {return o1->e() > o2->e();}); //C++11 

  int i_clus=0;

  


  //for(xAOD::CaloClusterContainer::const_iterator iCluster = clusColl.begin(); iCluster != clusColl.end(); iCluster++) {
  for (const CaloCluster* cluster_ptr : clusColl) {
     float energy = cluster_ptr->e()/GeV;
     float eta = cluster_ptr->eta();
     float phi = cluster_ptr->phi();
     float eSum=0.;
     float eSumTile=0.;
     float ratio=0.0;
     float ratioTile=0.0;

     if (energy <  m_Threshold/GeV) continue;

     const CaloClusterCellLink* cellLinks=cluster_ptr->getCellLinks();
     if (!cellLinks) {
       ATH_MSG_DEBUG( "No cell links for this cluster"  );
       return;
     }

     const CaloCellContainer* cellCont=cellLinks->getCellContainer();
     if (!cellCont) {
       ATH_MSG_DEBUG( "DataLink to cell container is broken"  );
       return;
     }

     for (CaloCluster::const_cell_iterator itrCell = cluster_ptr->cell_begin();itrCell!=cluster_ptr->cell_end(); ++itrCell) {
       const CaloCell * theCell=*itrCell;
       float cell_e = theCell->energy()/GeV;
       eSum+=cell_e;

       const Identifier cellId = theCell->ID();
       if (m_caloCellHelper->is_tile(cellId))  eSumTile+=cell_e; 
     }

     if (eSum>0.) {
       ratio= energy / eSum;
       ratioTile=eSumTile / eSum;
     }

     m_clustersCellsRatioE->Fill(  energy, ratio, 1.0  );
     m_clustersCellsRatioEta->Fill(  eta, ratio, 1.0  );
     m_clustersCellsRatioPhi->Fill(  phi, ratio, 1.0  );

     m_clustersCellsE->Fill(  energy, ratioTile, 1.0  );
     m_clustersCellsEta->Fill(  eta, ratioTile, 1.0  );
     m_clustersCellsPhi->Fill(  phi, ratioTile, 1.0  );

     if( ratioTile > 0.5 ) {
       m_clustersE->Fill( energy );
       m_clustersEta->Fill( eta, energy );
       m_clustersPhi->Fill( phi, energy );
       m_clustersEtaPhi->Fill(  eta, phi  );
     }
     i_clus++;
     if (i_clus > 4) break; // take 4  most energetic clusters
  }
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloClusterVecMon::procHistograms( ) {
  StatusCode sc = StatusCode::SUCCESS;
  return sc;
}

