/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTowerMonTool.cxx
// PACKAGE:  TileMonitoring  
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch) 
//           
//
// ********************************************************************

#include "TileTowerMonTool.h"

#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerContainer.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"


/*---------------------------------------------------------*/
TileTowerMonTool::TileTowerMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_TileTowerTrig(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("energyThreshold"     , m_Threshold = 50.); //Threshold in MeV
  declareProperty("towersContainerName" , m_towersContName = "TileTower"); //SG Tower Container

  m_path = "/Tile/Tower"; //ROOT File directory
}

/*---------------------------------------------------------*/
TileTowerMonTool::~TileTowerMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileTowerMonTool:: initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in initialize()" );

  return TileFatherMonTool::initialize();
}


/*---------------------------------------------------------*/
StatusCode TileTowerMonTool::bookHistTrigPart( int trig , int part ) {
/*---------------------------------------------------------*/


    int element =  m_activeTrigs[trig]; // taking the vector element from the trigger mapping array

    std::string runNumStr = getRunNumStr();

    //m_TilenTowers[part].push_back( book1F(m_TrigNames[trig]+"/"+m_PartNames[part],"nTowers","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal "+m_PartNames[part]+" Tower number",150,0., 1500.) );
    //m_TilenTowers[part][ element ]->GetXaxis()->SetTitle("Number of Towers");

    //m_TileTowernCells[part].push_back( book1F(m_TrigNames[trig]+"/"+m_PartNames[part],"TowersnCells","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal "+m_PartNames[part]+" number of cells in most en. Tower",5,0., 5.) );
    //m_TileTowernCells[part][ element ]->GetXaxis()->SetTitle("Number of cells for each tower");

    //m_TileTowerEnergy[part].push_back( book1F(m_TrigNames[trig]+"/"+m_PartNames[part],"TowerEnergy","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal "+m_PartNames[part]+" Energy in most en. Tower (MeV)",80,0., 20000.) );
    //m_TileTowerEnergy[part][ element ]->GetXaxis()->SetTitle("most en. Tower Energy (MeV)");

    //m_TileTowerEnergyRebin[part].push_back( book1F(m_TrigNames[trig]+"/"+m_PartNames[part],"TowerEnergyRebin","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal "+m_PartNames[part]+" Energy in most en. Tower (MeV). Full Dynamic Range",40,0., 20000.) );
    //m_TileTowerEnergyRebin[part][ element ]->GetXaxis()->SetTitle("most en. Tower Energy (MeV)");
    //m_TileTowerEnergyRebin[part][ element ]->SetBit(TH1::kCanRebin);

    m_TileTowerEt[part].push_back( book1F(m_TrigNames[trig]+"/"+m_PartNames[part],"TowerEt" + m_PartNames[part] + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal "+m_PartNames[part]+" Et in most en. Tower (MeV)",80,0., 20000.) );
    m_TileTowerEt[part][ element ]->GetXaxis()->SetTitle("most en. Tower Et (MeV)");

    m_TileAllTowerEnergy[part].push_back( book1F(m_TrigNames[trig]+"/"+m_PartNames[part],"AllTowerEnergy" + m_PartNames[part] + m_TrigNames[trig],"Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal "+m_PartNames[part]+" All Towers Energy (MeV)",80,0., 20000.) );
    m_TileAllTowerEnergy[part][ element ]->GetXaxis()->SetTitle("All Towers Energy (MeV)");

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTowerMonTool::bookHistTrig( int trig ) {
/*---------------------------------------------------------*/

  // Taking care of the mapping between vector element and trigger type: if vector empty, element will be 0 and so on
  //m_activeTrigs[ trig ] = m_TileTowerEtaPhi.size();
  m_activeTrigs[trig] = m_TileTowerTrig;
  int element = m_activeTrigs[trig];

  std::string runNumStr = getRunNumStr();

  m_TileTowerTrig++; //Iterates size of m_TileTowerTrig for trigger type

  m_TileTowerEtaPhi.push_back(book2F( m_TrigNames[trig], "TowerEtaPhi" + m_TrigNames[trig]
                              , "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal Position of most en. Tower"
                              , 40, -2.0, 2.0, 64, -3.15, 3.15));
  m_TileTowerEtaPhi[element]->GetXaxis()->SetTitle("#eta");
  m_TileTowerEtaPhi[element]->GetYaxis()->SetTitle("#phi");

  //m_TileTowerEta.push_back( book1F(m_TrigNames[trig],"TowerEta","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal #eta position of most en. Tower",40,-2.0, 2.0) );
  //m_TileTowerEta[element]->GetXaxis()->SetTitle("#eta");

  //m_TileTowerPhi.push_back( book1F(m_TrigNames[trig],"TowerPhi","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal #phi position of most en. Tower",64,-3.15, 3.15) );
  //m_TileTowerPhi[element]->GetXaxis()->SetTitle("#phi");

  m_TileTowerEtaPhiDiff.push_back(book2F( m_TrigNames[trig], "TowerEtaPhiDiff" + m_TrigNames[trig]
                                  , "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal pos. correlation of Tow. opposite to most en. Towers"
                                  , 40, -2.0, 2.0, 64, 0., 6.4));
  m_TileTowerEtaPhiDiff[element]->GetXaxis()->SetTitle("#Delta #eta");
  m_TileTowerEtaPhiDiff[element]->GetYaxis()->SetTitle("#Delta #phi");

  //m_TileTowerEneDiff.push_back( book1F(m_TrigNames[trig],"TowerEneDiff","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal en. correlation of Tow. opposite to most en. Towers",200,-5000., 5000.) );
  //m_TileTowerEneDiff[element]->GetXaxis()->SetTitle("Energy diff. (MeV)");

  //m_TileTowerEneTim.push_back( bookProfile(m_TrigNames[trig],"TowerEneTim","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Energy of most en. Tower versus time",25,0., 100.,-2000.,200000.) );
  //m_TileTowerEneTim[element]->GetXaxis()->SetTitle("event number");
  //m_TileTowerEneTim[element]->GetYaxis()->SetTitle("<E> of the most en. Tower (MeV)");
  //m_TileTowerEneTim[ element ]->SetBit(TH1::kCanRebin);

  //m_TileTowerEneLumi.push_back( bookProfile(m_TrigNames[trig],"TowerEneLumi","Run "+runNumStr+" Trigger "+m_TrigNames[trig]+": TileCal Energy of most en. Tower versus LumiBlock",10,0., 10.,-2000.,200000., run, ATTRIB_MANAGED, "", "mergeRebinned") );
  //m_TileTowerEneLumi[element]->GetXaxis()->SetTitle("LumiBlock");
  //m_TileTowerEneLumi[element]->GetYaxis()->SetTitle("Average Energy of the most Energetic Tower (MeV)");
  //m_TileTowerEneLumi[ element ]->SetBit(TH1::kCanRebin);

  for (int p = PartEBA; p < NPartHisto; p++) {
    if (bookHistTrigPart(trig, p).isFailure()) {
        ATH_MSG_WARNING( "Error booking Tower histograms for trigger " << m_TrigNames[trig]
                         << ", partition " << m_PartNames[p] );
    }
  }

  //m_test = book2D("test","Test Time (ns)",150,-150.,150.,120,-20.,100.);

  return StatusCode::SUCCESS;
}

/// BookHistogram method is called at every event block ,lumi block and run.
/// At the moment we need to book only every run
/// It calls bookHistTrig for the 'AnyTrig' trigger type
/// All concrete trigger type histograms will only be booked when needed
/*---------------------------------------------------------*/
StatusCode TileTowerMonTool::bookHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO("in bookHistograms()");

  cleanHistVec(); //necessary to avoid problems at the eb, lumi blocks boundaries
  /*
  // triggers histograms will be booked when needed
  if (bookHistTrig(AnyTrig).isFailure()) {
    ATH_MSG_WARNING( "Error booking Tower histograms for Trigger " << m_TrigNames[AnyTrig] );
  }
   */

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
void TileTowerMonTool::cleanHistVec() {

/*---------------------------------------------------------*/

  for (int i = 0; i < 9; i++) {
    m_activeTrigs[i] = -1;
  }

  m_TileTowerTrig = 0;
  m_TileTowerEtaPhi.clear();
  //m_TileTowerEta.clear() ;
  //m_TileTowerPhi.clear() ;
  m_TileTowerEtaPhiDiff.clear();
  //m_TileTowerEneDiff.clear() ;
  //m_TileTowerEneTim.clear();
  //m_TileTowerEneLumi.clear();

  for (int part = PartEBA; part < NPartHisto; part++) {
    m_TileAllTowerEnergy[part].clear();
    //m_TilenTowers[part].clear() ;
    //m_TileTowernCells[part].clear() ;
    //m_TileTowerEnergy[part].clear() ;
    //m_TileTowerEnergyRebin[part].clear() ;
    m_TileTowerEt[part].clear();
  }

}

/*---------------------------------------------------------*/
StatusCode TileTowerMonTool::fillHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in fillHistograms()" );

  fillEvtInfo();
  // get Lvl1 Trigger word
  uint32_t lvl1info = getL1info();
  //uint32_t evtNum = getEvtNum();
  //uint32_t lumi = getLumiBlock();
  get_eventTrigs(lvl1info); //fill  m_eventTrigs; it always contains at least one element: AnyTrig.

  //check if trigger type is new and therefore some histograms must be booked
  for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
    if (m_activeTrigs[m_eventTrigs[i]] < 0) {
      // book histograms
      if (bookHistTrig(m_eventTrigs[i]).isFailure()) {
        ATH_MSG_WARNING( "Error booking Tower histograms for Trigger " << m_TrigNames[m_eventTrigs[i]] );
      }
    }
  }

  // Pointer to a Tile cell container
  const CaloTowerContainer* tower_container;

  //Retrieve Tower collection from SG 
  CHECK( evtStore()->retrieve(tower_container, m_towersContName) );
  ATH_MSG_VERBOSE( "TileTowerMonTool: Retrieval of Tile towers from container " << m_towersContName << " succeeded" );

  //int    ncells_most =       0 ;
  int partition = NumPart;
  double energy_most = 0.;
  double et_most = 0.;
  double eta_most = 0.;
  double phi_most = 0.;
  bool set_most = false;
  double energy_corr = 0.;
  double eta_corr = -10.;
  double phi_corr = 0.;
  bool set_corr = false;

  int nTowers[NPartHisto] = { 0, 0, 0, 0, 0 };

  CaloTowerContainer::const_iterator iTower = tower_container->begin();
  CaloTowerContainer::const_iterator lastTower = tower_container->end();
  for (; iTower != lastTower; ++iTower) {

    if ((*iTower)->energy() > energy_most) {
      const CaloTower* tower_ptr = *iTower;
      energy_most = tower_ptr->energy();
      et_most = tower_ptr->et();
      eta_most = tower_ptr->eta();
      phi_most = tower_ptr->phi();
      //ncells_most = tower_ptr->getNumberOfCells();  
      partition = getPartition(*(tower_ptr->cell_begin()));
      set_most = true;
    }

  }
  
  if (set_most) {

    for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {

      /// Fill histograms of every l1 trigger active for this event.
      /// AnyTrig will be always filled
      ///'i' is a trigger type that this event has triggered:  : TileCal, RPC,...
      /// AnyTrig is always triggered, by default.
      /// vecIndx() function does the mapping between the triggers in the m_eventTrigs vector and the 
      /// corresponding histogram vector element, it returns  m_activeTrigs[ m_eventTrigs[i] ].
      /// vecIndx(i) points to the vector element corresponding to the i-th trigger type in   m_eventTrigs
      /// For example i=0 =>  m_eventTrigs[i]= AnyTrig =>  m_activeTrigs[AnyTrig]=0, because
      /// by construction  m_eventTrigs[0]= AnyTrig and
      /// m_activeTrigs[AnyTrig] is equal to 0.
      /// for i!=0, the function result will depend, event by event, on the triggers stored in  m_eventTrigs 

      //m_TileTowerEta[ vecIndx(i) ]->Fill(eta_most,1.);
      //m_TileTowerPhi[ vecIndx(i) ]->Fill(phi_most,1.);
      m_TileTowerEtaPhi[vecIndx(i)]->Fill(eta_most, phi_most, 1.);

    }

    if (partition < NumPart) {

      //Fill Histos by partition
      // Please note: the trigger is the second element: hist_vec[part][trig]
      for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {

        //m_TileTowerEnergy[partition][ vecIndx(i) ]->Fill(energy_most,1.);
        //m_TileTowerEnergyRebin[partition][ vecIndx(i) ]->Fill(energy_most,1.);
        m_TileTowerEt[partition][vecIndx(i)]->Fill(et_most, 1.);
        //m_TileTowernCells[partition][ vecIndx(i) ]->Fill(ncells_most,1.);

        //Fill AllPartition Histos
        //m_TileTowerEnergy[NumPart][ vecIndx(i) ]->Fill(energy_most,1.);
        //m_TileTowerEnergyRebin[NumPart][ vecIndx(i) ]->Fill(energy_most,1.);
        m_TileTowerEt[NumPart][vecIndx(i)]->Fill(et_most, 1.);
        //m_TileTowernCells[NumPart][ vecIndx(i) ]->Fill(ncells_most,1.);

        //m_TileTowerEneTim[ vecIndx(i) ]->Fill(evtNum, energy_most);
        //m_TileTowerEneLumi[ vecIndx(i) ]->Fill(lumi, energy_most);
      }
    }
  }

  iTower = tower_container->begin();
  for (; iTower != lastTower; ++iTower) {

    const CaloTower* tower_ptr = *iTower;    // pointer to tower object
    double energy = tower_ptr->energy();
    double phi = tower_ptr->phi();
    double eta = tower_ptr->eta();

    if (tower_ptr->getNumberOfCells() > 0) {
      partition = getPartition(*(tower_ptr->cell_begin()));
    } else {
      partition = NumPart;
    }

    if (energy > 0.) {
      if ((fabs(eta) > 5.) || (fabs(phi) > 3.2)) {
        ATH_MSG_WARNING( "Tower information out of Eta - Phi boundaries, skipping this tower:" );
        ATH_MSG_WARNING( "Energy (MeV)=" << energy << "\tEta=" << eta << "\tPhi=" << phi );
        continue;
      }

      if (partition < NumPart) {
        nTowers[partition]++;
        nTowers[NumPart]++;

        for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {

          m_TileAllTowerEnergy[partition][vecIndx(i)]->Fill(energy, 1.);
          m_TileAllTowerEnergy[NumPart][vecIndx(i)]->Fill(energy, 1.);
        }
      }

      if ((phi_most * phi < 0.) && (energy > energy_corr)) {
        energy_corr = energy;
        eta_corr = eta;
        phi_corr = phi;
        set_corr = true;
      }

      if (msgLvl(MSG::VERBOSE)) {

        msg(MSG::VERBOSE) << "Tower nCells= " << tower_ptr->getNumberOfCells() << endmsg;
        msg(MSG::VERBOSE) << "Tower Energy= " << energy << "\tEt()= " << tower_ptr->et() << endmsg;
        msg(MSG::VERBOSE) << "Tower Eta= " << eta << "\tPhi= " << phi << endmsg;
      }

      //m_test->Fill(time,energy,1.);
    }
  }

  ATH_MSG_DEBUG( "Total Number of towers in the event: " << nTowers[NumPart] );

  //for (int p= PartEBA; p<NPartHisto; p++) {
  //for (unsigned int i=0; i< m_eventTrigs.size(); i++ ) {

  //m_TilenTowers[p][ vecIndx(i) ]->Fill(nTowers[p],1.);
  //}
  //}

  if (set_corr) {

    //double e_diff=0.;
    //if (phi_most>0.) {e_diff = energy_most-energy_corr;} else {e_diff = energy_corr-energy_most;}
    for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {

      //m_TileTowerEneDiff[ vecIndx(i) ]->Fill(e_diff,1.);
      m_TileTowerEtaPhiDiff[vecIndx(i)]->Fill(fabs(eta_corr) - fabs(eta_most), fabs(phi_corr - phi_most), 1.);
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTowerMonTool::procHistograms() {
/*---------------------------------------------------------*/

  if( endOfLumiBlockFlag() ||  endOfRunFlag() ) {
    ATH_MSG_INFO( "in procHistograms()" );
  }
  
    return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTowerMonTool::checkHists(bool /* fromFinalize */) {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in checkHists()"  );
  return StatusCode::SUCCESS;
}

