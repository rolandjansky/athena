/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloMonitoring/HLTCaloESD_xAODTrigEMClusters.h"

#include "TrigCaloMonitoring/HLTCaloMonTool.h"

#include <TProfile2D.h>

//#include "egammaInterfaces/IegammaShowerShape.h"


HLTCaloESD_xAODTrigEMClusters::HLTCaloESD_xAODTrigEMClusters(const std::string & type, const std::string & name, const IInterface* parent): 
  IHLTMonTool(type, name, parent)
//m_ShowerShapeTool("egammaShowerShape/egammashowershape")
{
  declareProperty("HLTContainerRun2", m_HLT_cont_name_run2 = "HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma");
  declareProperty("HLTContainerRun3", m_HLT_cont_name_run3 = "HLT_FastCaloEMClusters");
  declareProperty("OFFContainer", m_OFF_cont_name = "egammaClusters");
  declareProperty("MonGroupName", m_mongroup_name = "");

  declareProperty("OFFTypes",  m_OFF_types);  
  declareProperty("HLTMinET",  m_HLT_min_et  = -1.0);
  declareProperty("OFFMinET",  m_OFF_min_et  = -1.0);
  declareProperty("MaxDeltaR", m_max_delta_r = 0.04);
}


HLTCaloESD_xAODTrigEMClusters::~HLTCaloESD_xAODTrigEMClusters() 
{

}


StatusCode HLTCaloESD_xAODTrigEMClusters::init() 
{
  /*
  if (m_ShowerShapeTool.empty()) {
    ATH_MSG_INFO("ShowerShape is empty");
    return StatusCode::SUCCESS;
  } 
  
  // retrieve egammaShowerShape Tool
  if(m_ShowerShapeTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve "<<m_ShowerShapeTool);
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG("Retrieved Tool " << m_ShowerShapeTool); 
  */
  return StatusCode::SUCCESS;
}


StatusCode HLTCaloESD_xAODTrigEMClusters::book() 
{
  ATH_MSG_DEBUG("book()");
  
  // Set HLT container name
  m_HLT_cont_name = m_HLT_cont_name_run2;
  if(evtStore()->contains<xAOD::TrigEMClusterContainer>(m_HLT_cont_name_run3)) m_HLT_cont_name = m_HLT_cont_name_run3;

  // Prepare folder structure
  if(m_mongroup_name.empty()) m_mongroup_name = m_HLT_cont_name;
  
  m_mongroup_name.insert(0,"HLT/HLTCaloESD/");
  
  addMonGroup(new MonGroup(this, m_mongroup_name, run));
  
  setCurrentMonGroup(m_mongroup_name);
  
  // HLT clusters
  addHistogram(new TH1F     ("HLT_num",     "Number of HLT Clusters; Num Clusters; Entries",  51,  -0.5,  50.5));
  addHistogram(new TH2F     ("HLT_num_map", "Number of HLT Clusters; #eta;         #phi;",    50,  -5.0,   5.0, 64, -M_PI, M_PI));
  addHistogram(new TH1F     ("HLT_et",      "HLT Cluster E_{T};      E_{T} [GeV];  Entries", 100,   0.0, 100.0));
  addHistogram(new TH1F     ("HLT_eta",     "HLT Cluster #eta;       #eta;         Entries",  50,  -5.0,   5.0));
  addHistogram(new TH1F     ("HLT_phi",     "HLT Cluster #phi;       #phi;         Entries",  64, -M_PI,  M_PI));
  addHistogram(new TH1F     ("HLT_size",    "HLT Cluster Size;       Num Cells;    Entries", 101,  -5.0, 1005.0));
  addHistogram(new TH1F     ("HLT_reta",    "HLT Cluster R_{eta};    R_{eta};      Entries", 100,  -5.0,   5.0)); /////////////////////////////////////////////////////////////
  addHistogram(new TH1F     ("HLT_rstrip",  "HLT Cluster R_{strip};  R_{strip};    Entries", 100,  -5.0,   5.0)); /////////////////////////////////////////////////////////////

  // OFF clusters
  addHistogram(new TH1F     ("OFF_num",     "Number of OFF Clusters; Num Clusters; Entries",  51,  -0.5,  50.5));
  addHistogram(new TH2F     ("OFF_num_map", "Number of OFF Clusters; #eta;         #phi",     50,  -5.0,   5.0, 64, -M_PI, M_PI));
  addHistogram(new TH1F     ("OFF_et",      "OFF Cluster E_{T};      E_{T} [GeV];  Entries", 100,   0.0, 100.0));
  addHistogram(new TH1F     ("OFF_eta",     "OFF Cluster #eta;       #eta;         Entries",  50,  -5.0,   5.0));
  addHistogram(new TH1F     ("OFF_phi",     "OFF Cluster #phi;       #phi;         Entries",  64, -M_PI,  M_PI));
  addHistogram(new TH1F     ("OFF_type",    "OFF Cluster Type;       Size Enum;    Entries",  13,   0.5,  13.5));
  addHistogram(new TH1F     ("OFF_size",    "OFF Cluster Size;       Num Cells;    Entries", 101,  -5.0, 1005.0));

  // OFF clusters (no HLT matches)
  addHistogram(new TH1F     ("OFF_no_HLT_matches_num",     "Number of OFF Clusters (No HLT Matches); Num Clusters; Entries", 101,  -0.5, 100.5));
  addHistogram(new TH2F     ("OFF_no_HLT_matches_num_map", "Number of OFF Clusters (No HLT Matches); #eta;         #phi",     50,  -5.0,   5.0, 64, -M_PI, M_PI));
  addHistogram(new TH1F     ("OFF_no_HLT_matches_et",      "OFF Cluster (No HLT Matches) E_{T};      E_{T} [GeV];  Entries", 100,   0.0, 100.0));
  addHistogram(new TH1F     ("OFF_no_HLT_matches_eta",     "OFF Cluster (No HLT Matches) #eta;       #eta;         Entries",  50,  -5.0,   5.0));
  addHistogram(new TH1F     ("OFF_no_HLT_matches_phi",     "OFF Cluster (No HLT Matches) #phi;       #phi;         Entries",  64, -M_PI,  M_PI));
  addHistogram(new TH1F     ("OFF_no_HLT_matches_type",    "OFF Cluster (No HLT Matches) Type;       Size Enum;    Entries",  13,   0.5,  13.5));
  addHistogram(new TH1F     ("OFF_no_HLT_matches_size",    "OFF Cluster (No HLT Matches) Size;       Num Cells;    Entries", 101,  -5.0, 1005.0));

  // OFF clusters (with HLT match)
  addHistogram(new TH1F     ("OFF_with_HLT_match_num",     "Number of OFF Clusters (With HLT Match); Num Clusters; Entries", 101,  -0.5, 100.5));
  addHistogram(new TH2F     ("OFF_with_HLT_match_num_map", "Number of OFF Clusters (With HLT Match); #eta;         #phi",     50,  -5.0,   5.0, 64, -M_PI, M_PI));
  addHistogram(new TH1F     ("OFF_with_HLT_match_et",      "OFF Cluster (With HLT Match) E_{T};      E_{T} [GeV];  Entries", 100,   0.0, 100.0));
  addHistogram(new TH1F     ("OFF_with_HLT_match_eta",     "OFF Cluster (With HLT Match) #eta;       #eta;         Entries",  50,  -5.0,   5.0));
  addHistogram(new TH1F     ("OFF_with_HLT_match_phi",     "OFF Cluster (With HLT Match) #phi;       #phi;         Entries",  64, -M_PI,  M_PI));
  addHistogram(new TH1F     ("OFF_with_HLT_match_type",    "OFF Cluster (With HLT Match) Type;       Size Enum;    Entries",  13,   0.5,  13.5));
  addHistogram(new TH1F     ("OFF_with_HLT_match_size",    "OFF Cluster (With HLT Match) Size;       Num Cells;    Entries", 101,  -5.0, 1005.0));

  // HLT clusters vs. OFF clusters
  addHistogram(new TH1F       ("HLT_vs_OFF_minimum_delta_r",   "HLT vs OFF Cluster #DeltaR;                 #DeltaR;                                             Entries",  50,   0.0,   0.1));
  addHistogram(new TH1F       ("HLT_vs_OFF_delta_eta",         "HLT vs OFF Cluster #Delta#eta;              #eta_{_{ OFF}} - #eta_{_{ HLT}};                    Entries",  50,  -0.2,   0.2));
  addHistogram(new TH1F       ("HLT_vs_OFF_delta_phi",         "HLT vs OFF Cluster #Delta#phi;              #phi_{_{OFF}} - #phi_{_{HLT}};                      Entries",  50,   0.0,   0.02));
  addHistogram(new TH2F       ("HLT_vs_OFF_et_vs_et",          "HLT vs OFF Cluster E_{T};                   E_{T_{OFF}} [GeV];                        E_{T_{HLT}} [GeV]", 100,   0.0, 100.0, 100,   0.0, 100.0));
  addHistogram(new TH1F       ("HLT_vs_OFF_resolution",        "HLT vs OFF Cluster #DeltaE_{T} / E_{T};     E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}};            Entries", 100, -10.0,  10.0)); 
  addHistogram(new TProfile2D ("HLT_vs_OFF_resolution_map",    "HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta;                                                   #phi",  50,  -5.0,   5.0,  64, -M_PI,  M_PI));
  addProfile  (new TProfile   ("HLT_vs_OFF_resolution_vs_et",  "HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; E_{T_{OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >",  100,  0.0,   100.0));
  addProfile  (new TProfile   ("HLT_vs_OFF_resolution_vs_eta", "HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #eta_{_{ OFF}}; < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >",  20,  -5.0,   5.0));
  addProfile  (new TProfile   ("HLT_vs_OFF_resolution_vs_phi", "HLT vs OFF Cluster < #DeltaE_{T} / E_{T} >; #phi_{_{OFF}};  < E_{T_{OFF}} - E_{T_{HLT}} / E_{T_{OFF}} >",  16, -M_PI,  M_PI));

  return StatusCode::SUCCESS;
}


StatusCode HLTCaloESD_xAODTrigEMClusters::fill() 
{
  ATH_MSG_DEBUG("fill()");

  // retrieve HLT clusters 
  const xAOD::TrigEMClusterContainer* HLT_cont = 0;
  if(evtStore()->retrieve(HLT_cont, m_HLT_cont_name).isFailure()) {
    ATH_MSG_INFO ("could not retrieve the xAOD::TrigEMClusterContainer: " << m_HLT_cont_name); 
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG("successfully retrieved xAOD::TrigEMClusterContainer: " << m_HLT_cont_name << " (size = " << HLT_cont->size() << ")");
  }
  
  // loop over HLT clusters
  xAOD::TrigEMClusterContainer::const_iterator HLT_itr = HLT_cont->begin();
  xAOD::TrigEMClusterContainer::const_iterator HLT_end = HLT_cont->end();

  unsigned int HLT_num = 0;

  for(; HLT_itr != HLT_end; ++HLT_itr) {
    
    if((*HLT_itr)->et() < m_HLT_min_et) continue;
    
    HLT_num++;

    hist2("HLT_num_map")->Fill((*HLT_itr)->eta(),(*HLT_itr)->phi());
    hist ("HLT_et")     ->Fill((*HLT_itr)->et() / 1000);
    hist ("HLT_eta")    ->Fill((*HLT_itr)->eta());
    hist ("HLT_phi")    ->Fill((*HLT_itr)->phi());    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////// RSTRIP & RETA
    /*
      float HLT_emaxs1 = (*HLT_itr)->emaxs1();
      float HLT_e2ts1  = (*HLT_itr)->e2ts1();
      
      if((HLT_emaxs1 + HLT_e2ts1)> 0.2) {
        float HLT_rstrip = (HLT_emaxs1 - HLT_e2ts1) / (HLT_emaxs1 + HLT_e2ts1);
        hist("HLT_rstrip")->Fill(HLT_rstrip);
      }
    
      float HLT_e237 = (*HLT_itr)->e237();
      float HLT_e277 = (*HLT_itr)->e277();
    
      if(HLT_e277 > 0.2) {
        float HLT_reta = HLT_e237 / HLT_277;
        hist("HLT_reta")-> Fill(HLT_reta);
      }
    */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    hist ("HLT_size")   ->Fill((*HLT_itr)->nCells());
    
  } // end loop over HLT clusters
  
  hist("HLT_num")->Fill(HLT_num);

  // retrieve OFF clusters
  const xAOD::CaloClusterContainer* OFF_cont = 0; 
  if(evtStore()->retrieve(OFF_cont, m_OFF_cont_name).isFailure()) {
    ATH_MSG_INFO ("could not retrieve the xAOD::CaloClusterContainer: " << m_OFF_cont_name);
    return StatusCode::SUCCESS;
  } 
  else {
    ATH_MSG_DEBUG("successfully retrieved xAOD::CaloClusterContainer: " << m_OFF_cont_name << " (size = " << OFF_cont->size() << ")");
  }  
  
  // loop over OFF clusters
  xAOD::CaloClusterContainer::const_iterator OFF_itr = OFF_cont->begin();
  xAOD::CaloClusterContainer::const_iterator OFF_end = OFF_cont->end();
  
  xAOD::TrigEMClusterContainer::const_iterator HLT_match; // for matching

  unsigned int OFF_num = 0;

  unsigned int OFF_no_matches_num = 0;
  unsigned int OFF_with_match_num = 0;

  for(; OFF_itr != OFF_end; ++OFF_itr) {

    if((*OFF_itr)->et() <m_OFF_min_et) continue;

    bool OFF_type_match = false;
    
    for(unsigned int n=0; n< m_OFF_types.size(); n++) {
      if((*OFF_itr)->clusterSize() == m_OFF_types[n]) { OFF_type_match = true; break; } 
    }

    if(!m_OFF_types.empty() && !OFF_type_match) continue;

    OFF_num++;

    hist2("OFF_num_map")->Fill((*OFF_itr)->eta(),(*OFF_itr)->phi());
    hist ("OFF_et")     ->Fill((*OFF_itr)->et() / 1000);
    hist ("OFF_eta")    ->Fill((*OFF_itr)->eta());
    hist ("OFF_phi")    ->Fill((*OFF_itr)->phi());
    hist ("OFF_type")   ->Fill((*OFF_itr)->clusterSize());

    const CaloClusterCellLink* OFF_cell_links = (*OFF_itr)->getCellLinks();

    if(OFF_cell_links) { 
      hist("OFF_size")->Fill(OFF_cell_links->size());
    }
    else  ATH_MSG_WARNING("no valid OFF cell links");

    // matching starts here....
    HLT_itr = HLT_cont->begin(); // reset HLT itr to cont beginning   

    float min_delta_r  = 999999; 
  
    for(; HLT_itr != HLT_end; ++HLT_itr) { // find HLT match to OFF

      if((*HLT_itr)->et() < m_HLT_min_et) continue;
      
      float delta_r = DeltaR((*OFF_itr)->eta(), (*OFF_itr)->phi(), (*HLT_itr)->eta(), (*HLT_itr)->phi());

      if(delta_r < min_delta_r) { 
	
	min_delta_r = delta_r; 
       	HLT_match  =  HLT_itr; // avoid HLT double counts?
      } 
    
    }  // end loop over HLT clusters 
    
    hist("HLT_vs_OFF_minimum_delta_r")->Fill(min_delta_r);
    
    if(min_delta_r > m_max_delta_r) { // no HLT matches...
      
      OFF_no_matches_num++;

      hist2("OFF_no_HLT_matches_num_map")->Fill((*OFF_itr)->eta(),(*OFF_itr)->phi());
      hist ("OFF_no_HLT_matches_et" )    ->Fill((*OFF_itr)->et() / 1000);
      hist ("OFF_no_HLT_matches_eta")    ->Fill((*OFF_itr)->eta());
      hist ("OFF_no_HLT_matches_phi")    ->Fill((*OFF_itr)->phi());
      hist ("OFF_no_HLT_matches_type")   ->Fill((*OFF_itr)->clusterSize());

      if(OFF_cell_links)
      hist ("OFF_no_HLT_matches_size")   ->Fill(OFF_cell_links->size());

    }
    else { // with HLT match....

      OFF_with_match_num++;

      hist2("OFF_with_HLT_match_num_map")->Fill((*OFF_itr)->eta(),(*OFF_itr)->phi());
      hist ("OFF_with_HLT_match_et" )    ->Fill((*OFF_itr)->et() / 1000); 
      hist ("OFF_with_HLT_match_eta")    ->Fill((*OFF_itr)->eta());
      hist ("OFF_with_HLT_match_phi")    ->Fill((*OFF_itr)->phi()); 
      hist ("OFF_with_HLT_match_type")   ->Fill((*OFF_itr)->clusterSize());

      if(OFF_cell_links)
      hist ("OFF_with_HLT_match_size")   ->Fill(OFF_cell_links->size()); 

      // comment here.... 
      
      hist2("HLT_vs_OFF_et_vs_et") ->Fill((*OFF_itr)->et()/1000, (*HLT_match)->et()/1000); 
      
      float delta_et  = (((*OFF_itr)->et() - (*HLT_match)->et()) / (*OFF_itr)->et()) * 100;
      float delta_eta = (*OFF_itr)->eta() - (*HLT_match)->eta(); 
      float delta_phi = DeltaPhi((*OFF_itr)->phi(), (*HLT_match)->phi());
      
      hist ("HLT_vs_OFF_delta_eta")->Fill(delta_eta);    
      hist ("HLT_vs_OFF_delta_phi")->Fill(delta_phi);
      
      hist   ("HLT_vs_OFF_resolution")       ->Fill(delta_et);
      hist2  ("HLT_vs_OFF_resolution_map")   ->Fill((*OFF_itr)->eta(),(*OFF_itr)->phi(),delta_et);
      profile("HLT_vs_OFF_resolution_vs_et" )->Fill((*OFF_itr)->et()*0.001,delta_et);
      profile("HLT_vs_OFF_resolution_vs_eta")->Fill((*OFF_itr)->eta(),delta_et);  
      profile("HLT_vs_OFF_resolution_vs_phi")->Fill((*OFF_itr)->phi(),delta_et); 
    }
    
  } // end loop over OFF clusters

  hist("OFF_num")->Fill(OFF_num);   

  hist("OFF_no_HLT_matches_num")->Fill(OFF_no_matches_num);
  hist("OFF_with_HLT_match_num")->Fill(OFF_with_match_num);
  
  return StatusCode::SUCCESS;
}


StatusCode HLTCaloESD_xAODTrigEMClusters::proc() 
{
  return StatusCode::SUCCESS; 
}
