/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     CaloBaselineMon.cxx
//
// AUTHORS:   Benjamin Trocme (LPSC Grenoble)    01/2018
//           
//********************************************************************
              

#include "CaloMonitoring/CaloBaselineMon.h"

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

#include "RecBackgroundEvent/BeamBackgroundData.h"

#include "xAODEventInfo/EventInfo.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaKernel/Units.h"
#include <math.h>
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TH1I_LW.h"


using xAOD::CaloCluster;
using Athena::Units::GeV;

CaloBaselineMon::CaloBaselineMon(const std::string& type, const std::string& name, const IInterface* parent) :
  CaloMonToolBase(type, name, parent),
  //  m_caloMgr(nullptr),
  m_calo_id(nullptr),
  m_bunchCrossingTool("BunchCrossingTool")
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("CaloCellContainer", m_cellContainerName="AllCalo"); 
  declareProperty("BunchCrossingTool",m_bunchCrossingTool,"Tool handle for bunch crossing tool");
  declareProperty("partionList", m_partNames = {"EM","HEC+FCal"});
  declareProperty("pedestalMon_BCIDmin", m_pedestalMon_BCIDmin = 25);
  declareProperty("bcidtoolMon_BCIDmax", m_bcidtoolMon_BCIDmax = 144); 
  declareProperty("minimumEta", m_etaMin = {0.,1.2});
  declareProperty("maximumEta", m_etaMax = {3.2,5.});
  declareProperty("nbOfEtaBins", m_nbOfEtaBins = {16,19});
}


CaloBaselineMon::~CaloBaselineMon() {
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloBaselineMon::initialize() {

  //  ATH_MSG_WARNING("Entering CaloBaselineMon::initialize");
  StatusCode sc = StatusCode::SUCCESS;
  sc = retrieveTools();

  sc = ManagedMonitorToolBase::initialize();
  if(sc.isFailure()){
    ATH_MSG_ERROR("Could not initialize ManagedMonitorToolBase");
    return sc;
  }

  if (m_pedestalMon_BCIDmin > 0) m_bool_pedestalMon = true;
  else m_bool_pedestalMon = false;

  if (m_bcidtoolMon_BCIDmax > 0) m_bool_bcidtoolMon = true;
  else m_bool_bcidtoolMon = false;

  m_sum_partition_eta.resize(m_partNames.size());
  for (uint iPart = 0;iPart <  m_partNames.size();iPart++){
    m_sum_partition_eta[iPart].resize(m_nbOfEtaBins[iPart]);
  }

  m_etaBinWidth.resize(m_partNames.size());
  m_inv_etaBinWidth.resize(m_partNames.size());

  m_partMap.resize(3);
  m_partMap = {-1,-1,-1};
  // 0:EM / 1:HEC / FCal:2
  for (uint iPart = 0;iPart <  m_partNames.size();iPart++){
    if (m_partNames[iPart] == "EM") m_partMap[0] = iPart;
    if (m_partNames[iPart] == "HEC") m_partMap[1] = iPart;
    if (m_partNames[iPart] == "FCal") m_partMap[2] = iPart;
    if (m_partNames[iPart] == "HEC+FCal"){
	m_partMap[1] = iPart;
	m_partMap[2] = iPart;
      }  
    m_etaBinWidth[iPart] = (m_etaMax[iPart]-m_etaMin[iPart])/((float) m_nbOfEtaBins[iPart]);
    m_inv_etaBinWidth[iPart] = ((float) m_nbOfEtaBins[iPart])/(m_etaMax[iPart]-m_etaMin[iPart]);
  }

  return sc;
}

////////////////////////////////////////////////////////////////////////////

StatusCode CaloBaselineMon::retrieveTools(){

  StatusCode sc = StatusCode::SUCCESS;
//  sc = detStore()->retrieve(m_caloMgr);
//  if (sc.isFailure()) {
//    ATH_MSG_ERROR( "Unable to retrieve CaloIdManager from DetectorStore" );
//    return sc;
//  }
//  else{
//   ATH_MSG_INFO("CaloMgr is retrieved");
//  }


  ATH_CHECK( detStore()->retrieve(m_calo_id) );
  sc = m_bunchCrossingTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve bunchCrossingTool. Switch off CaloBaselineMon" );
    m_pedestalMon_BCIDmin = -1;
    m_bcidtoolMon_BCIDmax = -1; 
    return sc;
  }
  else{
   ATH_MSG_INFO("CaloMgr is retrieved");
  }
    
    //  ATH_CHECK( m_bunchCrossingTool.retrieve());
 
  return sc;
}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloBaselineMon::bookHistograms(){
// Book managed histograms. 

  ATH_MSG_DEBUG( "in bookHistograms()" );

  std::string TheTrigger;
  if (m_triggerChainProp == "")  TheTrigger="NoTrigSel";
  else TheTrigger = m_triggerChainProp;
  
  MonGroup  monGroup_general  ( this, "/CaloMonitoring/CaloBaseline/General", run, ATTRIB_MANAGED );
  MonGroup  monGroup_partition  ( this, "/CaloMonitoring/CaloBaseline/"+m_cellContainerName+TheTrigger, run, ATTRIB_MANAGED );

  std::string hName, hTitle;
  if (m_bool_pedestalMon) {
    hName  = "h1BCID_pedestalMon";
    hTitle = "BCID used for baseline monitoring";
    h1_BCID_pedestalMon = TH1I_LW::create(hName.c_str(), hTitle.c_str(),m_BCID0_nbins,m_BCID0_min,m_BCID0_max);
    h1_BCID_pedestalMon->GetXaxis()->SetTitle("BCID");
    h1_BCID_pedestalMon->GetYaxis()->SetTitle("Nb of events / BCID");
    monGroup_general.regHist(h1_BCID_pedestalMon).ignore();  
  }

  if (m_bool_bcidtoolMon) {
    hName  = "h1BCID_BCIDToolMon";
    hTitle = "BCID used for BCIDTool monitoring";
    h1_BCID_bcidtoolMon = TH1I_LW::create(hName.c_str(), hTitle.c_str(),m_BCID0_nbins,m_BCID0_min,m_BCID0_max);
    h1_BCID_bcidtoolMon->GetXaxis()->SetTitle("BCID");
    h1_BCID_bcidtoolMon->GetYaxis()->SetTitle("Nb of events / BCID");
    monGroup_general.regHist(h1_BCID_bcidtoolMon).ignore();  
  }
    
  m_partHistos.resize(m_partNames.size());
  for (uint iPart = 0;iPart <  m_partNames.size();iPart++){
    bookPartitionHistos(m_partHistos[iPart],iPart,monGroup_partition);
  }
    
  bookBaseHists(&monGroup_general).ignore(); //from base class

  return StatusCode::SUCCESS;
}

void CaloBaselineMon::bookPartitionHistos(partitionHistos& partition, uint partNumber, MonGroup& group)
{
  std::string hName, hTitle;
  
  int lb_nbins = 3000;

  if (m_bool_pedestalMon) {
    std::string str_auxTitle = " Empty BCID > "+ std::to_string(m_pedestalMon_BCIDmin) +"BCID away from last train";
    
    partition.hProf_pedestalMon_vs_EtaBCID.resize(m_nbOfEtaBins[partNumber]);
    for (uint iEta = 0; iEta < m_nbOfEtaBins[partNumber]; iEta ++){
      hName  = "hprof1d_pedestalMon_"+m_partNames[partNumber]+"_eta"+std::to_string(iEta);
      std::string str_eta0 = std::to_string( ((float) iEta)*m_etaBinWidth[partNumber]+m_etaMin[partNumber] );
      std::string str_eta1 = std::to_string( ((float) iEta + 1)*m_etaBinWidth[partNumber]+m_etaMin[partNumber] );
      hTitle = "Pedestal baseline ( "+str_auxTitle+") - "+m_partNames[partNumber]+" - " + str_eta0 +" < eta < " + str_eta1;
      partition.hProf_pedestalMon_vs_EtaBCID[iEta] = TProfile_LW::create(hName.c_str(), hTitle.c_str(),m_BCID0_nbins,m_BCID0_min,m_BCID0_max);
      partition.hProf_pedestalMon_vs_EtaBCID[iEta]->GetXaxis()->SetTitle("BCID");
      partition.hProf_pedestalMon_vs_EtaBCID[iEta]->GetYaxis()->SetTitle("E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]");
      group.regHist(partition.hProf_pedestalMon_vs_EtaBCID[iEta]).ignore();  
    }      
    
    hName  = "hprof1d_pedestalMon"+m_partNames[partNumber]+"_AllEta";
    hTitle = "Pedestal baseline ( "+str_auxTitle+") - "+m_partNames[partNumber];
    partition.hProf_pedestalMon_vs_Eta = TProfile_LW::create(hName.c_str(), hTitle.c_str(),m_nbOfEtaBins[partNumber],m_etaMin[partNumber],m_etaMax[partNumber]);
    partition.hProf_pedestalMon_vs_Eta->GetXaxis()->SetTitle("Eta");
    partition.hProf_pedestalMon_vs_Eta->GetYaxis()->SetTitle("E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]");
    group.regHist(partition.hProf_pedestalMon_vs_Eta).ignore();  
    
    hName  = "hprof1d_pedestalMon_"+m_partNames[partNumber]+"_LB";
    partition.hProf_pedestalMon_vs_LB = TProfile_LW::create(hName.c_str(), hTitle.c_str(),lb_nbins,0,(float) lb_nbins);
    partition.hProf_pedestalMon_vs_LB->GetXaxis()->SetTitle("Luminosity block");
    partition.hProf_pedestalMon_vs_LB->GetYaxis()->SetTitle("E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]");
    group.regHist(partition.hProf_pedestalMon_vs_LB).ignore();  
  }

  if (m_bool_bcidtoolMon) {
    int BCID_nbins = (int) m_bcidtoolMon_BCIDmax;
    float BCID_min = 0.;
    float BCID_max = m_bcidtoolMon_BCIDmax;
    std::string str_auxTitle = "BCID in bunch train";
    
    partition.hProf_bcidtoolMon_vs_EtaBCID.resize(m_nbOfEtaBins[partNumber]);
    for (uint iEta = 0; iEta < m_nbOfEtaBins[partNumber]; iEta ++){
      hName  = "hprof1d_bcidtoolMon_"+m_partNames[partNumber]+"_eta"+std::to_string(iEta);
      std::string str_eta0 = std::to_string( ((float) iEta)*m_etaBinWidth[partNumber]+m_etaMin[partNumber] );
      std::string str_eta1 = std::to_string( ((float) iEta + 1)*m_etaBinWidth[partNumber]+m_etaMin[partNumber] );
      hTitle = "BCIDtool baseline ( "+str_auxTitle+")-"+m_partNames[partNumber]+" - " + str_eta0 +" < eta < " + str_eta1;
      partition.hProf_bcidtoolMon_vs_EtaBCID[iEta] = TProfile_LW::create(hName.c_str(), hTitle.c_str(),BCID_nbins,BCID_min,BCID_max);
      partition.hProf_bcidtoolMon_vs_EtaBCID[iEta]->GetXaxis()->SetTitle("BCID");
      partition.hProf_bcidtoolMon_vs_EtaBCID[iEta]->GetYaxis()->SetTitle("E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]");
      group.regHist(partition.hProf_bcidtoolMon_vs_EtaBCID[iEta]).ignore();  
    }

    
    hName  = "hprof1d_bcidtoolMon_"+m_partNames[partNumber]+"_AllEta";
    hTitle = "BCIDtool baseline ( "+str_auxTitle+") - "+m_partNames[partNumber];
    partition.hProf_bcidtoolMon_vs_Eta = TProfile_LW::create(hName.c_str(), hTitle.c_str(),m_nbOfEtaBins[partNumber],m_etaMin[partNumber],m_etaMax[partNumber]);
    partition.hProf_bcidtoolMon_vs_Eta->GetXaxis()->SetTitle("Eta");
    partition.hProf_bcidtoolMon_vs_Eta->GetYaxis()->SetTitle("E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]");
    group.regHist(partition.hProf_bcidtoolMon_vs_Eta).ignore();  

    hName  = "hprof1d_bcidtoolMon_"+m_partNames[partNumber]+"_LB";
    partition.hProf_bcidtoolMon_vs_LB = TProfile_LW::create(hName.c_str(), hTitle.c_str(),lb_nbins,0,(float) lb_nbins);
    partition.hProf_bcidtoolMon_vs_LB->GetXaxis()->SetTitle("Luminosity block");
    partition.hProf_bcidtoolMon_vs_LB->GetYaxis()->SetTitle("E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]");
    group.regHist(partition.hProf_bcidtoolMon_vs_LB).ignore();  
  }

}

////////////////////////////////////////////////////////////////////////////
StatusCode CaloBaselineMon::fillHistograms() {
  
  StatusCode sc = StatusCode::SUCCESS;

  bool ifPass = 1;
  sc = checkFilters(ifPass);
  if(sc.isFailure() || !ifPass) return StatusCode::SUCCESS;
  
  const xAOD::EventInfo* eventInfo = nullptr;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );

  int lumiBlock = eventInfo->lumiBlock();
  int bcid = eventInfo->bcid();

  bool thisEvent_bool_pedestalMon = false;
  bool thisEvent_bool_bcidtoolMon = false;

  // Fill pedestalMon only when the bunch is empty and far away enough from the last train.
  if (m_bool_pedestalMon){
    if ((not m_bunchCrossingTool->isFilled(bcid)) and (m_bunchCrossingTool->gapAfterBunch(bcid) >= m_pedestalMon_BCIDmin*25.) and (m_bunchCrossingTool->gapBeforeBunch(bcid) >= m_pedestalMon_BCIDmin*25.)) thisEvent_bool_pedestalMon = true;
  }
  // Fill bcidtoolMon only when the bunch is in a bunch train and within accepted BCID range.
  if (m_bool_bcidtoolMon){
    if (m_bunchCrossingTool->isInTrain(bcid) and m_bunchCrossingTool->distanceFromFront(bcid) <= m_bcidtoolMon_BCIDmax*25.) thisEvent_bool_bcidtoolMon = true;
  }

  if (not (thisEvent_bool_pedestalMon or thisEvent_bool_bcidtoolMon)) return sc;

  const CaloCellContainer* cellCont = nullptr;
  sc=evtStore()->retrieve(cellCont, m_cellContainerName);
  if( sc.isFailure()  ||  !cellCont ) {
    ATH_MSG_WARNING("No CaloCell container found in TDS"); 
    return sc;
  }  

  // Initialize the sum per eta band for each partition
  for (uint iPart = 0;iPart <  m_partNames.size();iPart++){
    for (uint iEta = 0; iEta < m_nbOfEtaBins[iPart]; iEta ++){
      m_sum_partition_eta[iPart][iEta] = 0.;
    }
  }

  CaloCellContainer::const_iterator it = cellCont->begin(); 
  CaloCellContainer::const_iterator it_e = cellCont->end(); 

  for ( ; it!=it_e;++it) { 

    // cell info
    const CaloCell* cell = *it; 
    Identifier id = cell->ID();
    float energy = cell->energy();
    double eta = cell->caloDDE()->eta_raw();

    int partThisAlgo = 0;
    if  (m_calo_id->is_em(id)) partThisAlgo = m_partMap[0];
    if  (m_calo_id->is_hec(id)) partThisAlgo = m_partMap[1]; 
    if  (m_calo_id->is_fcal(id)) partThisAlgo = m_partMap[2]; 

   
    if ((eta<m_etaMin[partThisAlgo]) or (eta>m_etaMax[partThisAlgo])) continue;
    int etaBin = std::floor((eta-m_etaMin[partThisAlgo])*m_inv_etaBinWidth[partThisAlgo]);

    m_sum_partition_eta[partThisAlgo][etaBin] += energy;
  } // cell iter loop


  // Loop on cells is over. Now fill histograms with sum per eta.
  if (thisEvent_bool_pedestalMon ) {
    h1_BCID_pedestalMon->Fill(bcid);
    for (uint iPart = 0;iPart <  m_partNames.size();iPart++){ 
      for (uint iEta = 0; iEta < m_nbOfEtaBins[iPart]; iEta ++){
	// Normalize the sum by the \delta\eta.\delta\phi.\mu
	m_sum_partition_eta[iPart][iEta] = m_sum_partition_eta[iPart][iEta]*(m_inv_etaBinWidth[iPart])/(2*M_PI)/lbAverageInteractionsPerCrossing();
	float etaToBeFilled = ((float) iEta)*m_etaBinWidth[iPart] + m_etaMin[iPart];
	m_partHistos[iPart].hProf_pedestalMon_vs_Eta->Fill(etaToBeFilled,m_sum_partition_eta[iPart][iEta]);
	m_partHistos[iPart].hProf_pedestalMon_vs_EtaBCID[iEta]->Fill(bcid,m_sum_partition_eta[iPart][iEta]);
	m_partHistos[iPart].hProf_pedestalMon_vs_LB->Fill(lumiBlock,m_sum_partition_eta[iPart][iEta]);
      }
    }
  }

  if (thisEvent_bool_bcidtoolMon ) {
    h1_BCID_bcidtoolMon->Fill(bcid);
    for (uint iPart = 0;iPart <  m_partNames.size();iPart++){ 
      for (uint iEta = 0; iEta < m_nbOfEtaBins[iPart]; iEta ++){
	// Normalize the sum by the \delta\eta.\delta\phi.\mu
	m_sum_partition_eta[iPart][iEta] = m_sum_partition_eta[iPart][iEta]*(m_inv_etaBinWidth[iPart])/(2*M_PI)/lbAverageInteractionsPerCrossing();
	float etaToBeFilled = ((float) iEta)*m_etaBinWidth[iPart] + m_etaMin[iPart];
	m_partHistos[iPart].hProf_bcidtoolMon_vs_Eta->Fill(etaToBeFilled,m_sum_partition_eta[iPart][iEta]);
	m_partHistos[iPart].hProf_bcidtoolMon_vs_EtaBCID[iEta]->Fill(m_bunchCrossingTool->distanceFromFront(bcid)/25,m_sum_partition_eta[iPart][iEta]);
	m_partHistos[iPart].hProf_bcidtoolMon_vs_LB->Fill(lumiBlock,m_sum_partition_eta[iPart][iEta]);
      }
    }
  }

  return sc;
}


////////////////////////////////////////////////////////////////////////////
StatusCode CaloBaselineMon::procHistograms( ) {
  StatusCode sc = StatusCode::SUCCESS;
  return sc;
}
