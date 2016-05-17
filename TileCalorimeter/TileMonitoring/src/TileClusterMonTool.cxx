/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileClusterMonTool.cxx
// PACKAGE:  TileMonitoring  
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch) 
//           
//
// ********************************************************************

#include "TileMonitoring/TileClusterMonTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include <cmath>


/*---------------------------------------------------------*/
TileClusterMonTool::TileClusterMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_TileClusterTrig(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("energyThreshold",m_Threshold = 500.); //Threshold in MeV
  declareProperty("clustersContainerName",m_clustersContName="TileTopoCluster"); //SG Cluster Container

  m_path = "/Tile/Cluster"; //ROOT File directory.
  //Avoid the trailing slash or you will get a double slash in the histogram path: SHIFT//Tile/...
}

/*---------------------------------------------------------*/
TileClusterMonTool::~TileClusterMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileClusterMonTool:: initialize() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in initialize()" );

  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileClusterMonTool::bookHistTrig( int trig ) {
/*---------------------------------------------------------*/

  // Taking care of the mapping between vector element and trigger type: if vector empty, element will be 0 and so on

  //m_activeTrigs[ trig ] = m_TilenClusters.size();
  m_activeTrigs[ trig ] = m_TileClusterTrig; 
  int element = m_activeTrigs[ trig ];

  std::string runNumStr = getRunNumStr();

  m_TileClusterTrig++; //increment index vector

  m_TilenClusters.push_back( book1F(m_TrigNames[trig],"tilenClusters" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile Cluster number",50,0., 10., run, ATTRIB_MANAGED, "", "mergeRebinned") );
  //m_TilenClusters[ element ]->SetBit(TH1::kCanRebin);

  m_TileClusternCells.push_back( book1F(m_TrigNames[trig],"tileClusternCells" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile number of cells in most en. Cluster",100,0., 100., run, ATTRIB_MANAGED, "", "mergeRebinned") );
  //m_TileClusternCells[ element ]->SetBit(TH1::kCanRebin);

  m_TileClusterEt.push_back( book1F(m_TrigNames[trig],"tileClusterEt" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile Et in most en. Cluster (MeV)",80,0., 20000.) );
  m_TileClusterEt[ element ]->GetXaxis()->SetTitle("most en. Cluster Et (MeV)");
    
  m_TileClusterEtaPhi.push_back( book2F(m_TrigNames[trig],"tileClusterEtaPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile Position of most en. Cluster",21,-2.025, 2.025,64,-3.15,3.15) );
  m_TileClusterEtaPhi[ element ]->GetXaxis()->SetTitle("#eta");
  m_TileClusterEtaPhi[ element ]->GetYaxis()->SetTitle("#phi");

  m_TileClusterEneEtaPhi.push_back( bookProfile2D(m_TrigNames[trig],"tileClusterEneEtaPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile clusters average energy deposition versus #eta and #phi ",21,-2.025,2.025,64,-3.15, 3.15,-3.e6,3.e6) );
  m_TileClusterEneEtaPhi[ element ]->GetXaxis()->SetTitle("#eta");
  m_TileClusterEneEtaPhi[ element ]->GetYaxis()->SetTitle("#phi");

  m_TileClusterAllEtaPhi.push_back( book2F(m_TrigNames[trig],"tileClusterAllEtaPhi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile clusters number versus #eta and #phi ",21,-2.025,2.025,64,-3.15, 3.15) );
  m_TileClusterAllEtaPhi[ element ]->GetXaxis()->SetTitle("#eta");
  m_TileClusterAllEtaPhi[ element ]->GetYaxis()->SetTitle("#phi");

  m_TileClusterEtaPhiDiff.push_back( book2F(m_TrigNames[trig],"tileClusterEtaPhiDiff" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile pos. correlation of Clus. opposite to most en. Cluster",21,-2.025, 2.025,64,0.,6.4) );
  m_TileClusterEtaPhiDiff[ element ]->GetXaxis()->SetTitle("#Delta #eta");
  m_TileClusterEtaPhiDiff[ element ]->GetYaxis()->SetTitle("#Delta #phi");

  m_TileClusterEneDiff.push_back( book1F(m_TrigNames[trig],"tileClusterEneDiff" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile en. correlation of Clus. opposite to most en. Cluster",200,-10000., 10000.) );
  m_TileClusterEneDiff[ element ]->GetXaxis()->SetTitle("Energy diff. (MeV)");  
    
  m_TileClusterTimeDiff.push_back( book1F(m_TrigNames[trig],"tileClusterTimeDiff" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile time correlation of Clus. opposite to most en. Cluster",200,-100., 100.) );
  m_TileClusterTimeDiff[ element ]->GetXaxis()->SetTitle("Time diff. (ns)");  
    
  m_TileAllClusterEnergy.push_back( book1F(m_TrigNames[trig],"tileAllClusterEnergy" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile All Clusters Energy (MeV)",80,0., 20000.) );
  m_TileAllClusterEnergy[ element ]->GetXaxis()->SetTitle("All Clusters Energy (MeV)");

  m_TileClusterEneLumi.push_back( bookProfile(m_TrigNames[trig],"tileClusterEneLumi" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile Energy of most en. Cluster (MeV) as a function of the LumiBlock",10,0., 20.,-2000.,200000., run, ATTRIB_MANAGED, "", "mergeRebinned") );
  m_TileClusterEneLumi[ element ]->GetXaxis()->SetTitle("Lumi Block");
  m_TileClusterEneLumi[ element ]->GetYaxis()->SetTitle("Most Energetic cluster Averge Energy (MeV)");
  //m_TileClusterEneLumi[ element ]->SetBit(TH1::kCanRebin);

  m_TileClusterSumEt.push_back( book1F(m_TrigNames[trig],"tileClusterSumEt" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile Clusters SumEt",100,0., 20000.) );
  m_TileClusterSumEt[ element ]->GetXaxis()->SetTitle("SumEt (GeV)");

  m_TileClusterSumPx.push_back( book1F(m_TrigNames[trig],"tileClusterSumPx" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile Clusters Px",101,-10000., 10000.) );
  m_TileClusterSumPx[ element ]->GetXaxis()->SetTitle("SumPx (GeV)");

  m_TileClusterSumPy.push_back( book1F(m_TrigNames[trig],"tileClusterSumPy" + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile Clusters Py",101,-10000., 10000.) );
  m_TileClusterSumPy[ element ]->GetXaxis()->SetTitle("SumPy (GeV)");



  for(int p =  PartEBA; p < NPartHisto; p++) {

    m_TileClusterEnergy[ p ].push_back( book1F(m_TrigNames[trig]+"/"+m_PartNames[p],"tileClusterEnergy" + m_PartNames[p] + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": Tile Energy in most en. Cluster (MeV)",80,0., 20000.) );
    m_TileClusterEnergy[ p ][ element ]->GetXaxis()->SetTitle("most en. Cluster Energy (MeV)");
  }

  return StatusCode::SUCCESS;

}

/// BookHistogram method is called at every event block ,lumi block and run.
/// At the moment we need to book only every run
/// It calls bookHistTrig for the 'AnyTrig' trigger type
/// All concrete trigger type histograms will only be booked when needed
/*---------------------------------------------------------*/
StatusCode TileClusterMonTool::bookHistograms()
/*---------------------------------------------------------*/
{
  ATH_MSG_INFO( "in bookHistograms()" );

  cleanHistVec();

    /*
  if (bookHistTrig( AnyTrig ).isFailure()) {
    ATH_MSG_WARNING(  "Error booking Cluster histograms for Trigger " << m_TrigNames[AnyTrig] );
  }
    */

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void  TileClusterMonTool::cleanHistVec() {
/*---------------------------------------------------------*/

  for (int i = 0; i < 9; i++) {
    m_activeTrigs[i] = -1;
  }

  m_TileClusterTrig = 0;
  m_TilenClusters.clear();
  m_TileClusternCells.clear();
  //m_TileClusterTime.clear() ;
  m_TileClusterEt.clear();
  m_TileClusterEtaPhi.clear();
  //m_TileClusterEta.clear() ;
  //m_TileClusterPhi.clear() ;
  //m_TileClusterEneEta.clear() ;
  //m_TileClusterEnePhi.clear() ;
  m_TileClusterEneEtaPhi.clear();
  m_TileClusterAllEtaPhi.clear();
  m_TileClusterEtaPhiDiff.clear();
  m_TileClusterEneDiff.clear();
  m_TileClusterTimeDiff.clear();
  m_TileAllClusterEnergy.clear();
  //m_TileClusterEnergyOvThr.clear() ;
  //m_TileClusterEneTim.clear();
  m_TileClusterEneLumi.clear();
  m_TileClusterSumEt.clear();
  m_TileClusterSumPx.clear();
  m_TileClusterSumPy.clear();

  for (int part = PartEBA; part < NPartHisto; part++) {
    m_TileClusterEnergy[part].clear();
    //m_TileClusterEnergyRebin[part].clear() ;
  }

}

/*---------------------------------------------------------*/
StatusCode TileClusterMonTool::fillHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in fillHistograms()" );

  // get Lvl1 Trigger word
  fillEvtInfo();
  uint32_t lvl1info = getL1info();
  //uint32_t evtNum = getEvtNum();
  uint32_t lumi = getLumiBlock();
  get_eventTrigs(lvl1info); //fill  m_eventTrigs; it always contains at least one element: AnyTrig.
  
  //check if trigger type is new and therefore some histograms must be booked
  for (unsigned int i=0; i< m_eventTrigs.size();i++) {
    if ( vecIndx(i) < 0 ) {
      // book histograms
      if ( bookHistTrig(  m_eventTrigs[i] ).isFailure() ) {
        ATH_MSG_WARNING( "Error booking Cluster histograms for Trigger " << m_TrigNames[m_eventTrigs[i]] );
      }
    }
  }


  // Pointer to a Tile cell container
  const xAOD::CaloClusterContainer* cluster_container;

  //Retrieve Cluster collection from SG
  CHECK( evtStore()->retrieve(cluster_container, m_clustersContName) );

  ATH_MSG_VERBOSE( "TileClusterMonTool: Retrieval of Tile clusters from container " << m_clustersContName << " succeeded"  );

  int    nClusters        =   -10 ;
  int    ncells_most      =   -10 ;
  float energy_most      =     0.;
  float et_most          =     0.;
  float eta_most         =     0.;
  float phi_most         =     0.;
  bool	 set_most         = false ;
  float time_most        =     0.;
  int    partition_most   =     0 ;
  float cell_e_most      =     0.;
  float energy_corr      =     0.;
  float eta_corr         =     0.;
  float phi_corr         =     0.;
  float time_corr        =     0.;
  bool	 set_corr         = false ;
  float cell_e_corr      =     0.;

  nClusters = cluster_container->size() ;

  xAOD::CaloClusterContainer::const_iterator iCluster = cluster_container->begin();
  xAOD::CaloClusterContainer::const_iterator lastCluster  = cluster_container->end();
  for (; iCluster != lastCluster; ++iCluster) {

    if ((*iCluster)->e() > energy_most) {
      const xAOD::CaloCluster* cluster_ptr = *iCluster;
      energy_most = cluster_ptr->e();
      et_most = cluster_ptr->et();
      eta_most = cluster_ptr->eta();
      phi_most = cluster_ptr->phi();
      //      time_most   = cluster_ptr->getTime();
      ncells_most = cluster_ptr->size();
      set_most = true;

      xAOD::CaloCluster::const_cell_iterator it = cluster_ptr->cell_begin();
      xAOD::CaloCluster::const_cell_iterator it_end = cluster_ptr->cell_end();
      for (; it != it_end; ++it) {
        if ((*it)->energy() > cell_e_most) {
          cell_e_most = (*it)->energy();
          time_most = (*it)->time();
          partition_most = getPartition(*it);
        }
      }

    }

  }
  
  if (set_most) {

    for (unsigned int i=0; i< m_eventTrigs.size(); i++ ) {
      m_TileClusterEnergy[NumPart][ vecIndx(i) ]->Fill(energy_most,1.);
      m_TileClusterEnergy[partition_most][ vecIndx(i) ]->Fill(energy_most,1.);
      //m_TileClusterEnergyRebin[NumPart][ vecIndx(i) ]->Fill(energy_most,1.);
      //m_TileClusterEnergyRebin[partition_most][ vecIndx(i) ]->Fill(energy_most,1.);
      m_TileClusterEt[ vecIndx(i) ]->Fill(et_most,1.);
      //m_TileClusterEta[ vecIndx(i) ]->Fill(eta_most,1.);
      //m_TileClusterPhi[ vecIndx(i) ]->Fill(phi_most,1.);
      //m_TileClusterTime[ vecIndx(i) ]->Fill(time_most,1.);
      //m_TileClusterEneTim[ vecIndx(i) ]->Fill(evtNum,energy_most);
      m_TileClusterEneLumi[ vecIndx(i) ]->Fill(lumi,energy_most);
      m_TileClusterEtaPhi[ vecIndx(i) ]->Fill(eta_most,phi_most,1.);
      m_TileClusternCells[ vecIndx(i) ]->Fill(ncells_most,1.);

    }
  }

  float sumpx = 0.;
  float sumpy = 0.;

  iCluster = cluster_container->begin();
  for (; iCluster != lastCluster; ++iCluster) {

    const xAOD::CaloCluster* cluster_ptr = *iCluster;    // pointer to cluster object
    float energy = cluster_ptr->e();
    float phi = cluster_ptr->phi();
    float eta = cluster_ptr->eta();
    float pt = cluster_ptr->pt();
    float px = pt * cos(phi);
    float py = pt * sin(phi);
    //    float time   = cluster_ptr->getTime();

    sumpx += px;
    sumpy += py;

    if ((fabs(eta) > 5.) || (fabs(phi) > 3.2)) {
      ATH_MSG_WARNING( "Cluster information out of Eta - Phi boundaries, skipping this cluster:"  );
      ATH_MSG_WARNING( "Energy (MeV)=" << energy << "\tEta=" << eta << "\tPhi=" << phi );
      continue;
    }

    for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
      m_TileAllClusterEnergy[vecIndx(i)]->Fill(energy, 1.);
      m_TileAllClusterEnergy[vecIndx(i)]->Fill(energy, 1.);
      //m_TileClusterEneEta[ vecIndx(i) ]->Fill(eta,energy);
      //m_TileClusterEnePhi[ vecIndx(i) ]->Fill(phi,energy);
      m_TileClusterEneEtaPhi[vecIndx(i)]->Fill(eta, phi, energy);
      m_TileClusterAllEtaPhi[vecIndx(i)]->Fill(eta, phi);
      //if (energy > m_Threshold) {
      //m_TileClusterEnergyOvThr[ vecIndx(i) ]->Fill(energy,1.);
      //}
    }
    if ((phi_most * phi < 0.) && (energy > energy_corr)) {
      energy_corr = energy;
      eta_corr = eta;
      phi_corr = phi;
      set_corr = true;

      xAOD::CaloCluster::const_cell_iterator it = cluster_ptr->cell_begin();
      xAOD::CaloCluster::const_cell_iterator it_end = cluster_ptr->cell_end();
      for (; it != it_end; ++it) {
        if ((*it)->energy() > cell_e_corr) {
          cell_e_corr = (*it)->energy();
          time_corr = (*it)->time();
        }
      }

    }

    if (msgLvl(MSG::VERBOSE)) {

      msg(MSG::VERBOSE) << "Cluster nCells= " << cluster_ptr->size() << endmsg;
      msg(MSG::VERBOSE) << "Cluster Transverse Energy= " << energy << "\tEt()= " << cluster_ptr->et() << endmsg;
      msg(MSG::VERBOSE) << "Cluster Eta= " << eta << "\tPhi= " << phi << endmsg;
    }


  }

  ATH_MSG_DEBUG( "Number of clusters in the event: " << nClusters );

  float sumet = sqrt(sumpx * sumpx + sumpy * sumpy);

  for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
    m_TilenClusters[vecIndx(i)]->Fill(nClusters, 1.);
    m_TileClusterSumEt[vecIndx(i)]->Fill(sumet, 1.);
    m_TileClusterSumPx[vecIndx(i)]->Fill(sumpx, 1.);
    m_TileClusterSumPy[vecIndx(i)]->Fill(sumpy, 1.);
  }

  if (set_corr) {

    float e_diff = 0.;
    float t_diff = 0.;
    if (phi_most > 0.) {
      e_diff = energy_most - energy_corr;
      t_diff = time_most - time_corr;
    } else {
      e_diff = energy_corr - energy_most;
      t_diff = time_most - time_corr;
    }
    for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
      m_TileClusterTimeDiff[vecIndx(i)]->Fill(t_diff, 1.);
      m_TileClusterEneDiff[vecIndx(i)]->Fill(e_diff, 1.);
      m_TileClusterEtaPhiDiff[vecIndx(i)]->Fill(fabs(eta_corr) - fabs(eta_most), fabs(phi_corr - phi_most), 1.);
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileClusterMonTool::procHistograms() {
/*---------------------------------------------------------*/

  if( endOfLumiBlockFlag() ||  endOfRunFlag() ) { 

    ATH_MSG_INFO( "in procHistograms()" );
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileClusterMonTool::checkHists(bool /* fromFinalize */) {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in checkHists()" );

  return StatusCode::SUCCESS;
}

