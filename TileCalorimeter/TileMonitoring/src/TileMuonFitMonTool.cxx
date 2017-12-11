/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileMuonFitMonTool.cxx
// PACKAGE:  TileMonitoring  
//
// AUTHORS:   Luca Fiorini (Luca.Fiorini@cern.ch) 
//            Francesc Vives (fvives@ifae.es)
//
// ********************************************************************

#include "TileMonitoring/TileMuonFitMonTool.h"

#include "CaloEvent/CaloCellContainer.h"

#include "TileEvent/TileCosmicMuon.h"
#include "TileEvent/TileContainer.h"

#include "xAODTrigger/TrigDecision.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TH1F.h"
#include "TH2D.h"
#include "TProfile.h"

#include <cmath>


/*---------------------------------------------------------*/
TileMuonFitMonTool::TileMuonFitMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_isFirstEv(true)
  , m_trigok(false)
  , m_tileMuonFitTrig(0U)
  , m_tileMuonFitTimePartLVL1{}
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("muonfitContainerName", m_muonfitContName = "TileCosmicMuonHT"); //SG MuonFit Container
  declareProperty("muonfitCellContainerName", m_cellContainerKey = "AllCalo");
  declareProperty("UseLVL1", m_useLVL1 = true);

  m_path = "/Tile/Muonfit"; //ROOT File directory
}

/*---------------------------------------------------------*/
TileMuonFitMonTool::~TileMuonFitMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileMuonFitMonTool:: initialize() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in initialize()" );

  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileMuonFitMonTool::bookHistTrig( int trig ) {
/*---------------------------------------------------------*/

  std::string runNumStr = getRunNumStr();

  m_activeTrigs[trig] = m_tileMuonFitTrig;
  int element = m_activeTrigs[trig];

  m_tileMuonFitTrig++; //Increment Trigger index

  m_tileMuonFit.push_back(book1F(m_TrigNames[trig], "tileMuonFit" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": Tile MuonFit reconstructed", 10, -0.5, 9.5));

  m_tileMuonFitPos.push_back( book2F(m_TrigNames[trig], "tileMuonFitPos" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal Muon Fit position at y=0", 70, -7000., 7000., 45, -4500., 4500.));
  m_tileMuonFitPos[element]->GetXaxis()->SetTitle("z position (mm)");
  m_tileMuonFitPos[element]->GetYaxis()->SetTitle("x position (mm)");

  m_tileMuonFitDir.push_back(book1F(m_TrigNames[trig], "tileMuonFitDir" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit sinus of the angle in the y-z plane", 50, 0., 1.));
  m_tileMuonFitDir[element]->GetXaxis()->SetTitle("sin(#theta)");

  m_tileMuonFitDir2.push_back(
      book1F(m_TrigNames[trig], "tileMuonFitDir2" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit sinus of the angle in the x-y plane", 100, -1., 1.));
  m_tileMuonFitDir2[element]->GetXaxis()->SetTitle("sin(#phi)");

  m_tileMuonFitTime.push_back(
      book1F(m_TrigNames[trig], "tileMuonTime" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit-time at y=0 plane", 50, -75., 75.));
  m_tileMuonFitTime[element]->GetXaxis()->SetTitle("Muon time (ns)");

  m_tileMuonFitDirPos.push_back(book2F(m_TrigNames[trig], "tileMuonFitDirPos" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal Muon Fit vertical axis component wrt z coordinate at y=0", 50, -5000., 5000., 50, -1., 1.));
  m_tileMuonFitDirPos[element]->GetXaxis()->SetTitle("z position (mm)");
  m_tileMuonFitDirPos[element]->GetYaxis()->SetTitle("vertical direction component");

  m_tileMuonFitEnergy.push_back(book1F(m_TrigNames[trig], "tileMuonEnergy" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit Total Energy ", 50, 0., 10000.));
  m_tileMuonFitEnergy[element]->GetXaxis()->SetTitle("Muon energy (MeV)");

  m_tileMuonFitPath.push_back(book1F(m_TrigNames[trig], "tileMuonPath" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit Total Path length ", 50, 0., 10000.));
  m_tileMuonFitPath[element]->GetXaxis()->SetTitle("Muon path length (mm)");
  //    m_tileMuonFitPath[element]->SetBit(TH1::kCanRebin);

  m_tileMuonFitEnergyDensity.push_back(book1F(m_TrigNames[trig], "tileMuonEnergyDensity" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit Energy Density", 50, 0., 10.));
  m_tileMuonFitEnergyDensity[element]->GetXaxis()->SetTitle("Muon Energy Loss per mm (MeV/mm)");
  //    m_tileMuonFitEnergyDensity[element]->SetBit(TH1::kCanRebin);

  m_tileMuonFitNCells.push_back( book1F(m_TrigNames[trig], "tileMuonNCells" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit Number of Cells", 10, 0., 10., run, ATTRIB_MANAGED, "", "mergeRebinned"));
  m_tileMuonFitNCells[element]->GetXaxis()->SetTitle("number of cells");
  //m_tileMuonFitNCells[element]->SetBit(TH1::kCanRebin);

  m_tileMuonFitTimePos.push_back(bookProfile(m_TrigNames[trig], "tileMuonTimePos" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit Average Time VS z position when crossing y=0 plane", 80, -8000., 8000., -200., 200.));
  m_tileMuonFitTimePos[element]->GetXaxis()->SetTitle("Muon Fit z position at y=0 plane (mm)");
  m_tileMuonFitTimePos[element]->GetYaxis()->SetTitle("Muon Fit time at y=0 plane (ns)");

  m_tileMuonFitTimePart.push_back(bookProfile(m_TrigNames[trig], "tileMuonTimePart" + m_TrigNames[trig], "Run " + runNumStr + " Trigger " + m_TrigNames[trig] + ": TileCal MuonFit Average Time VS Partition when crossing y=0 plane", 4, -0.5, 3.5, -200., 200.));
  m_tileMuonFitTimePart[element]->GetXaxis()->SetTitle("Muon Fit Partition crossed");
  m_tileMuonFitTimePart[element]->GetYaxis()->SetTitle("Muon Fit time at y=0 plane (ns)");

  // set the x-axis label to partition name for occupancy and balance plot
  SetBinLabel(m_tileMuonFitTimePart[element]->GetXaxis(), m_PartNames, 4);

  return StatusCode::SUCCESS;
}

/// BookHistogram method is called at every event block ,lumi block and run.
/// At the moment we need to book only every run
/// It calls bookHistTrig for the 'AnyTrig' trigger type
/// All concrete trigger type histograms will only be booked when needed
/*---------------------------------------------------------*/
StatusCode TileMuonFitMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in bookHistograms()" );

  cleanHistVec(); //necessary to avoid problems at the eb, lumi blocks boundaries
  m_isFirstEv = true;
  m_trigok = false;
  m_l1triggers.clear();

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileMuonFitMonTool::cleanHistVec() {
/*---------------------------------------------------------*/


  for (int i = 0; i < 9; i++) {
    m_activeTrigs[i] = -1;
  }

  m_tileMuonFitTrig = 0; //reset index

  m_tileMuonFit.clear();
  m_tileMuonFitPos.clear();
  m_tileMuonFitDir.clear();
  m_tileMuonFitDir2.clear();
  m_tileMuonFitTime.clear();
  m_tileMuonFitDirPos.clear();
  m_tileMuonFitEnergy.clear();
  //m_tileMuonFitEnergyRebin.clear();
  m_tileMuonFitPath.clear();
  m_tileMuonFitNCells.clear();
  m_tileMuonFitEnergyDensity.clear();
  m_tileMuonFitTimePos.clear();
  m_tileMuonFitTimePart.clear();

}

/*---------------------------------------------------------*/
StatusCode TileMuonFitMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in fillHistograms()" );

  fillEvtInfo();
  // get Lvl1 Trigger word
  uint32_t lvl1info = getL1info();
  get_eventTrigs(lvl1info); //fill  m_eventTrigs; it always contains at least one element: AnyPhysTrig.

  //check if trigger type is new and therefore some histograms must be booked
  for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
    if (vecIndx(i) < 0) {
      // book histograms
      if (bookHistTrig(m_eventTrigs[i]).isFailure()) {
        ATH_MSG_WARNING( "Error booking MuonFit histograms for Trigger " << m_TrigNames[m_eventTrigs[i]] );
      }
    }
  }

  if (m_isFirstEv) {
    std::string runNumStr = getRunNumStr();


    if (m_useLVL1) {
      m_trigDec.retrieve().ignore();
      const Trig::ChainGroup* allL1 = m_trigDec->getChainGroup("L1_.*");
      m_l1triggers = allL1->getListOfTriggers();

      /*
      msg(MSG::INFO) << "LUCA lvl1 item names: [";
      for (unsigned int i = 0; i < m_l1triggers.size(); i++) {
        msg(MSG::INFO) << i << " " << m_l1triggers.at(i) << " , " << endmsg;
      }
      msg(MSG::INFO) << "] " << endmsg;
      */

      if (m_l1triggers.size() > 0) {
        m_trigok = true;
      }

    
      for (int i = PartEBA; i < NumPart; i++) {
        // set the x-axis label to partition name for occupancy and balance plot

        if (m_trigok) {
          m_tileMuonFitTimePartLVL1[i] = book2F("", "tileMuonTimePartLVL1" + m_PartNames[i], "Run " + runNumStr + ": " + m_PartNames[i] + " TileCal MuonFit Average Time VS LVL1 item when crossing y=0 plane", 31, -77.5, 77.5, m_l1triggers.size(), -0.5, m_l1triggers.size() - 0.5);
          SetBinLabel(m_tileMuonFitTimePartLVL1[i]->GetYaxis(), m_l1triggers);
        } else {
          m_tileMuonFitTimePartLVL1[i] = book2F("", "tileMuonTimePartLVL1" + m_PartNames[i], "Run " + runNumStr + ": " + m_PartNames[i] + " TileCal MuonFit Average Time VS LVL1 item when crossing y=0 plane", 31, -77.5, 77.5, 256, -0.5, 255.5);
          m_tileMuonFitTimePartLVL1[i]->GetYaxis()->SetTitle("LVL1 item");
        }
        m_tileMuonFitTimePartLVL1[i]->GetXaxis()->SetTitle("Time at y=0 (ns)");
      }
    }

    m_isFirstEv = false;
  }


  //LUCA Uncomment if you want to see how the trigger awareness works
  /*
  ATH_MSG_WARNING( "LUCA: size of  m_eventTrigs: " << m_eventTrigs.size() );
  for (int i = 0; i < m_eventTrigs.size(); i++) {
    ATH_MSG_WARNING( "LUCA: vecIndx(" << i << ")= " << vecIndx(i) );
    ATH_MSG_WARNING( "LUCA: m_eventTrigs(" << i << ")= " << m_eventTrigs[i] );
  }

  for (int i = Trig_b0; i < NTrigHisto; i++) {
    ATH_MSG_WARNING( "LUCA: m_activeTrigs(" << m_TrigNames[i] << ")= " << m_activeTrigs[i] );
  }

  */


  // Pointer to a Tile MuonFit container
  const TileCosmicMuonContainer* theTileCosmicMuonCnt;


  //Retrieve MuonFit collection from SG
  if(evtStore()->retrieve(theTileCosmicMuonCnt,m_muonfitContName).isFailure()) {
    ATH_MSG_WARNING( "Can't find TileCosmicMuonContainer with name " << m_muonfitContName << " in TES" );

    for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
      m_tileMuonFit[vecIndx(i)]->Fill(0., 1.);
    }

    return StatusCode::SUCCESS;
  }  

  ATH_MSG_VERBOSE( "TileMuonFitMonTool: Retrieval of Tile Muon Fit from container " << m_muonfitContName << " succeeded" );

  TileCosmicMuonContainer::const_iterator  it=theTileCosmicMuonCnt->begin();
  TileCosmicMuonContainer::const_iterator itend=theTileCosmicMuonCnt->end();
  const TileCosmicMuon*  theTileCosmicMuon;
  int nmu=0;

  std::vector<unsigned int> trigid;
  trigid.clear();
  if (m_useLVL1 && !m_trigok) {
    const DataHandle<xAOD::TrigDecision> trigDecision;
    if (evtStore()->retrieve(trigDecision).isSuccess()) { //snippet from TRT monitoring, thanks!
      std::vector<unsigned int> lvl1TAV = trigDecision->tav();
      for (unsigned int j = 0; j < 8 && j < lvl1TAV.size(); ++j) {
        for (unsigned int i = 0; i < 32; ++i) {
          if ((lvl1TAV[j] >> i) & 0x1) trigid.push_back(i + j * 32); // Found the ID
        } // Loop over bits in the last word, TAV
      } // Loop TAV
    }
  }

/*
  msg(MSG::INFO) << "LUCA lvl1 item: [";
  for (unsigned int i = 0; i < trigid.size(); i++) {
    msg(MSG::INFO) << " ,  " << trigid.at(i);
  }
  msg(MSG::INFO) << "] " << endmsg;
*/

  for (; it != itend; ++it) {
    theTileCosmicMuon = (*it);

    float mu_X = theTileCosmicMuon->GetPositionX();
    float mu_Z = theTileCosmicMuon->GetPositionZ();
    float mu_phi = theTileCosmicMuon->GetDirectionPhi();
    float mu_theta = theTileCosmicMuon->GetDirectionTheta();
    float mu_time = theTileCosmicMuon->GetTime();
    //float mu_fitQuality    = theTileCosmicMuon->GetFitQuality();
    //float mu_fitNCells     = theTileCosmicMuon->GetFitNCells();
    float mu_trackNCells = theTileCosmicMuon->GetTrackNCells();
    float mu_fullPath = theTileCosmicMuon->GetFullPath();
    //  float mu_fullPathBottom= theTileCosmicMuon->GetFullPathBottom();
    //  float mu_fullPathTop   = theTileCosmicMuon->GetFullPathTop();
    float mu_fullEnergy = theTileCosmicMuon->GetFullEnergy();
  

    if (mu_trackNCells > 2) { //we want the number of cells in the track to be more than 2
      nmu++;

      if ((mu_fullPath > 0) && (mu_fullPath < 10000.) && (mu_fullEnergy / mu_fullPath > 1.05)
          && (mu_fullEnergy / mu_fullPath < 7.5)) {
        int part = getTMFpart(theTileCosmicMuon); // get the partition crossed by the TMF

        if (m_useLVL1 && part > -1 && part < NumPart) {
          if (m_trigok) { // in case the TDT is working, we fill the histogram looping on the labels
            std::vector<std::string>::const_iterator it = m_l1triggers.begin();
            int l1 = 0;
            for (; it != m_l1triggers.end(); it++) {
              if (m_trigDec->isPassed(*it)) m_tileMuonFitTimePartLVL1[part]->Fill(mu_time, l1);
              l1++;
            }
          } else { // we have to rely on the lvl1 bits only
            for (unsigned int j = 0; j < trigid.size(); j++) {
              m_tileMuonFitTimePartLVL1[part]->Fill(mu_time, trigid.at(j));
            }
          }
        }

        for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
          m_tileMuonFitTimePos[vecIndx(i)]->Fill(mu_Z, mu_time);
          if (part > -1) {

            m_tileMuonFitTimePart[vecIndx(i)]->Fill(part, mu_time);
          }
        }
      }
      const float inv_mu_fullPath = (mu_fullPath > 0 ? 1. / mu_fullPath : 1);

      for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {

        m_tileMuonFitPos[vecIndx(i)]->Fill(mu_Z, mu_X, 1.);
        m_tileMuonFitDir[vecIndx(i)]->Fill(sin(mu_theta), 1.);
        m_tileMuonFitDir2[vecIndx(i)]->Fill(sin(mu_phi), 1.);
        m_tileMuonFitTime[vecIndx(i)]->Fill(mu_time, 1.);
        m_tileMuonFitDirPos[vecIndx(i)]->Fill(mu_Z, sin(mu_theta), 1.);
        m_tileMuonFitEnergy[vecIndx(i)]->Fill(mu_fullEnergy, 1.);
        //m_tileMuonFitEnergyRebin[ vecIndx(i) ]->Fill(mu_fullEnergy ,1.);
        m_tileMuonFitPath[vecIndx(i)]->Fill(mu_fullPath, 1.);
        m_tileMuonFitNCells[vecIndx(i)]->Fill(mu_trackNCells, 1.);

        if (mu_fullPath > 0) {
          //fillDensity(mu_X, mu_Z, mu_phi, mu_theta, mu_fullPath);
          m_tileMuonFitEnergyDensity[vecIndx(i)]->Fill(mu_fullEnergy * inv_mu_fullPath, 1.);
        }

      }
    } //end if number of cells in the mutrk >2

  } //end of loop over mutrk

  for (unsigned int i = 0; i < m_eventTrigs.size(); i++) {
    m_tileMuonFit[vecIndx(i)]->Fill(nmu, 1.);
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileMuonFitMonTool::procHistograms() {
/*---------------------------------------------------------*/

  if( endOfLumiBlockFlag() ||  endOfRunFlag() ) { 
    ATH_MSG_INFO( "in procHistograms()" );
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileMuonFitMonTool::checkHists(bool /* fromFinalize */) {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in checkHists()" );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
int  TileMuonFitMonTool::getTMFpart(const TileCosmicMuon* tmf) {
/*---------------------------------------------------------*/

  int part = -2;

  const CaloCellContainer* cellcoll;
  CHECK( evtStore()->retrieve(cellcoll, m_cellContainerKey), -1 );

  CaloCellContainer::const_iterator f_cell = cellcoll->begin();
  CaloCellContainer::const_iterator l_cell = cellcoll->end();

  int ncells = tmf->GetTrackNCells();
  for (int i = 0; i < ncells; i++) {
    Identifier tmfcellID = m_tileID->cell_id(tmf->GetTrackCellHash(i));

    for (; f_cell != l_cell; ++f_cell) {
      const CaloCell* cell = (*f_cell);

      Identifier cellID = cell->ID();
      if (m_tileID->is_tile(cellID)) {	    // keep only cells from TileCal calorimeter
        if (cellID == tmfcellID) {
          int p = getPartition(cell);
          if (part < 0) {
            part = p;
//            ATH_MSG_INFO( "New TMF" );
//            ATH_MSG_INFO( "Part=" << p );
          } else if (part != p) {
//            ATH_MSG_INFO( "Part=" << p );
            return -1;
          }

//          ATH_MSG_INFO( "Part=" << p );
          break;
        }
      }

    } //loop over cells container

  } //loop over track cells

//  ATH_MSG_INFO( "End of the method. Partition =" << part );
  return part;
}

/*---------------------------------------------------------*/
//void TileMuonFitMonTool::fillDensity(float x0, float z0, float phi, float theta, float path) {
/*---------------------------------------------------------*/
/*

  const float rr(5290000.); //2300 mm*2300.mm inner radius of TileCal
  if (x0 * x0 > rr) return; // we look only for muon passing through top and bottom sides

  float tgp = tan(phi);
  float tg2p = tgp * tgp;

  if (tg2p < 0.0001) return; // we are interested in horizontal muons

  float tgt = tan(theta);
  float tg2t = tgt * tgt;

  if (tg2t < 0.0001) return; // we are interested in horizontal muons

  float xup, yup, zup, xdo, ydo, zdo; //intercept on the track with top and bottom side of TileCal

  if (tg2p > 2500.) { //use vertical line approx
    yup = sqrt(rr - x0 * x0);
    xup = x0;
    ydo = -yup;
    xdo = x0;
  }

  else {

    float disc = sqrt(rr + tg2p * (rr - x0 * x0));
    float x1 = (tg2p * x0 + disc) / (1. + tg2p);
    float x2 = (tg2p * x0 - disc) / (1. + tg2p);

    float y1 = tgp * (x1 - x0);
    float y2 = tgp * (x2 - x0);

    if (y1 > 0) {
      yup = y1;
      ydo = y2;
      xup = x1;
      xdo = x2;
    } else {
      yup = y2;
      ydo = y1;
      xup = x2;
      xdo = x1;
    }
  }

  if (tg2t > 2500.) { //use vertical line approx
    zup = z0;
    zdo = z0;
  }

  else {
    zup = (yup + tgt * z0) / tgt;
    zdo = (ydo + tgt * z0) / tgt;

  }

  float lup = sqrt((xup - x0) * (xup - x0) + yup * yup + (zup - z0) * (zup - z0));
  float ldo = sqrt((xdo - x0) * (xdo - x0) + ydo * ydo + (zdo - z0) * (zdo - z0));
  float ltot = lup + ldo;
  ATH_MSG_INFO( "Points:   xup = " << xup << "\t yup = " << yup << "\t zup = " << zup );
  ATH_MSG_INFO( "Points:    x0 = " << x0 << "\t  y0 = 0.00" << "\t  z0 = " << z0 );
  ATH_MSG_INFO( "Points:   xdo = " << xdo << "\t ydo = " << ydo << "\t zdo = " << zdo );
  ATH_MSG_INFO( "angles:   phi = " << phi << "\t tgp = " << tgp << "\t tg2p = " << tg2p );
  ATH_MSG_INFO(  "angles:   the = " << theta << "\t tgt = " << tgt << "\t tg2t = " << tg2t );
  ATH_MSG_INFO( "lengths:  total = " << path << "\t lin = " << ltot << "\t lup = " << lup << " \t ldo = " << ldo );

}
*/
