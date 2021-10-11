/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTCaloESD_CaloCells.h"

#include <TProfile2D.h>


HLTCaloESD_CaloCells::HLTCaloESD_CaloCells(const std::string & type, const std::string & name, const IInterface* parent): 
  IHLTMonTool(type, name, parent)
{
  declareProperty("HLTContainer", m_HLT_cont_name = "HLT_CaloCellContainer_TrigCaloCellMaker");
  declareProperty("OFFContainer", m_OFF_cont_name = "AllCalo");
  declareProperty("MonGroupName", m_mongroup_name = "");
  
  declareProperty("HLTMinET",     m_HLT_min_et =  -1.0);
  declareProperty("OFFMinET",     m_OFF_min_et =  -1.0);
}


HLTCaloESD_CaloCells::~HLTCaloESD_CaloCells() 
{

}


StatusCode HLTCaloESD_CaloCells::init() 
{
  return StatusCode::SUCCESS;
}


StatusCode HLTCaloESD_CaloCells::book() 
{
  ATH_MSG_DEBUG("book()");
  
  // prepare folder structure
  if(m_mongroup_name.empty()) m_mongroup_name = m_HLT_cont_name;
  
  m_mongroup_name.insert(0,"HLT/HLTCaloESD/");
  
  addMonGroup(new MonGroup(this, m_mongroup_name, run));
  
  //if(m_show_layer != -1) m_mongroup_name.append(std::to_string(m_show_layer));
  
  setCurrentMonGroup(m_mongroup_name);
  
  // HLT cells
  addHistogram(new TH1F       ("HLT_LARG_num",        "Number of HLT LAr Cells;  Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("HLT_LARG_num_map",    "Number of HLT LAr Cells;  #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("HLT_LARG_energy",     "HLT LAr Cell Energy;      Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("HLT_LARG_energy_map", "HLT LAr Cell < Energy >;  #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));
  
  addHistogram(new TH1F       ("HLT_EMB__num",        "Number of HLT EMB Cells;  Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("HLT_EMB__num_map",    "Number of HLT EMB Cells;  #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("HLT_EMB__energy",     "HLT EMB Cell Energy;      Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("HLT_EMB__energy_map", "HLT EMB Cell < Energy >;  #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));
  
  addHistogram(new TH1F       ("HLT_EMEC_num",        "Number of HLT EMEC Cells; Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("HLT_EMEC_num_map",    "Number of HLT EMEC Cells; #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("HLT_EMEC_energy",     "HLT EMEC Cell Energy;     Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("HLT_EMEC_energy_map", "HLT EMEC Cell < Energy >; #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));

  addHistogram(new TH1F       ("HLT_HEC__num",        "Number of HLT HEC Cells;  Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("HLT_HEC__num_map",    "Number of HLT HEC Cells;  #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("HLT_HEC__energy",     "HLT HEC Cell Energy;      Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("HLT_HEC__energy_map", "HLT HEC Cell < Energy >;  #eta;                #phi",     50,- 5.0,   5.0,64,-M_PI,M_PI));

  addHistogram(new TH1F       ("HLT_FCAL_num",        "Number of HLT FCAL Cells; Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("HLT_FCAL_num_map",    "Number of HLT FCAL Cells; #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("HLT_FCAL_energy",     "HLT FCAL Cell Energy;     Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("HLT_FCAL_energy_map", "HLT FCAL Cell < Energy >; #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));
  
  addHistogram(new TH1F       ("HLT_TILE_num",        "Number of HLT Tile Cells; Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));              
  addHistogram(new TH2F       ("HLT_TILE_num_map",    "Number of HLT Tile Cells; #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("HLT_TILE_energy",     "HLT Tile Cell Energy;     Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("HLT_TILE_energy_map", "HLT Tile Cell < Energy >; #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));
    
  // OFF cells
  addHistogram(new TH1F       ("OFF_LARG_num",        "Number of OFF LAr Cells;  Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("OFF_LARG_num_map",    "Number of OFF LAr Cells;  #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("OFF_LARG_energy",     "OFF LAr Cell Energy;      Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("OFF_LARG_energy_map", "OFF LAr Cell < Energy >;  #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));
  
  addHistogram(new TH1F       ("OFF_EMB__num",        "Number of OFF EMB Cells;  Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("OFF_EMB__num_map",    "Number of OFF EMB Cells;  #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("OFF_EMB__energy",     "OFF EMB Cell Energy;      Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("OFF_EMB__energy_map", "OFF EMB Cell < Energy >;  #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));

  addHistogram(new TH1F       ("OFF_EMEC_num",        "Number of OFF EMEC Cells; Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("OFF_EMEC_num_map",    "Number of OFF EMEC Cells; #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("OFF_EMEC_energy",     "OFF EMEC Cell Energy;     Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("OFF_EMEC_energy_map", "OFF EMEC Cell < Energy >; #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));

  addHistogram(new TH1F       ("OFF_HEC__num",        "Number of OFF HEC Cells;  Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("OFF_HEC__num_map",    "Number of OFF HEC Cells;  #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("OFF_HEC__energy",     "OFF HEC Cell Energy;      Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("OFF_HEC__energy_map", "OFF HEC Cell < Energy >;  #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));

  addHistogram(new TH1F       ("OFF_FCAL_num",        "Number of OFF FCAL Cells; Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("OFF_FCAL_num_map",    "Number of OFF FCAL Cells; #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("OFF_FCAL_energy",     "OFF FCAL Cell Energy;     Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("OFF_FCAL_energy_map", "OFF FCAL Cell < Energy >; #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));

  addHistogram(new TH1F       ("OFF_TILE_num",        "Number of OFF Tile Cells; Num Cells (x10^{3}); Entries", 201, -0.5, 200.5));
  addHistogram(new TH2F       ("OFF_TILE_num_map",    "Number of OFF Tile Cells; #eta;                #phi;",    50, -5.0,   5.0,64,-M_PI,M_PI));
  addHistogram(new TH1F       ("OFF_TILE_energy",     "OFF Tile Cell Energy;     Energy [GeV];        Entries",  50,  0.0,  50.0));
  addHistogram(new TProfile2D ("OFF_TILE_energy_map", "OFF Tile Cell < Energy >; #eta;                #phi",     50, -5.0,   5.0,64,-M_PI,M_PI));
  
  // HLT cells vs. OFF cells
  addHistogram(new TH1F       ("HLT_vs_OFF_LARG_delta_energy",     "HLT Cell vs OFF Cell (LAr);  #DeltaEnergy / Energy_{OFF};  Entries",             50, -10.0, 10.0));
  addHistogram(new TProfile2D ("HLT_vs_OFF_LARG_delta_energy_map", "HLT Cell vs OFF Cell (LAr):  < | #DeltaEnergy / Energy_{OFF} | >;  #eta; #phi",  50,  -5.0,  5.0, 64, -M_PI, M_PI));
  
  addHistogram(new TH1F       ("HLT_vs_OFF_EMB__delta_energy",     "HLT Cell vs OFF Cell (EMB);  #DeltaEnergy / Energy_{OFF};  Entries",             50, -10.0, 10.0));
  addHistogram(new TProfile2D ("HLT_vs_OFF_EMB__delta_energy_map", "HLT Cell vs OFF Cell (EMB):  < | #DeltaEnergy / Energy_{OFF} | >;  #eta; #phi",  50,  -5.0,  5.0, 64, -M_PI, M_PI));

  addHistogram(new TH1F       ("HLT_vs_OFF_EMEC_delta_energy",     "HLT Cell vs OFF Cell (EMEC); #DeltaEnergy / Energy_{OFF};  Entries",             50, -10.0, 10.0));
  addHistogram(new TProfile2D ("HLT_vs_OFF_EMEC_delta_energy_map", "HLT Cell vs OFF Cell (EMEC): < | #DeltaEnergy / Energy_{OFF} | >;  #eta; #phi",  50,  -5.0,  5.0, 64, -M_PI, M_PI));
  
  addHistogram(new TH1F       ("HLT_vs_OFF_HEC__delta_energy",     "HLT Cell vs OFF Cell (HEC);  #DeltaEnergy / Energy_{OFF};  Entries",             50, -10.0, 10.0));
  addHistogram(new TProfile2D ("HLT_vs_OFF_HEC__delta_energy_map", "HLT Cell vs OFF Cell (HEC):  < | #DeltaEnergy / Energy_{OFF} | >;  #eta; #phi",  50,  -5.0,  5.0, 64, -M_PI, M_PI));
  
  addHistogram(new TH1F       ("HLT_vs_OFF_FCAL_delta_energy",     "HLT Cell vs OFF Cell (FCAL); #DeltaEnergy / Energy_{OFF};  Entries",             50, -10.0, 10.0));
  addHistogram(new TProfile2D ("HLT_vs_OFF_FCAL_delta_energy_map", "HLT Cell vs OFF Cell (FCAL): < | #DeltaEnergy / Energy_{OFF} | >;  #eta; #phi",  50,  -5.0,  5.0, 64, -M_PI, M_PI));
  
  addHistogram(new TH1F       ("HLT_vs_OFF_TILE_delta_energy",     "HLT Cell vs OFF Cell (Tile); #DeltaEnergy / Energy_{OFF};  Entries",             50, -10.0, 10.0));
  addHistogram(new TProfile2D ("HLT_vs_OFF_TILE_delta_energy_map", "HLT Cell vs OFF Cell (Tile): < | #DeltaEnergy / Energy_{OFF} | >;  #eta; #phi",  50,  -5.0,  5.0, 64, -M_PI, M_PI));
  
  return StatusCode::SUCCESS;
}


StatusCode HLTCaloESD_CaloCells::fill() 
{
  ATH_MSG_DEBUG("fill()");
  
  // retrieve HLT cells
  const CaloCellContainer* HLT_cont = 0;
  if(evtStore()->retrieve(HLT_cont, m_HLT_cont_name).isFailure()) {
    ATH_MSG_INFO ("could not retrieve the CaloCellContainer: " << m_HLT_cont_name); 
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG("successfully retrieved CaloCellContainer: " << m_HLT_cont_name << " (size = " << HLT_cont->size() << ")");
  }

  // retrieve OFF cells
  const ::CaloCellContainer* OFF_cont = 0; 
  if(evtStore()->retrieve(OFF_cont, m_OFF_cont_name).isFailure()) {
    ATH_MSG_INFO ("could not retrieve the CaloCellContainer: " << m_OFF_cont_name);
    return StatusCode::SUCCESS;
  } 
  else {
    ATH_MSG_DEBUG("successfully retrieved CaloCellContainer: " << m_OFF_cont_name << " (size = " << OFF_cont->size() << ")");
  }  

  // initialise cell counters
  unsigned int HLT_LARG_num = 0;
  unsigned int HLT_EMB__num = 0;
  unsigned int HLT_EMEC_num = 0;
  unsigned int HLT_HEC__num = 0;
  unsigned int HLT_FCAL_num = 0;
  unsigned int HLT_TILE_num = 0;

  unsigned int OFF_LARG_num = 0;
  unsigned int OFF_EMB__num = 0;
  unsigned int OFF_EMEC_num = 0;
  unsigned int OFF_HEC__num = 0;
  unsigned int OFF_FCAL_num = 0;
  unsigned int OFF_TILE_num = 0;
  
  // loop over HLT clusters
  CaloCellContainer::const_iterator HLT_itr = HLT_cont->begin();
  CaloCellContainer::const_iterator HLT_end = HLT_cont->end();

  for(; HLT_itr != HLT_end; ++HLT_itr) {
    
    const CaloCell* HLT_cell = (*HLT_itr);
    if(!HLT_cell || !HLT_cell->caloDDE()) {
      continue;
    }
    
    IdentifierHash hash = HLT_cell->caloDDE()->calo_hash();

    double HLT_energy = HLT_cell->energy() / 1000;
    double HLT_eta    = HLT_cell->eta();
    double HLT_phi    = HLT_cell->phi();
    
    // CaloGain::CaloGain HLT_gain = HLT_cell->gain();
    
    if(HLT_cell->caloDDE()->is_tile()) {
      HLT_TILE_num++;
      hist2("HLT_TILE_num_map")   ->Fill(HLT_eta,HLT_phi);
      hist ("HLT_TILE_energy")    ->Fill(HLT_energy);
      hist2("HLT_TILE_energy_map")->Fill(HLT_eta,HLT_phi,HLT_energy);
    }
    else { 
      HLT_LARG_num++;
      hist2("HLT_LARG_num_map")   ->Fill(HLT_eta,HLT_phi);
      hist ("HLT_LARG_energy")    ->Fill(HLT_energy);
      hist2("HLT_LARG_energy_map")->Fill(HLT_eta,HLT_phi,HLT_energy);
      
      if(HLT_cell->caloDDE()->is_lar_em_barrel()) {
	HLT_EMB__num++;
	hist2("HLT_EMB__num_map")   ->Fill(HLT_eta,HLT_phi);
	hist ("HLT_EMB__energy")    ->Fill(HLT_energy);
	hist2("HLT_EMB__energy_map")->Fill(HLT_eta,HLT_phi,HLT_energy);
      }
      
      if(HLT_cell->caloDDE()->is_lar_em_endcap()) {
	HLT_EMEC_num++;
	hist2("HLT_EMEC_num_map")   ->Fill(HLT_eta,HLT_phi);
	hist ("HLT_EMEC_energy")    ->Fill(HLT_energy);
	hist2("HLT_EMEC_energy_map")->Fill(HLT_eta,HLT_phi,HLT_energy);
      }

      if(HLT_cell->caloDDE()->is_lar_hec() ) {
	HLT_HEC__num++;
	hist2("HLT_HEC__num_map")   ->Fill(HLT_eta,HLT_phi);
	hist ("HLT_HEC__energy")    ->Fill(HLT_energy);
	hist2("HLT_HEC__energy_map")->Fill(HLT_eta,HLT_phi,HLT_energy);
      }

      if(HLT_cell->caloDDE()->is_lar_fcal()) {
	HLT_FCAL_num++;
	hist2("HLT_FCAL_num_map")   ->Fill(HLT_eta,HLT_phi);
	hist ("HLT_FCAL_energy")    ->Fill(HLT_energy);
	hist2("HLT_FCAL_energy_map")->Fill(HLT_eta,HLT_phi,HLT_energy);
      }
    }
    
    const CaloCell* OFF_cell = OFF_cont->findCell(hash);
    if(!OFF_cell || !OFF_cell->caloDDE()) {
      continue;
    }
    
    double OFF_energy = OFF_cell->energy() / 1000;
    double OFF_eta    = OFF_cell->eta();
    double OFF_phi    = OFF_cell->phi();
    
    // CaloGain::CaloGain OFF_gain = OFF_cell->gain();
    
    if(OFF_cell->caloDDE()->is_tile()) {
      hist2("OFF_TILE_num_map")   ->Fill(OFF_eta,OFF_phi);
      hist ("OFF_TILE_energy")    ->Fill(OFF_energy);
      hist2("OFF_TILE_energy_map")->Fill(OFF_eta,OFF_phi,OFF_energy);
    }
    else {
      hist2("OFF_LARG_num_map")   ->Fill(OFF_eta,OFF_phi);
      hist ("OFF_LARG_energy")    ->Fill(OFF_energy);
      hist2("OFF_LARG_energy_map")->Fill(OFF_eta,OFF_phi,OFF_energy);      
      
      if(OFF_cell->caloDDE()->is_lar_em_barrel()) {
        OFF_EMB__num++;
        hist2("OFF_EMB__num_map")   ->Fill(OFF_eta,OFF_phi);
        hist ("OFF_EMB__energy")    ->Fill(OFF_energy);
        hist2("OFF_EMB__energy_map")->Fill(OFF_eta,OFF_phi,OFF_energy);
      }
      
      if(OFF_cell->caloDDE()->is_lar_em_endcap()) {
        OFF_EMEC_num++;
        hist2("OFF_EMEC_num_map")   ->Fill(OFF_eta,OFF_phi);
        hist ("OFF_EMEC_energy")    ->Fill(OFF_energy);
        hist2("OFF_EMEC_energy_map")->Fill(OFF_eta,OFF_phi,OFF_energy);
      }
      
      if(OFF_cell->caloDDE()->is_lar_hec() ) {
        OFF_HEC__num++;
        hist2("OFF_HEC__num_map")   ->Fill(OFF_eta,OFF_phi);
        hist ("OFF_HEC__energy")    ->Fill(OFF_energy);
        hist2("OFF_HEC__energy_map")->Fill(OFF_eta,OFF_phi,OFF_energy);
      }
      
      if(OFF_cell->caloDDE()->is_lar_fcal()) {
        OFF_FCAL_num++;
        hist2("OFF_FCAL_num_map")   ->Fill(OFF_eta,OFF_phi);
        hist ("OFF_FCAL_energy")    ->Fill(OFF_energy);
        hist2("OFF_FCAL_energy_map")->Fill(OFF_eta,OFF_phi,OFF_energy);
      }
    }
    
    // if(OFF_energy < m_OFF_min_et) continue;
    if(OFF_energy < 0.000000001) continue; // QUICK FIX

    double delta_energy = ((OFF_energy - HLT_energy) / OFF_energy) * 100;
    
    if(OFF_cell->caloDDE()->is_tile()) {
      hist ("HLT_vs_OFF_TILE_delta_energy")    ->Fill(delta_energy);
      hist2("HLT_vs_OFF_TILE_delta_energy_map")->Fill(OFF_eta,OFF_phi,delta_energy);
    }
    else  {
      hist ("HLT_vs_OFF_LARG_delta_energy")    ->Fill(delta_energy);
      hist2("HLT_vs_OFF_LARG_delta_energy_map")->Fill(OFF_eta,OFF_phi,delta_energy);
      
      if(OFF_cell->caloDDE()->is_lar_em_barrel()) {
	hist ("HLT_vs_OFF_EMB__delta_energy")    ->Fill(delta_energy);
        hist2("HLT_vs_OFF_EMB__delta_energy_map")->Fill(OFF_eta,OFF_phi,delta_energy);
      }
      
      if(OFF_cell->caloDDE()->is_lar_em_endcap()) {
	hist ("HLT_vs_OFF_EMEC_delta_energy")    ->Fill(delta_energy);
        hist2("HLT_vs_OFF_EMEC_delta_energy_map")->Fill(OFF_eta,OFF_phi,delta_energy);
      }
      
      if(OFF_cell->caloDDE()->is_lar_hec() ) {
	hist ("HLT_vs_OFF_HEC__delta_energy")    ->Fill(delta_energy);
        hist2("HLT_vs_OFF_HEC__delta_energy_map")->Fill(OFF_eta,OFF_phi,delta_energy);
      }
      
      if(OFF_cell->caloDDE()->is_lar_fcal()) {
	hist ("HLT_vs_OFF_FCAL_delta_energy")    ->Fill(delta_energy);
        hist2("HLT_vs_OFF_FCAL_delta_energy_map")->Fill(OFF_eta,OFF_phi,delta_energy);
      }
    }
  
  } // end loop over HLT cells
  
  hist("HLT_LARG_num")->Fill(HLT_LARG_num / 1000);
  hist("HLT_EMB__num")->Fill(HLT_EMB__num / 1000);
  hist("HLT_EMEC_num")->Fill(HLT_EMEC_num / 1000);
  hist("HLT_HEC__num")->Fill(HLT_HEC__num / 1000);
  hist("HLT_FCAL_num")->Fill(HLT_FCAL_num / 1000);
  hist("HLT_TILE_num")->Fill(HLT_TILE_num / 1000);

  hist("OFF_LARG_num")->Fill(OFF_LARG_num / 1000);
  hist("OFF_EMB__num")->Fill(OFF_EMB__num / 1000);
  hist("OFF_EMEC_num")->Fill(OFF_EMEC_num / 1000);
  hist("OFF_HEC__num")->Fill(OFF_HEC__num / 1000);
  hist("OFF_FCAL_num")->Fill(OFF_FCAL_num / 1000);
  hist("OFF_TILE_num")->Fill(OFF_TILE_num / 1000);  
  
  return StatusCode::SUCCESS;
}


StatusCode HLTCaloESD_CaloCells::proc() 
{
  return StatusCode::SUCCESS; 
}

