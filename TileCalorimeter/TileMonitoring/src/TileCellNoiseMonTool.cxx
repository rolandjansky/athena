/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileCellNoiseMonTool.cxx
// PACKAGE:  TileMonitoring  
//
//           Produce CellNoise folder in tilemon.root for monitoring cell noise
//           Input is RAW
//           Output:
//                -- Cell noise histos for each individual cell (possibly do not keep in the final tilemon.root)
//                -- 2D map histograms per partition/gain/ for sigma1, sigma2, R, chi2, chi2prb, RMS, RMS/Sigma1 
// 
// AUTHOR:   Christophe Clement  largely (inspired from TileCellMonTool.cxx)
//
//
// ********************************************************************

#include "TileCellNoiseMonTool.h"
#include "PairBuilder.h"

#include "CaloEvent/CaloCellContainer.h"

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileCell.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TString.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <map>



/*---------------------------------------------------------*/
TileCellNoiseMonTool::TileCellNoiseMonTool(const std::string & type,  const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileBadChanTool("TileBadChanTool")
    //, m_TileCellTrig(0U)
    //, m_delta_lumiblock(0U)
    //, m_oldLumiblock(-1)
  , m_isFirstEvent(true)
  , m_mapSigma1{}
  , m_mapSigma2{}
  , m_mapR{}
  , m_mapChi2{}
  , m_mapChi2prb{}
  , m_mapRmsOsig{}
  , m_mapRms{}
  , m_hPartition1(0)
  , m_hPartition2(0)
  , m_h2Partition0(0)
  , m_h2Partition1(0)
  , m_h2Partition2(0)
  , m_h2Partition3(0)


/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("cellsContainerName"     , m_cellsContName="AllCalo"); //SG Cell Container
  declareProperty("doOnline"               , m_doOnline=false); //online mode
  declareProperty("TileBadChanTool"        , m_tileBadChanTool);
  declareProperty("Xmin"                   , m_xMin = -1000. ); //xmin for the single cell noise histos
  declareProperty("Xmax"                   , m_xMax =  1000. ); //xmax for the single cell noise histos

  m_path = "/Tile/CellNoise";

}

/*---------------------------------------------------------*/
TileCellNoiseMonTool::~TileCellNoiseMonTool()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode TileCellNoiseMonTool::initialize() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in initialize()" );

  //=== get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve() );

  ATH_MSG_INFO( "in TileCellNoiseMonTool::initialize (...) " );
  ATH_MSG_INFO( " - m_path = " << m_path );

  return TileFatherMonTool::initialize();
}

     
/*---------------------------------------------------------*/
StatusCode TileCellNoiseMonTool::bookCellNoiseHistos() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in bookCellNoiseHistos()" );
  ATH_MSG_INFO( " - m_path = " << m_path );

  // Create a vector of Long Barrel Cell CellName+CellID
  const int NLBCells  = 23;
  std::string LBCellName[NLBCells] = {
    "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10",
    "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9",
    "D0", "D1", "D2", "D3"
  };

  const int NEBCells  = 18;
  std::string EBCellName[NEBCells] = {
    "A12", "A13", "A14", "A15", "A16",
    "B11", "B12", "B13", "B14", "B15",
    "D4",  "D5", "D6",
    "E1",  "E2", "E3",   "E4", "C10"
  };

  // follows TileFatherMonTool.cxx/h m_ros2partition and m_partition2ros conventions
  const int Npartitions = 4;
  std::string PartitionName[Npartitions] = { "EBA", "LBA", "LBC", "EBC" };

  const int Nmodules = 64;

  std::string module_str;

  std::string runNumStr = getRunNumStr();

  char CellName[30];
  char modName[10];

  for (int imod = 0; imod < Nmodules; ++imod) {

    std::ostringstream convert;
    convert << imod + 1;

    if (imod <= 8) {
      module_str = "0" + convert.str();
    }
    if (imod > 8) {
      module_str = convert.str();
    }

    // MYHISTOS
    // ""
    // Long barrel cells
    for (int icell = 0; icell < NLBCells; ++icell) {

      m_tileCellEne[PartLBA][imod].push_back(
          book1F("",
              "CellNoise_" + PartitionName[PartLBA] + module_str + "_" + LBCellName[icell],
              "TileCellNoise - Run " + runNumStr + " " + PartitionName[PartLBA] + module_str + " "
                  + LBCellName[icell], 100, m_xMin, m_xMax));
      

      m_tileCellEne[PartLBC][imod].push_back(
          book1F("",
              "CellNoise_" + PartitionName[PartLBC] + module_str + "_" + LBCellName[icell],
              "TileCellNoise - Run " + runNumStr + " " + PartitionName[PartLBC] + module_str + " "
                  + LBCellName[icell], 100, m_xMin, m_xMax));
	
    } // icell
    
      // Extended barrel cells
    for (int icell = 0; icell < NEBCells; ++icell) {

      m_tileCellEne[PartEBA][imod].push_back(
          book1F("",
              "CellNoise_" + PartitionName[PartEBA] + module_str + "_" + EBCellName[icell],
              "TileCellNoise - Run " + runNumStr + " " + PartitionName[PartEBA] + module_str + " "
                  + EBCellName[icell], 100, m_xMin, m_xMax));

      m_tileCellEne[PartEBC][imod].push_back(
          book1F("",
              "CellNoise_" + PartitionName[PartEBC] + module_str + "_" + EBCellName[icell],
              "TileCellNoise  Run " + runNumStr + " " + PartitionName[PartEBC] + module_str + " "
                  + EBCellName[icell], 100, m_xMin, m_xMax));

    } // icell

  } // module

  m_hPartition1  = book1F ("" , "h_partitions1","partitions1", 8, -2., 6.);
  m_hPartition2  = book1F ("" , "h_partitions2","partitions1", 8, -2., 6.);
  m_h2Partition0 = book2F ("" , "h2_partition0","h2_partition0", 100, -2., 2., 100, 0, 6.28);
  m_h2Partition1 = book2F ("" , "h2_partition1","h2_partition1", 100, -2., 2., 100, 0, 6.28);
  m_h2Partition2 = book2F ("" , "h2_partition2","h2_partition2", 100, -2., 2., 100, 0, 6.28);
  m_h2Partition3 = book2F ("" , "h2_partition3","h2_partition3", 100, -2., 2., 100, 0, 6.28);

  for (int ipart = 0; ipart < 4; ++ipart) {
    m_mapSigma1[ipart] = book2F("", "map_sigma1_" + PartitionName[ipart]
                                 , "Sigma 1 - " + PartitionName[ipart], 23, 0., 23., 65, 0., 65.);

    m_mapSigma2[ipart] = book2F("", "map_sigma2_" + PartitionName[ipart]
                                 , "Sigma 2 - " + PartitionName[ipart], 23, 0., 23., 65, 0., 65.);

    m_mapR[ipart] = book2F("", "map_R_" + PartitionName[ipart]
                            , "R (A_{1}/A_{2})- " + PartitionName[ipart], 23, 0., 23., 65, 0., 65.);

    m_mapChi2[ipart] = book2F("", "map_chi2_" + PartitionName[ipart]
                               , "chi2 - " + PartitionName[ipart], 23, 0., 23., 65, 0., 65.);

    m_mapChi2prb[ipart] = book2F("", "map_chi2prb_" + PartitionName[ipart]
                                  , "chi2 prob. - " + PartitionName[ipart], 23, 0., 23., 65, 0., 65.);

    m_mapRmsOsig[ipart] = book2F("", "map_rmsOsig_" + PartitionName[ipart]
                                  , "RMS/ Sigma1 - " + PartitionName[ipart], 23, 0., 23., 65, 0., 65.);

    m_mapRms[ipart] = book2F("", "map_rms_" + PartitionName[ipart]
                              , "RMS - " + PartitionName[ipart], 23, 0., 23., 65, 0., 65.);

    // Set cell Names for LB partitions
    if (PartitionName[ipart] == "LBA" || PartitionName[ipart] == "LBC") {
      for (int icell = 0; icell < NLBCells; ++icell) {

        sprintf(CellName, "%s ", LBCellName[icell].c_str());
        m_mapSigma1[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapSigma2[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapR[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapChi2[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapChi2prb[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapRmsOsig[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapRms[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
      } // icell
    }  // if ipart is Barrel
    if (PartitionName[ipart] == "EBA" || PartitionName[ipart] == "EBC") {
      for (int icell = 0; icell < NEBCells; ++icell) {

        sprintf(CellName, "%s ", EBCellName[icell].c_str());
        m_mapSigma1[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapSigma2[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapR[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapChi2[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapChi2prb[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapRmsOsig[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
        m_mapRms[ipart]->GetXaxis()->SetBinLabel(icell + 1, CellName);
      } // icell
    }  // if ipart is Barrel
    //////////// SET THE MODULE NAMES ON THE Y-AXIS ////////////
    for (int imod = 1; imod < 65; imod = imod + 2) {
      sprintf(modName, "%s%s%i ", PartitionName[ipart].c_str(), (imod < 10 ? "0" : ""), imod);
      m_mapSigma1[ipart]->GetYaxis()->SetBinLabel(imod + 1, modName);
      m_mapSigma2[ipart]->GetYaxis()->SetBinLabel(imod + 1, modName);
      m_mapR[ipart]->GetYaxis()->SetBinLabel(imod + 1, modName);
      m_mapChi2[ipart]->GetYaxis()->SetBinLabel(imod + 1, modName);
      m_mapChi2prb[ipart]->GetYaxis()->SetBinLabel(imod + 1, modName);
      m_mapRmsOsig[ipart]->GetYaxis()->SetBinLabel(imod + 1, modName);
      m_mapRms[ipart]->GetYaxis()->SetBinLabel(imod + 1, modName);
    } // imod

  } // ipart

  return StatusCode::SUCCESS;
} // bookCellNoiseHistos

/*---------------------------------------------------------*/
StatusCode TileCellNoiseMonTool::bookHistograms()
/*---------------------------------------------------------*/
{
  ATH_MSG_INFO( "in bookHistograms()" );
  ATH_MSG_INFO( "---  m_path = " << m_path );

  cleanHistVec(); //necessary to avoid problems at the run, evblock, lumi blocks boundaries
  m_isFirstEvent = true;
  
  // Use all triggers (if needs to be done per trigger type then move the booking to the fillhisto where we check the trigger)

  if (bookCellNoiseHistos().isFailure()) {
    ATH_MSG_WARNING( "Error with bookCellNoiseHistos() " );
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileCellNoiseMonTool::cleanHistVec() {
/*---------------------------------------------------------*/

  for (int i = 0; i < 9; i++) {
    m_activeTrigs[i] = -1;
  }

  for (int ipart = 0; ipart < 4; ++ipart) {
    for (int imod = 0; imod < 64; ++imod) {
      m_tileCellEne[ipart][imod].clear();
    } // imod
  } // ipart

} // cleanHistVec

/*---------------------------------------------------------*/
void TileCellNoiseMonTool::do2GFit() {
/*---------------------------------------------------------*/

  ATH_MSG_VERBOSE( "in do2GFit() " );

  // check there is at least one histo to fit!
  float xmin = -1000.;
  float xmax = 1000.;

  if (m_tileCellEne[0][0].size() == 0) {
    ATH_MSG_WARNING( "in  do2GFit() - m_TileCellEne[0][0] contains zero histogram - bailing out from 2G fit section " );
    return;
  }

  TF1 fitfunction ("total", "gaus(0)+gaus(3)", xmin, xmax);
  fitfunction.SetLineColor(2);

  double fitresults[9] = {0};
  double R, rmsOsig;
  for (int ipart = 0; ipart < 4; ++ipart) {

    for (int imod = 0; imod < 64; ++imod) {
      // loop over cells
      for (unsigned int icell = 0; icell < m_tileCellEne[ipart][imod].size(); ++icell) {
        // fit the single cell energy distributions

        ATH_MSG_VERBOSE( "in  do2GFit() : ipart =  " << ipart
                         << "   imod = " << imod
                         << "   icell = " << icell );

        if (m_tileCellEne[ipart][imod][icell] != 0
            && m_tileCellEne[ipart][imod][icell]->GetEntries() > 0) {

          do2GFit(m_tileCellEne[ipart][imod][icell], fitresults, &fitfunction);
        }

	  // then store the fitresults into a permanent container
        ATH_MSG_VERBOSE( "Fit results:"
                          << " sigma1  = " << fitresults[2]
                          << "  sigma2  = " << fitresults[5]
                          << "  amp1    = " << fitresults[0]
                          << "  amp2    = " << fitresults[3]
                          << "  chi2    = " << fitresults[6]
		                      << "  chi2prb = " << fitresults[7]
		                      << "  CellRMS = " << fitresults[8] );


        //////////// Store the results in 2D maps /////////////

        //////////// Store Sigma 1,2,R, chi2, RMS/sigma , RMS /////////////
        R = (fitresults[3] != 0) ? fitresults[0] / fitresults[3] : -1;
        m_mapSigma1[ipart]->Fill((float) icell + 0.5, (float) imod + 1.5, fitresults[2]); // sigma 1
        m_mapSigma2[ipart]->Fill((float) icell + 0.5, (float) imod + 1.5, fitresults[5]); // sigma 2
        m_mapR[ipart]->Fill((float) icell + 0.5, (float) imod + 1.5, R); // R = Amp1/Amp2
        m_mapChi2[ipart]->Fill((float) icell + 0.5, (float) imod + 1.5, fitresults[6]);
        m_mapChi2prb[ipart]->Fill((float) icell + 0.5, (float) imod + 1.5, fitresults[7]);
        rmsOsig = (fitresults[2] != 0) ? (fitresults[8] / fitresults[2]) : -1;
        m_mapRmsOsig[ipart]->Fill((float) icell + 0.5, (float) imod + 1.5, rmsOsig);
        m_mapRms[ipart]->Fill((float) icell + 0.5, (float) imod + 1.5, fitresults[8]);

      } // icell

    } // imodule  

  } // partition

  return;
}

/*---------------------------------------------------------*/
void TileCellNoiseMonTool::do2GFit(TH1F* h, double * fitresults, TF1* fitfunction) {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "entering do2GFit(TH1F* h, double * fitresults, TF1* fitfunction) = " << h->GetName() );

  double par[6];

  // start values for fit parameters should be the same as in the main reconstruction
  float nentries = h->GetEntries();
  float rms = h->GetRMS();

  ATH_MSG_DEBUG( "in do2GFit(...)  : nentries = " << nentries << ", rms      = " << rms );

  par[0] = 0.1 * nentries;
  par[1] = 0.;
  par[2] = 0.7 * rms;

  par[3] = 0.15 * par[0];
  par[4] = 0.;
  par[5] = 5. * par[2];

  fitfunction->SetParameters(par);

  float bin = h->GetBinWidth(0);
  float lim1 = bin;
  float lim2 = std::max(rms * 1.05, bin * 2.0);
  float lim3 = std::max(rms * 10.0, bin * 20.);

  //  ATH_MSG_DEBUG( "in do2GFit(...)  : test1 " );

  fitfunction->SetParLimits(0,0.,nentries);
  //  ATH_MSG_DEBUG( "in do2GFit(...)  : test2 " );

  fitfunction->FixParameter(1,0.);
  fitfunction->SetParLimits(2,lim1,lim2);
  //  ATH_MSG_DEBUG( "in do2GFit(...)  : test3 " );

  fitfunction->SetParLimits(3,0.,nentries);
  //  ATH_MSG_DEBUG( "in do2GFit(...)  : test4 " );

  fitfunction->FixParameter(4,0.);
  fitfunction->SetParLimits(5,lim2,lim3);
  //  ATH_MSG_DEBUG( "in do2GFit(...)  : test5 " );

  h->Fit(fitfunction, "B");

  fitresults [0] = fitfunction->GetParameter(0);
  fitresults [1] = fitfunction->GetParameter(1);
  fitresults [2] = fitfunction->GetParameter(2);
  fitresults [3] = fitfunction->GetParameter(3);
  fitresults [4] = fitfunction->GetParameter(4);
  fitresults [5] = fitfunction->GetParameter(5);
  fitresults [6] = fitfunction->GetChisquare();

//  ATH_MSG_DEBUG( "in do2GFit(...)  : chi2     = " << fitfunction->GetChisquare() );
//
//  int NDF = fitfunction->GetNDF();
//  ATH_MSG_DEBUG( "in do2GFit(...)  : NDF      = " << NDF );
//
//  double chi2prob = TMath::Prob(fitfunction->GetChisquare(), NDF);
//  ATH_MSG_DEBUG( "in do2GFit(...)  : chi2prob = " << chi2prob );


  //fitresults [7] = chi2prob; // fitfunction->GetProb();
  fitresults[7] = fitfunction->GetProb();
  fitresults[8] = h->GetRMS();

  return;
}


/*---------------------------------------------------------*/
StatusCode TileCellNoiseMonTool::fillHistoPerCell() {
/*---------------------------------------------------------*/


  ATH_MSG_VERBOSE( "in fillHistoPerCell() " );

  // transform a channel index into a CellIndex: -1 is a channel not connected to a cell
  int LBch2cellID[48] = {
    19,  0, 10, 10,  0,  1, 11, 11,  1,  2,  2, 12,
    12, 20, 20,  3, 13, 13,  3,  4,  4, 14, 14,  5,
    21, 21,  5, 15, 15,  6, -1, -1,  6, 16, 16,  7,
     8,  8,  7, 17, 17, 22, 18, -1, 22,  9,  9, 18
  };


  int EBch2cellID[48] = {
    15, 16, 10, 10, 17, 17,  0,  0,  5,  5,  1,  1,
    13, 14,  6,  6, 11, 11, 15, 16,  2,  2,  7,  7,
    -1, -1, -1, -1, -1, -1,  8,  3,  3, -1, -1,  8,
     9, 12, 12,  9,  4,  4, -1, -1, -1, -1, -1, -1
  };


  const int NLBCells  = 23;
  std::string LBCellName[NLBCells] = {
    "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10",
    "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9",
    "D0", "D1", "D2", "D3"
  };

  const int NEBCells  = 18;
  std::string EBCellName[NEBCells] = {
    "A12", "A13", "A14", "A15", "A16",
    "B11", "B12", "B13", "B14", "B15",
    "D4",  "D5", "D6",
    "E1",  "E2", "E3",   "E4", "C10"
  };

  // do we need to do something for the first event?
  if (m_isFirstEvent) {
    initFirstEvent();
  }

  // Pointer to a Tile cell container
  const CaloCellContainer* cell_container;

  //Retrieve Cell collection from SG
  CHECK( evtStore()->retrieve(cell_container, m_cellsContName) );

  // Loop over calorimeter cells
  CaloCellContainer::const_iterator iCell = cell_container->begin();
  CaloCellContainer::const_iterator lastCell  = cell_container->end();
  
  for ( ; iCell != lastCell; ++iCell) {

    const CaloCell * cell_ptr = *iCell;     // pointer to cell object
    Identifier id = cell_ptr->ID();

    // Continue only if this is a TileCal Cell
    if (m_tileID->is_tile(id)) {

      const TileCell* tile_cell = dynamic_cast<const TileCell*>(cell_ptr);
      if (tile_cell == 0) continue;

      short ch1 = -1;
      short ch2 = -1; // channel number, range 0-47 or -1 for unknown
      int partition1 = -1;
      int partition2 = -1;

      const CaloDetDescrElement * caloDDE = tile_cell->caloDDE();

      // note that m_ros2partition returns 0-4 following definition in TileFatherMonTool.h
      IdentifierHash hash1 = caloDDE->onl1();
      if (hash1 != TileHWID::NOT_VALID_HASH) {
        HWIdentifier hw1 = m_tileHWID->channel_id(hash1);
        ch1 = m_tileHWID->channel(hw1);
        partition1 = m_ros2partition[m_tileHWID->ros(hw1)];
      }

      IdentifierHash hash2 = caloDDE->onl2();
      if (hash2 != TileHWID::NOT_VALID_HASH) {
        HWIdentifier hw2 = m_tileHWID->channel_id(hash2);
        ch2 = m_tileHWID->channel(hw2);
        partition2 = m_ros2partition[m_tileHWID->ros(hw2)];
      }


      m_hPartition1->Fill(partition1);
      m_hPartition2->Fill(partition2);

      // just to avoid potential problems with disconnected cells
      if (partition1 >= 4) partition1 = getPartition(cell_ptr);
      if (partition2 >= 4) partition2 = partition1;

      // something is wrong - go to next cell
      if (partition1 >= 4) {
        ATH_MSG_WARNING(  "TileCellNoiseMonTool:fillHistoPerCell : partition1 >= 5 ! - partition =  " << partition1 );
        ATH_MSG_WARNING( "TileCellNoiseMonTool:fillHistoPerCell : skipping to next event " );
        continue;
      }
  
      // note that in single PMT cell both badch1() and badch2() are changed together
      bool badch1 = (tile_cell->badch1());
      bool badch2 = (tile_cell->badch2());

      // 0 = both PMTs are good; 1= 1 PMT is bad; 2= both PMTs are bad, or PMT is bad for single PMT cell
      int cell_isbad = (int)badch1 + (int)badch2;

      int gn1 = tile_cell->gain1(); // gain of first PMT
      int gn2 = tile_cell->gain2(); // gain of second PMT

      bool ch1Ok = (ch1>-1 && gn1 != CaloGain::INVALIDGAIN);
      bool ch2Ok = (ch2>-1 && gn2 != CaloGain::INVALIDGAIN);

      // Cell energy and position info
      double energy = cell_ptr  -> energy();
      double eta    = cell_ptr  -> eta();
      double phi    = cell_ptr  -> phi();

      if (partition1 == 0) {
        m_h2Partition0->Fill(eta, phi);
      } else if (partition1 == 1) {
        m_h2Partition1->Fill(eta, phi);
      } else if (partition1 == 2) {
        m_h2Partition2->Fill(eta, phi);
      } else if (partition1 == 3) {
        m_h2Partition3->Fill(eta, phi);
      }

      // From the channel number we need to decide which cell this is
      
      // if LB
      int CellID1=0, CellID2=0;
      std::string CellName1 = "";
      std::string CellName2 = "";

      if (ch1 >= 0) {
        if (partition1 == PartLBA || partition1 == PartLBC) {
          CellID1 = LBch2cellID[ch1];
        } else {
          CellID1 = EBch2cellID[ch1];
        }
      }

      if (ch2 >= 0) {
        if (partition2 == PartLBA || partition2 == PartLBC) {
          CellID2 = LBch2cellID[ch2];
        } else {
          CellID2 = EBch2cellID[ch2];
        }
      }

      if (msgLvl(MSG::DEBUG) && (fabs(energy - 1.) < 0.01)) {
        msg(MSG::DEBUG) << "ch1 = " << ch1
                        << " ch2 = " << ch2
                        << "  CellID1 = " << CellID1
                        << "  CellID2 = " << CellID2 << endmsg;

        msg(MSG::DEBUG) << "partition1           = " << partition1 << endmsg;
        msg(MSG::DEBUG) << "partition2           = " << partition2 << endmsg;
        msg(MSG::DEBUG) << "m_tileID->module(id) = " << m_tileID->module(id) << endmsg;
        msg(MSG::DEBUG) << "gain1                = " << gn1 << endmsg;
        msg(MSG::DEBUG) << "gain2                = " << gn2 << endmsg;
      }

      if (partition1 == PartLBA || partition1 == PartLBC) {
        CellName1 = LBCellName[CellID1];
      } else {
        CellName1 = EBCellName[CellID1];
      }

      if (partition2 == PartLBA || partition2 == PartLBC) {
        CellName2 = LBCellName[CellID2];
      } else {
        CellName2 = EBCellName[CellID2];
      }


      if (msgLvl(MSG::DEBUG) && (fabs(energy - 1) < 0.01)) {
        msg(MSG::DEBUG) << "CellName1           = " << CellName1 << endmsg;
        msg(MSG::DEBUG) << "CellName2           = " << CellName2 << endmsg;
        msg(MSG::DEBUG) << "Cell energy         = " << energy << endmsg;
        msg(MSG::DEBUG) << "ch1Ok               = " << (ch1Ok ? 1 : 0) << "  ch2Ok           = " << (ch2Ok ? 1 : 0) << endmsg;
        msg(MSG::DEBUG) << "HistoName           = " << m_tileCellEne[partition1][m_tileID->module(id)][CellID1]->GetName() << endmsg;
      }

      ATH_MSG_DEBUG( "ch1Ok = " << ch1Ok
                     << " ch2Ok = " << ch2Ok
                     << "  cell_isbad = " << cell_isbad
                     << " gn1 = " << gn1
                     << " gn2 = " << gn2
                     << " energy = " << energy );

      //// Fill histo with cell energy ////  
      if (ch1Ok && ch2Ok && cell_isbad == 0) {
        if (gn1 == gn2) m_tileCellEne[partition1][m_tileID->module(id)][CellID1]->Fill(energy);
        //if(gn1==1 && gn2==1) 	  m_TileCellEne[partition1][m_tileID->module(id)][CellID1]->Fill(energy);

        if (CellName1 != CellName2) {
          ATH_MSG_WARNING( " CellName1 and 2 are different :  CellName1 =  " << CellName1
                           << "   CellName2 =" << CellName2 );
        }

      }

    } // if ( m_tileID->is_tile(id) 

  } // Cell Loop

  return StatusCode::SUCCESS;
} // fillHistoPerCell




/*---------------------------------------------------------*/
StatusCode TileCellNoiseMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in fillHistograms() " );

  if (fillHistoPerCell().isFailure()) {
    ATH_MSG_WARNING( "Error  in fillHistoPerCell()  " );
  }

  return StatusCode::SUCCESS;

} // fillHistograms



/*---------------------------------------------------------*/
StatusCode TileCellNoiseMonTool::procHistograms() {
/*---------------------------------------------------------*/

  if (endOfLumiBlockFlag() || endOfRunFlag()) {
    ATH_MSG_INFO( "in procHistograms()" );
  }

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileCellNoiseMonTool::finalHists() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in finalHists()" );
  ATH_MSG_INFO( " - m_path = " << m_path );

  // Call the 2G fit for all Cell energy histograms
  do2GFit();

  return StatusCode::SUCCESS;
} // finalHists()



/*---------------------------------------------------------*/
StatusCode TileCellNoiseMonTool::checkHists(bool /* fromFinalize */) {
/*---------------------------------------------------------*/
  ATH_MSG_INFO( "in checkHists()" );
  return StatusCode::SUCCESS;
} // checkHists()




// Operations to be done only once at the first event
/*---------------------------------------------------------*/
void TileCellNoiseMonTool::initFirstEvent() {
/*---------------------------------------------------------*/
  m_isFirstEvent = false; //Set the flag
} // FirstEvInit

