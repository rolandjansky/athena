/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//TileTBBeamMonTool

  //  TODO
  //  -1 to 199 to show pedestal 
  //  -0.5 to 4.5 for muons
  

//Gaudi Includes
#include "GaudiKernel/ITHistSvc.h"

//Event info
#include "xAODEventInfo/EventInfo.h"
#include "CaloEvent/CaloCellContainer.h"

//Tile Includes
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBFrag.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileMonitoring/TileTBBeamMonTool.h"
#include "TileMonitoring/TileFatherMonTool.h"
#include "TileEvent/TileCell.h"

//Histogramming
#include "TH2F.h"
#include "TH1F.h"
#include "TString.h"

//std
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

#define WRONG_SAMPLE(frag,chan,size)                                    \
msg(MSG::ERROR) << "Wrong no. of samples (" << size                     \
                << ") for channel " << chan                             \
                << " in frag 0x"<<MSG::hex << frag << MSG::dec          \
                << " - " << BeamFragName[frag&0x1F] << endmsg;

#define WRONG_CHANNEL(frag,chan)                                        \
msg(MSG::ERROR) << "Wrong channel " << chan                             \
                << " in frag 0x"<<MSG::hex << frag << MSG::dec          \
                << " - " << BeamFragName[frag&0x1F] << endmsg;

#define FRAG_FOUND(frag,chan,size)                                      \
if (msgLvl(MSG::DEBUG))                                                 \
  msg(MSG::DEBUG) << "Found channel " << chan                           \
                  << " in frag 0x"<<MSG::hex << frag << MSG::dec        \
                  << " - " << BeamFragName[frag&0x1F]                   \
                  << " of size " << size << endmsg;


#define SIGNAL_FOUND(frag,chan,amplitude)                               \
if (msgLvl(MSG::DEBUG))                                                 \
  msg(MSG::DEBUG) << "Found channel " << chan                           \
                  << " in frag 0x"<<MSG::hex << frag << MSG::dec        \
                  << " - " << BeamFragName[frag&0x1F]                   \
                  << " with amp=" << amplitude << endmsg;

TileTBBeamMonTool::TileTBBeamMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_isFirstEvent(true)
  , m_s1cou(0)
  , m_s2cou(0)
  , m_s3cou(0)
  , m_cher1(0)
  , m_cher2(0)
  , m_cher3(0)
  , m_muTag(0)
  , m_muHalo(0)
  , m_muVeto(0)
  , m_pmt1cou(0)
  , m_pmt2cou(0)
  , m_pmt3cou(0)
  , m_pmt4cou(0)
  , m_pmt5cou(0)
  , m_pmt6cou(0)
  , m_pmt7cou(0)
  , m_pmt8cou(0)
  , m_pmt9cou(0)
  , m_pmt10cou(0)
  , m_pmt11cou(0)
  , m_pmt12cou(0)
  , m_xCha1(0.0F)
  , m_yCha1(0.0F)
  , m_xCha2(0.0F)
  , m_yCha2(0.0F)
  , m_xCha1_sum(0.0F)
  , m_yCha1_sum(0.0F)
  , m_xCha2_sum(0.0F)
  , m_yCha2_sum(0.0F)
  , m_xImp(0.0F)
  , m_yImp(0.0F)
  , m_total_energy(0)
  , m_cell_energy(0)
  , m_total_muon_energy(0)
{


  //Beam Chamber Calibrations

  declareProperty("BC1Z", m_beamBC1Z  = 15600.0);
  declareProperty("BC2Z", m_beamBC2Z  = 2600.0); 

  // June 2017 calibration, https://pcata007.cern.ch/elog/TB2017/550 (schae@cern.ch)
  declareProperty("BC1HorizontalSlope", m_horizontal_slope1 = -0.175220);
  declareProperty("BC1HorizontalOffset", m_horizontal_offset1 = 0.153584934082);
  declareProperty("BC1VerticalSlope", m_vertical_slope1 = -0.176567356723);
  declareProperty("BC1VerticalOffset", m_vertical_offset1 = -0.493246053303);

  declareProperty("BC2HorizontalSlope", m_horizontal_slope2 = -0.176122);
  declareProperty("BC2HorizontalOffset", m_horizontal_offset2 = 0.414611893278);
  declareProperty("BC2VerticalSlope", m_vertical_slope2 = -0.173472808704);
  declareProperty("BC2VerticalOffset", m_vertical_offset2 = 0.150807740888);

  /*
  // Constant updated 29-09-26 by Joakim Olsson: https://pcata007.cern.ch/elog/TB2016/300
  declareProperty("BC1HorizontalSlope", m_horizontal_slope1 = -0.172098);
  declareProperty("BC1HorizontalOffset", m_horizontal_offset1 = 0.100857923042);
  declareProperty("BC1VerticalSlope", m_vertical_slope1 = -0.172855178323);
  declareProperty("BC1VerticalOffset", m_vertical_offset1 = -0.133045996607);

  declareProperty("BC2HorizontalSlope", m_horizontal_slope2 = -0.173463);
  declareProperty("BC2HorizontalOffset", m_horizontal_offset2 = 0.271555258578);
  declareProperty("BC2VerticalSlope", m_vertical_slope2 = -0.173805131744);
  declareProperty("BC2VerticalOffset", m_vertical_offset2 = 0.305483228502);
  */

//  //Constants updated 13-10-15 by Vincent & Michaela: https://pcata007.cern.ch/elog/TB2015/107
//  declareProperty("BC1HorizontalSlope", m_horizontal_slope1 = -0.175666);
//  declareProperty("BC1HorizontalOffset", m_horizontal_offset1 = -0.0462586);
//  declareProperty("BC1VerticalSlope", m_vertical_slope1 = -0.176809);
//  declareProperty("BC1VerticalOffset", m_vertical_offset1 = -0.051923);
//
//  declareProperty("BC2HorizontalSlope", m_horizontal_slope2 = -0.18053);
//  declareProperty("BC2HorizontalOffset", m_horizontal_offset2 = 0.25202);
//  declareProperty("BC2VerticalSlope", m_vertical_slope2 = -0.181128);
//  declareProperty("BC2VerticalOffset", m_vertical_offset2 = 0.0431688);

  //Energy cut for cell impact plot
  declareProperty("CutEnergyMin", m_cut_energy_min = 0); //for 50GeV electrons 
  declareProperty("CutEnergyMax", m_cut_energy_max = 100); //for 50GeV electrons 

                                          // # , BC1X1 = 0.0798
                                          // # , BC1X2 = 0.169
                                          // # , BC1Y1 = -0.0977
                                          // # , BC1Y2 = 0.169
                                          // # , BC2X1 = -0.0217178
                                          // # , BC2X2 = 0.181873
                                          // # , BC2Y1 = 0.0412262
                                          // # , BC2Y2 = 0.181259)

  //Containers used 

  declareProperty("TileBeamElemContainer", m_beamElemContainer = "TileBeamElemCnt");
  declareProperty("CellContainer", m_cellContainer = "AllCalo"); //SG Cell Container
}

TileTBBeamMonTool::~TileTBBeamMonTool() {
}

StatusCode TileTBBeamMonTool:: initialize() {

    ATH_MSG_INFO( "in initialize()" );
    return TileFatherMonTool::initialize();
}


StatusCode TileTBBeamMonTool::storeBeamElements() {


  m_total_muon_energy = 0;

  if ( m_beamElemContainer.size() == 0)
    return StatusCode::SUCCESS;

  //retreive elements from beam element container
  const TileBeamElemContainer* BeamElemCnt;
  CHECK( evtStore()->retrieve(BeamElemCnt, m_beamElemContainer) );

  //iterate over container elements
  TileBeamElemContainer::const_iterator collItr = BeamElemCnt->begin();
  TileBeamElemContainer::const_iterator lastColl = BeamElemCnt->end();

  for(; collItr != lastColl; ++collItr) {

    //iterate over 
    TileBeamElemCollection::const_iterator beamItr = (*collItr)->begin();
    TileBeamElemCollection::const_iterator lastBeam = (*collItr)->end();

    //output for verbose...
    if (msgLvl(MSG::VERBOSE)) {

      for (; beamItr != lastBeam; ++beamItr) {
        HWIdentifier id = (*beamItr)->adc_HWID();
        std::vector<uint32_t> digits = (*beamItr)->get_digits();
        msg(MSG::VERBOSE) << " --- TileBeamElem -- Identifier " << m_tileHWID->to_string(id) << " ";
        msg(MSG::VERBOSE) << (*beamItr)->get_digits().size() << " " << digits.size() << endmsg;
        msg(MSG::VERBOSE) << MSG::VERBOSE << " --- TileBeamElem -- BeamElem : " << MSG::dec;
        for (unsigned int k = 0; k < digits.size(); k++)
          msg(MSG::VERBOSE) << digits[k] << " ";
        msg(MSG::VERBOSE) << endmsg;
      }
      //restore iterator
      beamItr = (*collItr)->begin();
    }

    //retreive frag identifiers
    int frag = (*collItr)->identify();

    for (; beamItr != lastBeam; ++beamItr) {
      
      HWIdentifier id = (*beamItr)->adc_HWID();
      std::vector<uint32_t> digits = (*beamItr)->get_digits();
      int cha = m_tileHWID->channel(id);
      int dsize = digits.size();
      
      if ( dsize <= 0 ) {
        
        WRONG_SAMPLE(frag, cha, dsize);
        
      } else {
        
        int amplitude = digits[0];
        SIGNAL_FOUND(frag, cha, amplitude);
        
        switch (frag) {

        case MUON_ADC_FRAG:

          switch(cha) {
          
          // set counter values individual PMTs:
          case 0: m_pmt1cou = amplitude; break;
          case 1: m_pmt2cou = amplitude; break;
          case 2: m_pmt3cou = amplitude; break;
          case 3: m_pmt4cou = amplitude; break;
          case 4: m_pmt5cou = amplitude; break;
          case 5: m_pmt6cou = amplitude; break;
          case 6: m_pmt7cou = amplitude; break;
          case 7: m_pmt8cou = amplitude; break;
	    /*
          case 8: m_pmt9cou = amplitude; break;
          case 9: m_pmt10cou = amplitude; break;
          case 10: m_pmt11cou = amplitude; break;
          case 11: m_pmt12cou = amplitude; break;
	    */
          default: WRONG_CHANNEL(frag, cha);

          }

          if(cha < 8) {
            m_muBack[cha] = amplitude;
            m_total_muon_energy += amplitude;
          }

          break;

	case ADDR_ADC_FRAG:

	  switch(cha) {

	  case 0: m_pmt9cou = amplitude; break;
	  case 1: m_pmt10cou = amplitude; break;
	  case 2: m_pmt11cou = amplitude; break;
	  case 3: m_pmt12cou = amplitude; break;
	  case 4:
	  case 5:
	  case 6:
	  case 7: break;
	  default: WRONG_CHANNEL(frag, cha);
	  }

          if(cha < 6) {
            m_muBack[8+cha] = amplitude;
            m_total_muon_energy += amplitude;
          }

	  break;
	    
	case COMMON_TDC1_FRAG:
          
          FRAG_FOUND(frag, cha, dsize);
          
          if (cha < 16) m_btdc[cha] = amplitude;
          else WRONG_CHANNEL(frag, cha);
          break;
          
        case BEAM_ADC_FRAG:
          
          switch(cha) {
          
	    // set counter values for 1D beam elements
          case 0: m_s1cou = amplitude; break;
          case 1: m_s2cou = amplitude; break;
          case 2: m_s3cou = amplitude; break;
          case 3: m_cher1 = amplitude; break;
          case 4: m_cher2 = amplitude; break;
          case 5: m_muTag = amplitude; break;
          case 6: m_muHalo= amplitude; break;
          case 7: m_muVeto= amplitude; break;
          default: WRONG_CHANNEL(frag, cha);
	    
          }
          break;
        }
      }
    }
  }

  //Retreiving total event energy...
    // Pointer to a Tile cell container
  const CaloCellContainer* cell_container;

    //Retrieve Cell collection from SG
  CHECK( evtStore()->retrieve(cell_container, m_cellContainer) );

  m_total_energy = 0;
  m_cell_energy = 0;

  if (cell_container->empty()) return StatusCode::SUCCESS;

  for (const CaloCell* cell : *cell_container) {
    //    Identifier id = cell->ID();
    const TileCell* tile_cell = dynamic_cast<const TileCell*>(cell);
    if (tile_cell == 0) continue;

    //append cell id --- print on plot all the cell id's used 
    // Identifier m_cell_id = cell->ID();

    double energy = cell->energy() * 0.001; // keep energy in pC
    m_total_energy += energy;

    //keep energy of maximum cell to see modulation at impact
    if (energy > m_cell_energy)  {
      m_cell_energy = energy; //in pC again
    }
  }

  // Beam Chamber Coordinates

  // For BC1
  m_xCha1 = (m_btdc[1] - m_btdc[0]) * m_horizontal_slope1 + m_horizontal_offset1;
  m_yCha1 = (m_btdc[2] - m_btdc[3]) * m_vertical_slope1 + m_vertical_offset1; //(up - down)

  // For BC2:
  m_xCha2 = (m_btdc[5] - m_btdc[4]) * m_horizontal_slope2 + m_horizontal_offset2;
  m_yCha2 = (m_btdc[6] - m_btdc[7]) * m_vertical_slope2 + m_vertical_offset2; //(up-down)

  //Sum Plots

  // For BC1
  m_xCha1_sum = (m_btdc[1] + m_btdc[0]) * m_horizontal_slope1 + m_horizontal_offset1;
  m_yCha1_sum = (m_btdc[2] + m_btdc[3]) * m_vertical_slope1 + m_vertical_offset1;

  //For BC2
  m_xCha2_sum = (m_btdc[5] + m_btdc[4]) * m_horizontal_slope2 + m_horizontal_offset2;
  m_yCha2_sum = (m_btdc[6] + m_btdc[7]) * m_vertical_slope2 + m_vertical_offset2;

  //Impact Coordinates

  //for one cell, plot energy total as a function of x Impact -- (m_xImp, cell_energy)...

  m_xImp = m_xCha2 + (m_xCha2 - m_xCha1) * m_beamBC2Z / (m_beamBC1Z - m_beamBC2Z);
  m_yImp = m_yCha2 + (m_yCha2 - m_yCha1) * m_beamBC2Z / (m_beamBC1Z - m_beamBC2Z);

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTBBeamMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "::bookHistograms() :  m_path =  " << m_path  );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTBBeamMonTool::fillHistograms() {
/*---------------------------------------------------------*/
  ATH_MSG_DEBUG( "in fillHistograms()" );

  if (m_isFirstEvent) {
  // fill event info like L1 trigger type, run number, etc...
  fillEvtInfo();
  initFirstEvent();
  }

  CHECK( storeBeamElements() );

  m_cher3 = m_s3cou; // Test beam September 2016

  //Filling histograms
  m_BC1Profile->Fill(m_xCha1, m_yCha1);
  m_BC2Profile->Fill(m_xCha2, m_yCha2); 
  m_BC1ProfileSum->Fill(m_xCha1_sum, m_yCha1_sum); 
  m_BC2ProfileSum->Fill(m_xCha2_sum, m_yCha2_sum); 
  m_BC1Xhist->Fill(m_xCha1); 
  m_BC1Yhist->Fill(m_yCha1); 
  m_BC2Xhist->Fill(m_xCha2); 
  m_BC2Yhist->Fill(m_yCha2);
  m_CellEnergyImpactX->Fill(m_xImp, m_cell_energy); 
  m_CellEnergyImpactY->Fill(m_yImp, m_cell_energy); 
  m_TotalEnergyImpactX->Fill(m_xImp, m_total_energy); 
  m_TotalEnergyImpactY->Fill(m_yImp, m_total_energy); 
  m_S1hist->Fill(m_s1cou); 
  m_S2hist->Fill(m_s2cou); 
  m_S3hist->Fill(m_s3cou); 
  m_Cher1hist->Fill(m_cher1); 
  m_Cher2hist->Fill(m_cher2); 
  m_Cher3hist->Fill(m_cher3); 
  m_ImpactProfile->Fill(m_xImp, m_yImp); 
  m_Cher1Energy->Fill(m_total_energy, m_cher1);
  m_Cher2Energy->Fill(m_total_energy, m_cher2);
  m_Cher3Energy->Fill(m_total_energy, m_cher3);
  m_CherCompare->Fill(m_cher1, m_cher2);
  m_ScinCalEnergy->Fill(m_s1cou, m_total_energy);
  m_MuonEnergy->Fill(m_total_muon_energy);
  m_PMT1->Fill(m_pmt1cou);
  m_PMT2->Fill(m_pmt2cou);
  m_PMT3->Fill(m_pmt3cou);
  m_PMT4->Fill(m_pmt4cou);
  m_PMT5->Fill(m_pmt5cou);
  m_PMT6->Fill(m_pmt6cou);
  m_PMT7->Fill(m_pmt7cou);
  m_PMT8->Fill(m_pmt8cou);
  m_PMT9->Fill(m_pmt9cou);
  m_PMT10->Fill(m_pmt10cou);
  m_PMT11->Fill(m_pmt11cou);
  m_PMT12->Fill(m_pmt12cou);

  //for hit map

  m_PMTHitMap->SetBinContent(1, 1, m_pmt8cou);
  m_PMTHitMap->SetBinContent(2, 1, m_pmt7cou);
  m_PMTHitMap->SetBinContent(3, 1, m_pmt6cou);
  m_PMTHitMap->SetBinContent(4, 1, m_pmt5cou);
  m_PMTHitMap->SetBinContent(1, 2, m_pmt4cou);
  m_PMTHitMap->SetBinContent(2, 2, m_pmt3cou);
  m_PMTHitMap->SetBinContent(3, 2, m_pmt2cou);
  m_PMTHitMap->SetBinContent(4, 2, m_pmt1cou);

  //...

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileTBBeamMonTool::initFirstEvent() {
/*---------------------------------------------------------*/
  m_isFirstEvent = false; //Set the flag

  //Retreiving run number
  std::string runNumber = getRunNumStr();

  // Booking histograms...

  //BC1 Profile
  m_BC1Profile = book2F("", "BC1Profile", "Run " + runNumber + ": BC1 Beam Profile",1000, -100., 100., 1000, -100., 100., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_BC1Profile->GetXaxis()->SetTitle("X (mm)");
  m_BC1Profile->GetYaxis()->SetTitle("Y (mm)");
  m_BC1Profile->SetOption("COLZ");

  //BC1 Sum Plot
  m_BC1ProfileSum = book2F("", "BC1ProfileSum", "Run " + runNumber + ": BC1 Beam Profile Sum",1000, -300., 0., 1000, -300., 0., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_BC1ProfileSum->GetXaxis()->SetTitle("X (mm)");
  m_BC1ProfileSum->GetYaxis()->SetTitle("Y (mm)");
  m_BC1ProfileSum->SetOption("COLZ");

  //BC2 Profile
  m_BC2Profile = book2F("", "BC2Profile", "Run " + runNumber + ": BC2 Beam Profile", 1000, -100., 100., 1000, -100., 100., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_BC2Profile->GetXaxis()->SetTitle("X (mm)");
  m_BC2Profile->GetYaxis()->SetTitle("Y (mm)");
  m_BC2Profile->SetOption("COLZ");

  //BC2 Sum Plot
  m_BC2ProfileSum = book2F("", "BC2ProfileSum", "Run " + runNumber + ": BC2 Beam Profile Sum",1000, -300., 0., 1000, -300., 0., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_BC2ProfileSum->GetXaxis()->SetTitle("X (mm)");
  m_BC2ProfileSum->GetYaxis()->SetTitle("Y (mm)");
  m_BC2ProfileSum->SetOption("COLZ");

  //1-D XY Beam Profiles

  //BC1 X&Y Profiles

  m_BC1Xhist = book1F("", "BC1Xhist", "Run " + runNumber + ": BC1X", 200, -100., 100., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_BC1Xhist->GetXaxis()->SetTitle("X (mm)"); 
  m_BC1Xhist->GetYaxis()->SetTitle("Counts"); 

  m_BC1Yhist = book1F("", "BC1Yhist", "Run " + runNumber + ": BC1Y", 200, -100., 100., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_BC1Yhist->GetXaxis()->SetTitle("Y (mm)"); 
  m_BC1Yhist->GetYaxis()->SetTitle("Counts"); 

  //BC2 X&Y Profiles

  m_BC2Xhist = book1F("", "BC2Xhist", "Run " + runNumber + ": BC2X", 200, -100., 100., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_BC2Xhist->GetXaxis()->SetTitle("X (mm)"); 
  m_BC2Xhist->GetYaxis()->SetTitle("Counts"); 

  m_BC2Yhist = book1F("", "BC2Yhist", "Run " + runNumber + ": BC2Y", 200, -100., 100., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_BC2Yhist->GetXaxis()->SetTitle("Y (mm)"); 
  m_BC2Yhist->GetYaxis()->SetTitle("Counts"); 

  //2D Histogram of Cell energy and impact XY  
  m_CellEnergyImpactX = book2F("", "CellEnergyImpactX", "Run " + runNumber + ": Cell Energy Impact X ",1000, -100., 100., 1000, m_cut_energy_min, m_cut_energy_max, run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_CellEnergyImpactX->GetXaxis()->SetTitle("X (mm)");
  m_CellEnergyImpactX->GetYaxis()->SetTitle("Cell Energy");
  m_CellEnergyImpactX->SetOption("COLZ");

  m_CellEnergyImpactY = book2F("", "CellEnergyImpactY", "Run " + runNumber + ": Cell Energy Impact Y ",1000, -100., 100., 1000, m_cut_energy_min, m_cut_energy_max, run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_CellEnergyImpactY->GetXaxis()->SetTitle("Y (mm)");
  m_CellEnergyImpactY->GetYaxis()->SetTitle("Cell Energy");
  m_CellEnergyImpactY->SetOption("COLZ");

  //2D Histogram of Total Energy and impact XY
  m_TotalEnergyImpactX = book2F("", "TotalEnergyImpactX", "Run " + runNumber + ": Total Energy Impact X ",1000, -100., 100., 1000, m_cut_energy_min, m_cut_energy_max, run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_TotalEnergyImpactX->GetXaxis()->SetTitle("X (mm)");
  m_TotalEnergyImpactX->GetYaxis()->SetTitle("Total Energy");
  m_TotalEnergyImpactX->SetOption("COLZ");

  m_TotalEnergyImpactY = book2F("", "TotalEnergyImpactY", "Run " + runNumber + ": Total Energy Impact Y ",1000, -100., 100., 1000, m_cut_energy_min, m_cut_energy_max, run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_TotalEnergyImpactY->GetXaxis()->SetTitle("Y (mm)");
  m_TotalEnergyImpactY->GetYaxis()->SetTitle("Total Energy");
  m_TotalEnergyImpactY->SetOption("COLZ");

  //Impact profile
  m_ImpactProfile = book2F("", "ImpactProfile", "Run " + runNumber + ": Impact Profile",1000, -100., 100., 1000, -100., 100., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_ImpactProfile->GetXaxis()->SetTitle("X (mm)");
  m_ImpactProfile->GetYaxis()->SetTitle("Y (mm)");
  m_ImpactProfile->SetOption("COLZ");

  //2D Cherenkov response wrt total energy response of the calorimeter:
  m_Cher1Energy = book2F("", "Cher1Energy", "Run " + runNumber + ": Cher1 counts v. Total Energy",500, 0., 200., 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_Cher1Energy->GetXaxis()->SetTitle("Total energy");
  m_Cher1Energy->GetYaxis()->SetTitle("Cher1");
  m_Cher1Energy->SetOption("COLZ");

  m_Cher2Energy = book2F("", "Cher2Energy", "Run " + runNumber + ": Cher2 counts v. Total Energy",500, 0., 200., 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_Cher2Energy->GetXaxis()->SetTitle("Total energy");
  m_Cher2Energy->GetYaxis()->SetTitle("Cher2");
  m_Cher2Energy->SetOption("COLZ");

  m_Cher3Energy = book2F("", "Cher3Energy", "Run " + runNumber + ": Cher3 counts v. Total Energy",500, 0., 200., 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_Cher3Energy->GetXaxis()->SetTitle("Total energy");
  m_Cher3Energy->GetYaxis()->SetTitle("Cher3");
  m_Cher3Energy->SetOption("COLZ");


  //2D cher 1 vs. cher 2
  m_CherCompare = book2F("", "CherCompare", "Run " + runNumber + ": Cher 2 vs. Cher 1",500, 0., 1700., 500, 0., 1000., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_CherCompare->GetXaxis()->SetTitle("Cher1");
  m_CherCompare->GetYaxis()->SetTitle("Cher2");
  m_CherCompare->SetOption("COLZ");

  //2D Scintillator vs calorimeter energy
  m_ScinCalEnergy = book2F("", "ScinCalEnergy", "Run " + runNumber + ": Total Energy vs. Scintillator Counts",500, 0., 5000., 500, 0., 200., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_ScinCalEnergy->GetXaxis()->SetTitle("Scintillator Counts");
  m_ScinCalEnergy->GetYaxis()->SetTitle("Total Energy");
  m_ScinCalEnergy->SetOption("COLZ");

  //1D elements histograms

  m_S1hist = book1F("", "S1hist", "Run " + runNumber + ": S1 Hist", 200, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_S1hist->GetYaxis()->SetTitle("Counts");

  m_S2hist = book1F("", "S2hist", "Run " + runNumber + ": S2 Hist", 200, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_S2hist->GetYaxis()->SetTitle("Counts");

  m_S3hist = book1F("", "S3hist", "Run " + runNumber + ": S3 Hist", 200, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_S3hist->GetYaxis()->SetTitle("Counts");

  m_Cher1hist = book1F("", "Cher1hist", "Run " + runNumber + ": Cher1 Hist", 200, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_Cher1hist->GetYaxis()->SetTitle("Counts");

  m_Cher2hist = book1F("", "Cher2hist", "Run " + runNumber + ": Cher2 Hist", 200, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_Cher2hist->GetYaxis()->SetTitle("Counts");

  m_Cher3hist = book1F("", "Cher3hist", "Run " + runNumber + ": Cher3 Hist", 200, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_Cher3hist->GetYaxis()->SetTitle("Counts");


  m_MuonEnergy = book1F("", "TotalMuonEnergy", "Run " + runNumber + ": Muon Wall Total Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_MuonEnergy->GetYaxis()->SetTitle("Counts");

  m_PMT1 = book1F("", "MuonWallPMT1", "Run " + runNumber + ": Muon Wall PMT1 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT1->GetYaxis()->SetTitle("Counts");

  m_PMT2 = book1F("", "MuonWallPMT2", "Run " + runNumber + ": Muon Wall PMT2 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT2->GetYaxis()->SetTitle("Counts");

  m_PMT3 = book1F("", "MuonWallPMT3", "Run " + runNumber + ": Muon Wall PMT3 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT3->GetYaxis()->SetTitle("Counts");

  m_PMT4 = book1F("", "MuonWallPMT4", "Run " + runNumber + ": Muon Wall PMT4 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT4->GetYaxis()->SetTitle("Counts");

  m_PMT5 = book1F("", "MuonWallPMT5", "Run " + runNumber + ": Muon Wall PMT5 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT5->GetYaxis()->SetTitle("Counts");

  m_PMT6 = book1F("", "MuonWallPMT6", "Run " + runNumber + ": Muon Wall PMT6 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT6->GetYaxis()->SetTitle("Counts");

  m_PMT7 = book1F("", "MuonWallPMT7", "Run " + runNumber + ": Muon Wall PMT7 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT7->GetYaxis()->SetTitle("Counts");

  m_PMT8 = book1F("", "MuonWallPMT8", "Run " + runNumber + ": Muon Wall PMT8 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT8->GetYaxis()->SetTitle("Counts");

  m_PMT9 = book1F("", "MuonWallPMT9", "Run " + runNumber + ": Muon Wall PMT9 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT9->GetYaxis()->SetTitle("Counts");

  m_PMT10 = book1F("", "MuonWallPMT10", "Run " + runNumber + ": Muon Wall PMT10 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT10->GetYaxis()->SetTitle("Counts");

  m_PMT11 = book1F("", "MuonWallPMT11", "Run " + runNumber + ": Muon Wall PMT11 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT11->GetYaxis()->SetTitle("Counts");

  m_PMT12 = book1F("", "MuonWallPMT12", "Run " + runNumber + ": Muon Wall PMT12 Hist", 500, 0., 5000., run, ATTRIB_MANAGED, "", "mergeRebinned"),
  m_PMT12->GetYaxis()->SetTitle("Counts");

  //PMT hit map
  m_PMTHitMap = book2F("", "PMTHitMap", "Run " + runNumber + ": PMT Hit Map", 4, 0., 4., 2, 0., 2., run, ATTRIB_MANAGED, "", "mergeRebinned");
  m_PMTHitMap->SetOption("COLZ");
  m_PMTHitMap->SetStats(0);



}
 
/*---------------------------------------------------------*/
StatusCode TileTBBeamMonTool::procHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in procHistograms()" );

  return StatusCode::SUCCESS;
}



