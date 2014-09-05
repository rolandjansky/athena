/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDQFragMonTool.cxx
// PACKAGE:  
//
// AUTHOR:   Natanael Nunes de Moura Junior
//
//
// ********************************************************************

#include "AthenaKernel/errorcheck.h"

#include "TileMonitoring/TileDQFragMonTool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileDCSSvc.h"

#include "TH2I.h"
#include "TProfile.h"
#include "TVirtualPad.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>


/*---------------------------------------------------------*/
TileDQFragMonTool::TileDQFragMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileBadChanTool("TileBadChanTool")
  , m_tileDCSSvc("TileDCSSvc",name)
  , m_beamInfo("TileBeamInfoProvider")
  , dqStatus(0)
  , m_last_lb(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("TileRawChannelContainerDSP",   m_contNameDSP =     "TileRawChannelCnt"); //SG DSP RCh Container
  declareProperty("TileRawChannelContainerOffl",  m_contNameOffline = "TileRawChannelFixed"); //SG offline RCh Container
  declareProperty("TileDigitsContainer",          m_contNameDigits =  "TileDigitsCnt");     // Digits
  declareProperty("NegAmpHG",m_negAmpHG = -200.);  // cut on negative amplitude in high gain
  declareProperty("NegAmpLG",m_negAmpLG = -15.);   // cut on negative amplitude in low gain
  declareProperty("JumpDeltaHG",  m_jumpDeltaHG = 99.9); // minimal jump in high gain
  declareProperty("JumpDeltaLG",  m_jumpDeltaLG = 49.9); // minimal jump in low gain
  declareProperty("PedDetlaHG",   m_pedDeltaHG  =  4.1); // max variation of "const" value in high gain
  declareProperty("PedDetlaLG",   m_pedDeltaLG  =  4.1); // max variation of "const" value in low gain
  declareProperty("SkipMasked",   m_skipMasked = true);  // ignore masked channels or not
  declareProperty("SkipGapCells", m_skipGapCells = true);// ignore MBTS and E1-E4 cells in negE detection
  declareProperty("doPlots",m_doPlots = false); // print plots of the histograms
  declareProperty("doOnline",m_doOnline = false); // online mode
  declareProperty("CheckDCS",m_checkDCS = false);
  declareProperty("TileBadChanTool"        , m_tileBadChanTool);
  m_path = "Tile/DQFrag"; //ROOT file directory
  
  // starting up the label variable....
  m_ErrorsLabels.clear();
  m_ErrorsLabels.push_back("OK");                 // Error: 0
  m_ErrorsLabels.push_back("HEADER_FORM");        // Error: 1
  m_ErrorsLabels.push_back("HEADER_PAR");         // Error: 2
  m_ErrorsLabels.push_back("MEMO_PAR");           // Error: 3
  m_ErrorsLabels.push_back("FE_CRC");             // Error: 4
  m_ErrorsLabels.push_back("ROD_CRC");            // Error: 5
  m_ErrorsLabels.push_back("BCID");               // Error: 6
  m_ErrorsLabels.push_back("SAMPLE_FORM");        // Error: 7
  m_ErrorsLabels.push_back("SAMPLE_PAR");         // Error: 8
  m_ErrorsLabels.push_back("DOUBLE_STB");         // Error: 9
  m_ErrorsLabels.push_back("SINGLE_STB");         // Error: 10
  m_ErrorsLabels.push_back("GLOBAL_CRC");         // Error: 11
  m_ErrorsLabels.push_back("MASKED");             // Error: 12
  // corrupted data
  m_ErrorsLabels.push_back("0 -> 1023");          // Error: NERR - 1 + 1
  m_ErrorsLabels.push_back("Zeros");              // Error: NERR - 1 + 2
  m_ErrorsLabels.push_back("Two 1023 + ped");     // Error: NERR - 1 + 3
  m_ErrorsLabels.push_back("Jump 2 levels");      // Error: NERR - 1 + 4
  m_ErrorsLabels.push_back("Single Up + ped");    // Error: NERR - 1 + 5
  m_ErrorsLabels.push_back("Single Dn + ped");    // Error: NERR - 1 + 6
  m_ErrorsLabels.push_back("Single Up + sig");    // Error: NERR - 1 + 7
  m_ErrorsLabels.push_back("Single Dn + sig");    // Error: NERR - 1 + 8
  m_ErrorsLabels.push_back("Ped>200 LG");         // Error: NERR - 1 + 9
  m_ErrorsLabels.push_back("Single Dn LG_s0");    // Error: NERR - 1 + 10
  m_ErrorsLabels.push_back("Single Dn LG_s6");    // Error: NERR - 1 + 11
  m_ErrorsLabels.push_back("Up LG_s0_s6 or Gap"); // Error: NERR - 1 + 12
  m_ErrorsLabels.push_back("Dn LG_s0_s6 or Gap"); // Error: NERR - 1 + 13
  
  m_PartitionsLabels.push_back("LBA");
  m_PartitionsLabels.push_back("LBC");
  m_PartitionsLabels.push_back("EBA");
  m_PartitionsLabels.push_back("EBC");
  
  m_UpdateTotal = 0; 
  m_UpdateCount = 0;
  m_UpdateCount_shadow[0] = 0;
  m_UpdateCount_shadow[1] = 0;
  m_UpdateCount_shadow[2] = 0;
  m_UpdateCount_shadow[3] = 0;
  m_UpdateCount_shadow[4] = 0;
  m_UpdateCount_shadow[5] = 0;
  m_UpdateCount_shadow[6] = 0;
  m_UpdateCount_shadow[7] = 0;
  m_UpdateCount_shadow[8] = 0;
  m_UpdateCount_shadow[9] = 0;
  m_Update      = 50; // default..

  errorDir = "/Tile/DMUErrors";
  

#if 0
  // not used in the code
    int dmu_map[16][3] = {    {0,1,2},       // DMU 1...  
			      {3,4,5},       // DMU 2...
			      {6,7,8},       // DMU 3...
			      {9,10,11},     // DMU 4...
			      {12,13,14},    // DMU 5...
			      {15,16,17},    // DMU 6...
			      {18,19,20},    // DMU 7...
			      {21,22,23},    // DMU 8...
			      {24,25,26},    // DMU 9...
			      {27,28,29},    // DMU 10...
			      {30,31,32},    // DMU 11...
			      {33,34,35},    // DMU 12...
			      {36,37,38},    // DMU 13...
			      {39,40,41},    // DMU 14...
			      {42,43,44},    // DMU 15...
			      {45,46,47}};   // DMU 16...
    for (int i=0;i<16;++i) {
      for (int j=0;j<3;++j) {
	m_dmu_map[i][j] = dmu_map[i][j];
      }
    }
  //this map can be find on: https://twiki.cern.ch/twiki/bin/viewfile/Atlas/TileBarrelCommissioning?rev=1;filename=mapping_PMT_CHANNEL_AUG-2007.pdf
#endif /* 0 */

  std::ostringstream ss;
  std::string ebcell[48] = {"E3","E4","D4","D4","C10","C10","A12","A12","B11","B11","A13","A13",
                            "E1","E2","B12","B12","D5","D5","","","A14","A14","B13","B13",
                            "","","","","","","B14","A15","A15","","","B14",
                            "B15","D6","D6","B15","A16","A16","","","","","",""};

  std::string lbcell[48] = {"D0","A1","B1","B1","A1","A2","B2","B2","A2","A3","A3","B3",
                            "B3","D1","D1","A4","B4","B4","A4","A5","A5","B5","B5","A6",
                            "D2","D2","A6","B6","B6","A7","","","A7","B7","B7","A8",
                            "A9","A9","A8","B8","B8","D3","B9","","D3","A10","A10","B9"};

  m_PartNames[PartEBA] = "EBA";
  m_PartNames[PartLBA] = "LBA";
  m_PartNames[PartLBC] = "LBC";
  m_PartNames[PartEBC] = "EBC";

  for (int p = 0; p < NumPart; p++) {
    for (int m = 1; m < 65; m++) {
      ss.str("");
      if (m % 2 == 1) {
        ss << m_PartNames[p] << std::setfill('0') << std::setw(2) << m << std::setfill(' '); //EBA01, EBA03,...
      } else {
        ss.str(" ");
      }
      m_moduleLabel[p].push_back(ss.str());
    }
    for (int ch = 0; ch < 48; ch++) {
      ss.str("");
      if ((p < PartLBA) || (p > PartLBC))
        ss << ebcell[ch];
      else
        ss << lbcell[ch];
      if ((ss.str()).length() > 0) ss << "_";
      ss << "ch" << ch; //D0_ch1, A1_ch2, .., ch44, ...
      m_cellchLabel[p].push_back(ss.str());
    }
  }
}

/*---------------------------------------------------------*/
TileDQFragMonTool::~TileDQFragMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileDQFragMonTool:: initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO(  "in initialize()" );

  CHECK( m_beamInfo.retrieve() );

  CHECK(  m_tileBadChanTool.retrieve() );

  CHECK( TileFatherMonTool::initialize() );

  if (m_checkDCS) {
    CHECK( m_tileDCSSvc.retrieve() );
  }

  return StatusCode::SUCCESS;

}


/*---------------------------------------------------------*/
/* njunior@cern.ch */
/*
 *---------------------------------------------------------*
void TileDQFragMonTool::rebin(TH2I* hist)*/
/*---------------------------------------------------------*
{*/
/*
MsgStream log(msgSvc(), name());

//log << MSG::INFO << "in REBIN" << endreq;
if (m_ErrorsLabels.size()<2) {

m_ErrorsLabels.clear();
m_ErrorsLabels.push_back("OK");                 // Error: 0
m_ErrorsLabels.push_back("HEADER_FORM");        // Error: 1
m_ErrorsLabels.push_back("HEADER_PAR");         // Error: 2
m_ErrorsLabels.push_back("MEMO_PAR");           // Error: 3
m_ErrorsLabels.push_back("FE_CRC");             // Error: 4
m_ErrorsLabels.push_back("ROD_CRC");            // Error: 5
m_ErrorsLabels.push_back("BCID");               // Error: 6
m_ErrorsLabels.push_back("SAMPLE_FORM");        // Error: 7
m_ErrorsLabels.push_back("SAMPLE_PAR");         // Error: 8
m_ErrorsLabels.push_back("DOUBLE_STB");         // Error: 9
m_ErrorsLabels.push_back("SINGLE_STB");         // Error: 10 
}
//log << MSG::INFO << "after PUSH_BACK" << endreq;
std::string hname,htitle;
hname = hist->GetName();
htitle= hist->GetTitle();
//log << MSG::INFO << "after GET NAME" << endreq;

//deregHist(hist);
//delete hist;
//(m_manager->m_objMap).erase(hname);
hist = book2I(errorDir,hname,htitle,16,0.0,16.0,11,0.0,11.0);
SetBinLabel(hist->GetYaxis(),m_ErrorsLabels);
log << MSG::INFO << "after SETBINLABEL" << endreq;

}
*/
 
/*---------------------------------------------------------*/
StatusCode TileDQFragMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  m_UpdateTotal = 0; //reset counters
  m_UpdateCount = 0;
  m_UpdateCount_shadow[0] = 0;
  m_UpdateCount_shadow[1] = 0;
  m_UpdateCount_shadow[2] = 0;
  m_UpdateCount_shadow[3] = 0;
  m_UpdateCount_shadow[4] = 0;
  m_UpdateCount_shadow[5] = 0;
  m_UpdateCount_shadow[6] = 0;
  m_UpdateCount_shadow[7] = 0;
  m_UpdateCount_shadow[8] = 0;
  m_UpdateCount_shadow[9] = 0;
  m_last_lb = 0u; // save value

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileDQFragMonTool::bookFirstHist(  )
/*---------------------------------------------------------*/
{
  ATH_MSG_INFO( "in bookFirstHist" );

  std::string runNumStr = getRunNumStr();      
    
    
  // The First Global Histogram
  std::vector<std::string> globalName,globalTitle;
  globalName.push_back("TileGlobalCRC_errors_top");
  globalName.push_back("TileGlobalCRC_errors_bottom");
  globalTitle.push_back("Run "+runNumStr+": Global Error Top [1,32]");
  globalTitle.push_back("Run "+runNumStr+": Global Error Bottom[33,64]");
    
  std::string globalDir = m_stem + "/DMUErrors";
  hist_global[0] = book2I(globalDir,globalName[0],globalTitle[0],32,0.5,32.5,4,0.,4.);
  hist_global[0]->GetXaxis()->SetTitle("Drawer");
  SetBinLabel(hist_global[0]->GetYaxis(),m_PartitionsLabels);
    
  // The Second Global Histogram
  hist_global[1] = book2I(globalDir,globalName[1],globalTitle[1],32,32.5,64.5,4,0.,4);
  SetBinLabel(hist_global[1]->GetYaxis(),m_PartitionsLabels);
  hist_global[1]->GetXaxis()->SetTitle("Drawer");

  // Histograms of bad drawers
  std::string badDrawersDir = m_stem + "/DMUErrors/BadDrawers";
  //if (m_contNameDSP.size() > 0) {
    //hist_BadDrawerBCID = book1I(badDrawersDir, "TileBadDrawersBcid", "# of drawers with BCID errors in all MB", 256, 0.5, 256.5);
    //hist_BadDrawerBCID -> GetXaxis()->SetTitle("# modules with BCID errors in all MB");
    //hist_BadDrawerBCID_lb = bookProfile(badDrawersDir, "TileBadDrawersBcidLB", "# of drawers with BCID errors in all MB",
//	1500, -0.5, 1499.5);
    //hist_BadDrawerBCID_lb -> GetXaxis()->SetTitle("LumiBlock");
    //hist_BadDrawerBCID_lb -> GetYaxis()->SetTitle("# modules with BCID errors in all MB");

    //hist_BadDrawerBCID_nonmask = book1I(badDrawersDir, "TileBadDrawersBcidNonMask", "# of drawers with not masked BCID errors in all MB", 256, 0.5, 256.5);
    //hist_BadDrawerBCID_nonmask -> GetXaxis()->SetTitle("# modules with BCID errors in all MB");
    //hist_BadDrawerBCID_lb_nonmask = bookProfile(badDrawersDir, "TileBadDrawersBcidLBNonMask", "# of drawers with not masked BCID errors in all MB",
//	1500, -0.5, 1499.5);
    //hist_BadDrawerBCID_lb_nonmask -> GetXaxis()->SetTitle("LumiBlock");
    //hist_BadDrawerBCID_lb_nonmask -> GetYaxis()->SetTitle("# modules with not masked BCID errors in all MB");
  //} else {
    //hist_BadDrawerBCID = NULL;
    //hist_BadDrawerBCID_lb = NULL;
    //hist_BadDrawerBCID_nonmask = NULL;
    //hist_BadDrawerBCID_lb_nonmask = NULL;
  //}

  if (m_contNameOffline.size() > 0) {
    //hist_BadChanNeg[0] = book1I(badDrawersDir, "TileBadChannelNegAmpLG", "# of LG channels with Amp < -15 counts (-10 GeV)", 100, 0.5, 100.5);
    //hist_BadChanNeg[0] -> GetXaxis()->SetTitle("# LG channels with Amp < -15 counts (-10 GeV)");
    //hist_BadChanNeg_lb[0] = bookProfile(badDrawersDir, "TileBadChannelNegAmpLGLB", "# of LG channels with Amp < -15 counts (-10 GeV)",
//	1500, -0.5, 1499.5);
    //hist_BadChanNeg_lb[0] -> GetXaxis()->SetTitle("LumiBlock");
    //hist_BadChanNeg_lb[0] -> GetYaxis()->SetTitle("# LG channels with Amp < -15 counts (-10 GeV)");

    //hist_BadChanNeg[1] = book1I(badDrawersDir, "TileBadChannelNegAmpHG", "# of HG channels with Amp < -200 counts (-2 GeV)", 100, 0.5, 100.5);
    //hist_BadChanNeg[1] -> GetXaxis()->SetTitle("# HG channels with Amp < -200 counts (-2 GeV)");
    //hist_BadChanNeg_lb[1] = bookProfile(badDrawersDir, "TileBadChannelNegAmpHGLB", "# of HG channels with Amp < -200 counts (-2 GeV)",
//	1500, -0.5, 1499.5);
    //hist_BadChanNeg_lb[1] -> GetXaxis()->SetTitle("LumiBlock");
    //hist_BadChanNeg_lb[1] -> GetYaxis()->SetTitle("# HG channels with Amp < -200 counts (-2 GeV)");

    //hist_BadChanNeg_nonmask[0] = book1I(badDrawersDir, "TileBadChannelNegAmpLGNonMask", "# of not masked LG channels with Amp < -15 counts (-10 GeV)", 100, 0.5, 100.5);
    //hist_BadChanNeg_nonmask[0] -> GetXaxis()->SetTitle("# LG channels with Amp < -15 counts (-10 GeV)");
    //hist_BadChanNeg_lb_nonmask[0] = bookProfile(badDrawersDir, "TileBadChannelNegAmpLGLBNonMask", "# of not masked LG channels with Amp < -15 counts (-10 GeV)",
//	1500, -0.5, 1499.5);
    //hist_BadChanNeg_lb_nonmask[0] -> GetXaxis()->SetTitle("LumiBlock");
    //hist_BadChanNeg_lb_nonmask[0] -> GetYaxis()->SetTitle("# not masked LG channels with Amp < -15 counts (-10 GeV)");

    //hist_BadChanNeg_nonmask[1] = book1I(badDrawersDir, "TileBadChannelNegAmpHGNonMask", "# of not masked HG channels with Amp < -200 counts (-2 GeV)", 100, 0.5, 100.5);
    //hist_BadChanNeg_nonmask[1] -> GetXaxis()->SetTitle("# HG channels with Amp < -200 counts (-2 GeV)");
    //hist_BadChanNeg_lb_nonmask[1] = bookProfile(badDrawersDir, "TileBadChannelNegAmpHGLBNonMask", "# of not masked HG channels with Amp < -200 counts (-2 GeV)",
//	1500, -0.5, 1499.5);
    //hist_BadChanNeg_lb_nonmask[1] -> GetXaxis()->SetTitle("LumiBlock");
    //hist_BadChanNeg_lb_nonmask[1] -> GetYaxis()->SetTitle("# not masked HG channels with Amp < -200 counts (-2 GeV)");

    for (int p = 0; p < NumPart; p++)
    {
      hist_BadChannelNeg2D[p] = book2S(badDrawersDir, "TileBadChannelsNegMap" + m_PartNames[p],
	  "# Negative Amp in " + m_PartNames[p], 64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(hist_BadChannelNeg2D[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(hist_BadChannelNeg2D[p]->GetYaxis(), m_cellchLabel[p]);

      hist_BadChannelNeg2D_nonmask[p] = book2S(badDrawersDir, "TileBadChannelsNegMapNonMask" + m_PartNames[p],
	  "# Not Masked Negative Amp in " + m_PartNames[p], 64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(hist_BadChannelNeg2D_nonmask[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(hist_BadChannelNeg2D_nonmask[p]->GetYaxis(), m_cellchLabel[p]);
    }
  } else {
    //hist_BadChanNeg[0] = NULL;
    //hist_BadChanNeg_lb[0] = NULL;
    //hist_BadChanNeg[1] = NULL;
    //hist_BadChanNeg_lb[1] = NULL;
    hist_BadChannelNeg2D[0] = NULL;
    hist_BadChannelNeg2D[1] = NULL;
    hist_BadChannelNeg2D[2] = NULL;
    hist_BadChannelNeg2D[3] = NULL;

    //hist_BadChanNeg_nonmask[0] = NULL;
    //hist_BadChanNeg_lb_nonmask[0] = NULL;
    //hist_BadChanNeg_nonmask[1] = NULL;
    //hist_BadChanNeg_lb_nonmask[1] = NULL;
    hist_BadChannelNeg2D_nonmask[0] = NULL;
    hist_BadChannelNeg2D_nonmask[1] = NULL;
    hist_BadChannelNeg2D_nonmask[2] = NULL;
    hist_BadChannelNeg2D_nonmask[3] = NULL;
  }

  if (m_contNameDigits.size() > 0) {
    //hist_BadDrawerJump = book1I(badDrawersDir, "TileBadDrawersJump", "# of drawers with Jump errors", 256, 0.5, 256.5);
    //hist_BadDrawerJump -> GetXaxis()->SetTitle("# modules with Jump errors");
    //hist_BadDrawerJump_lb = bookProfile(badDrawersDir, "TileBadDrawersJumpLB", "# of drawers with Jump errors",
//	1500, -0.5, 1499.5);
    //hist_BadDrawerJump_lb -> GetXaxis()->SetTitle("LumiBlock");
    //hist_BadDrawerJump_lb -> GetYaxis()->SetTitle("# modules with Jump errors");

    //hist_BadDrawerJump_nonmask = book1I(badDrawersDir, "TileBadDrawersJumpNonMask", "# of drawers with not masked Jump errors", 256, 0.5, 256.5);
    //hist_BadDrawerJump_nonmask -> GetXaxis()->SetTitle("# modules with Jump errors");
    //hist_BadDrawerJump_lb_nonmask = bookProfile(badDrawersDir, "TileBadDrawersJumpLBNonMask", "# of drawers with not masked Jump errors",
//	1500, -0.5, 1499.5);
    //hist_BadDrawerJump_lb_nonmask -> GetXaxis()->SetTitle("LumiBlock");
    //hist_BadDrawerJump_lb_nonmask -> GetYaxis()->SetTitle("# modules with not masked Jump errors");

    for (int p = 0; p < NumPart; p++)
    {
      hist_BadChannelJump2D[p] = book2S(badDrawersDir, "TileBadChannelsJumpMap" + m_PartNames[p],
	  "# Jump errors in " + m_PartNames[p], 64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(hist_BadChannelJump2D[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(hist_BadChannelJump2D[p]->GetYaxis(), m_cellchLabel[p]);

      hist_BadChannelJump2D_nonmask[p] = book2S(badDrawersDir, "TileBadChannelsJumpMapNonMask" + m_PartNames[p],
	  "# not masked Jump errors in " + m_PartNames[p], 64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(hist_BadChannelJump2D_nonmask[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(hist_BadChannelJump2D_nonmask[p]->GetYaxis(), m_cellchLabel[p]);
    }
  } else {
    //hist_BadDrawerJump = NULL;
    //hist_BadDrawerJump_lb = NULL;
    hist_BadChannelJump2D[0] = NULL;
    hist_BadChannelJump2D[1] = NULL;
    hist_BadChannelJump2D[2] = NULL;
    hist_BadChannelJump2D[3] = NULL;

    //hist_BadDrawerJump_nonmask = NULL;
    //hist_BadDrawerJump_lb_nonmask = NULL;
    hist_BadChannelJump2D_nonmask[0] = NULL;
    hist_BadChannelJump2D_nonmask[1] = NULL;
    hist_BadChannelJump2D_nonmask[2] = NULL;
    hist_BadChannelJump2D_nonmask[3] = NULL;
  }

  memset(globalErrCount,0,sizeof(globalErrCount));

  for (int ros = 0; ros < 4; ros++) {
    for (int drawer = 0; drawer < 64; ++drawer){
      bookErrHist(ros,drawer);
    }
  }
  
  
}
/*---------------------------------------------------------*/



/*---------------------------------------------------------*/
void TileDQFragMonTool::bookErrHist(int ros, int drawer) {
/*---------------------------------------------------------*/

  std::ostringstream sStr;
  sStr << m_PartitionsLabels[ros] << std::setfill('0') << std::setw(2)
       << drawer+1  << std::setfill(' ');
  std::string moduleName=sStr.str();
  std::string histName,histTitle;
  std::string histlbName, histlbTitle;
 
  std::string runNumStr = getRunNumStr();
 
  histTitle = "Run "+runNumStr+": "+ moduleName+ " DMU Header Errors";
  histName = "TileDigiErrors"+moduleName;
 
  histlbTitle = "Run "+runNumStr+": "+ moduleName+ " (#total_events - #ok_events) / (#total_events)";
  histlbName = "FracTileDigiErrors"+moduleName;
 
   
  hist_error[ros][drawer] = book2I(errorDir,histName.c_str(), histTitle.c_str(),16,0.0,16.0,NERR + NCORRUPTED,0.,NERR + NCORRUPTED);
  if (m_doOnline) { // create "shadow" histograms for last 10 LB
    for (int i = 0; i < 10; i++)
      hist_error_shadow[ros][drawer][i] = new TH2I(*hist_error[ros][drawer]);
  }
  hist_error[ros][drawer]->GetXaxis()->SetTitle("DMU");
  SetBinLabel(hist_error[ros][drawer]->GetYaxis(),m_ErrorsLabels);  

  if (m_doOnline)
  {
    hist_error_lb[ros][drawer] = bookProfile(errorDir, histlbName.c_str(), histlbTitle.c_str(),
      100, -99.5, 0.5, -1.1, 1.1);
    hist_error_lb[ros][drawer]->GetXaxis()->SetTitle("Last LumiBlocks");
  }
  else
  {
    hist_error_lb[ros][drawer] = bookProfile(errorDir, histlbName.c_str(), histlbTitle.c_str(),
      1500, -0.5, 1499.5, -1.1, 1.1);
    hist_error_lb[ros][drawer]->GetXaxis()->SetTitle("LumiBlock");
  }
  hist_error_lb[ros][drawer]->GetYaxis()->SetTitle("Fraction of Digital errors");

  //if (drawer==1) {//we fill the summary histogram

    //histTitle =  "Run "+runNumStr+": "+m_PartitionsLabels[ros]+" number of events with NOT MASKED Digital Errors ";
    //histName = m_PartitionsLabels[ros]+ "summaryOfErrors";
    //hist_summary[ros] = book1I(errorDir,histName.c_str(), histTitle.c_str(),64,0.5,64.5);
    //hist_summary[ros]->GetXaxis()->SetTitle("Drawer");
    //hist_summary[ros]->GetYaxis()->SetTitle("Number of Events with not masked DigErr");
 // }

}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
StatusCode TileDQFragMonTool::fillHistograms() {
/*---------------------------------------------------------*/


  fillEvtInfo();
  m_UpdateCount++;

  if ( (m_UpdateTotal==0)&&(m_UpdateCount==1)) { // first event of the run, fill masked dmu
    m_last_lb = getLumiBlock();
    bookFirstHist();
    fillMasking();
  }

  if(m_last_lb < getLumiBlock()){
    updateHistograms();
  }
  m_UpdateCount_shadow[getLumiBlock() % 10]++;

  fillBadDrawer();
  for (int ros = 0; ros < 4; ros++) {
    for (int drawer = 0; drawer < 64; ++drawer){
      fillErrHist(ros,drawer);
      fillGlobalHist(ros,drawer);
    }
    //hist_summary[ros]->SetEntries(m_nEvents);
  }

  hist_global[0]->SetEntries(m_nEvents);
  hist_global[1]->SetEntries(m_nEvents);
  if(m_UpdateCount > m_Update){
    updateHistograms();
  }

  if(m_last_lb < getLumiBlock()){
    m_last_lb = getLumiBlock();
  }
  //*mlog <<  MSG::INFO << "m_nEvents, m_UpdateTotal, m_UpdateCount: " << m_nEvents << ", " << m_UpdateTotal << ", " << m_UpdateCount << endreq;

  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragMonTool::fillBadDrawer() {
/*---------------------------------------------------------*/


  dqStatus = m_beamInfo->getDQstatus();

#if 0
  if (m_contNameDSP.size() > 0) {
    const TileRawChannelContainer* RawChannelCnt;

    if (evtStore()->retrieve(RawChannelCnt, m_contNameDSP).isFailure()) {
      ATH_MSG_WARNING( "Can't retrieve DSP RawChannel from TES" );
      m_contNameDSP = "";
    } else {

      TileRawChannelContainer::const_iterator collItr = RawChannelCnt->begin();
      TileRawChannelContainer::const_iterator lastColl = RawChannelCnt->end();

      int nBadDr = 0;
      int nBadDrNM = 0;

      for (; collItr != lastColl; ++collItr) {
        int status = findDrawerErrors(*collItr);
        if (status == 4) {
          ++nBadDr;
          int fragId = (*collItr)->identify();
          int drawer = fragId & 0x3F;  // range 0-63
          int ROS = fragId >> 8;  // range 1-4
          bool drbad = true;
          if (m_checkDCS) drbad = m_tileDCSSvc->statusIsBad(ROS, drawer);

          if (!drbad) ++nBadDrNM;
        }
      }
      //hist_BadDrawerBCID -> Fill((double) nBadDr);
      //hist_BadDrawerBCID_lb -> Fill(getLumiBlock(), (double) nBadDr);
      //hist_BadDrawerBCID_nonmask -> Fill((double) nBadDrNM);
      //hist_BadDrawerBCID_lb_nonmask -> Fill(getLumiBlock(), (double) nBadDrNM);
    }
  }
#endif /* 0 */


  uint32_t lvl1info = getL1info();
  bool phys = (lvl1info == 0) || (((lvl1info >> Trig_b7) & 1) == 1);
  if (phys && m_contNameOffline.size() > 0) {
    const TileRawChannelContainer* RawChannelCntOffl;

    if(evtStore()->retrieve(RawChannelCntOffl, m_contNameOffline).isFailure()) {
      ATH_MSG_WARNING( "Can't retrieve offline RawChannel from TES" );
      m_contNameOffline = "";
    } else {

      TileRawChannelUnit::UNIT rChUnit = RawChannelCntOffl->get_unit();
      if (rChUnit != TileRawChannelUnit::ADCcounts && rChUnit != TileRawChannelUnit::OnlineADCcounts ) {
        ATH_MSG_WARNING( "RawChannel units are not ADC counts - WILL NOT CHECK NEGATIVE AMPLITUDES" );

      } else {
          
        const TileDigitsContainer* DigCnt=0;
        TileDigitsContainer::const_iterator DigCollItr;
        TileDigitsContainer::const_iterator lastDigColl;
        if (m_contNameDigits.size() > 0) {
          if(evtStore()->retrieve(DigCnt, m_contNameDigits).isFailure()) {
            ATH_MSG_WARNING( "Can't retrieve Digits from TES" );
            m_contNameDigits = "";
            DigCnt=0;
          } else {
            DigCollItr  = DigCnt->begin();
            lastDigColl = DigCnt->end();
          }
        }
    
        TileRawChannelContainer::const_iterator collItr  = RawChannelCntOffl->begin();
        TileRawChannelContainer::const_iterator lastColl = RawChannelCntOffl->end();

        int nBadCh[2] = {0,0};
        int nBadChNM[2] = {0,0};
       
        for (; collItr != lastColl; ++collItr) {
          TileRawChannelCollection::const_iterator chItr=(*collItr)->begin();
          TileRawChannelCollection::const_iterator lastCh=(*collItr)->end();

          int fragId = (*collItr)->identify();
          int drawer = (fragId & 0x3F); // range 0-63
          int module = drawer + 1;  // range 1-64
          int ROS = fragId >> 8;  // range 1-4
          int partition = m_ros2partition[ROS]; // range 0-3

          for (; chItr != lastCh; ++chItr) {
            const TileRawChannel * pRch = (*chItr);
            HWIdentifier adcId = pRch->adc_HWID();
            int channel = m_tileHWID->channel(adcId);
            int gain = m_tileHWID->adc(adcId);

            if (findChannelErrors(*chItr, gain) > 0) {
              ++nBadCh[gain];

              hist_BadChannelNeg2D[partition]->Fill(module, channel);
              if (isModuleDCSgood(ROS, drawer) && dqStatus->isChanDQgood(ROS, drawer, channel)) {
                ++nBadChNM[gain];
                hist_BadChannelNeg2D_nonmask[partition]->Fill(module, channel);

                float ped = pRch->pedestal(); // errors are saved in ped as 100000 + 10000*error
                if (ped > 100000. && DigCnt) {
                  while (((*DigCollItr)->identify() != fragId) && (DigCollItr != lastDigColl)) {
                    ++DigCollItr;
                  }

                  if (DigCollItr != lastDigColl) {

                    TileDigitsCollection::const_iterator digItr = (*DigCollItr)->begin();
                    TileDigitsCollection::const_iterator lastDig = (*DigCollItr)->end();

                    for (; digItr != lastDig; ++digItr) {

                      const TileDigits * pDigits = (*digItr);
                      if (m_tileHWID->channel(pDigits->adc_HWID()) == channel) {

                        msg(MSG::INFO) << "LB " << getLumiBlock()
                                       << " Evt " << getEvtNum()
                                       << " " << m_PartNames[partition]
                                       << std::setfill('0') << std::setw(2) << module
                                       << std::setfill(' ') << std::setw(3) << channel
                                       << ((gain)?" HG":" LG") << " neg amp: "
                                       << std::setprecision(2) << std::fixed << std::setw(7)
                                       << (*chItr)->amplitude()
                                       << "   Samples: " << std::setprecision(0);

                        std::vector<float> vdigits = (*digItr)->samples();
                        unsigned int nSamp = vdigits.size();
                        for (unsigned int i = 0; i < nSamp; ++i) {
                          msg(MSG::INFO) << vdigits[i] << " ";
                        }

                        msg(MSG::INFO) << "  error = " << TileRawChannelBuilder::BadPatternName(ped) << endmsg;
                      }
                    }
                  }
                }
              }
            }
          }
        }
        //hist_BadChanNeg[0] -> Fill((double) (nBadCh[0]));
        //hist_BadChanNeg_lb[0] -> Fill(getLumiBlock(), (double) (nBadCh[0]));
        //hist_BadChanNeg[1] -> Fill((double) (nBadCh[1]));
        //hist_BadChanNeg_lb[1] -> Fill(getLumiBlock(), (double) (nBadCh[1]));

        //hist_BadChanNeg_nonmask[0] -> Fill((double) (nBadChNM[0]));
        //hist_BadChanNeg_lb_nonmask[0] -> Fill(getLumiBlock(), (double) (nBadChNM[0]));
        //hist_BadChanNeg_nonmask[1] -> Fill((double) (nBadChNM[1]));
        //hist_BadChanNeg_lb_nonmask[1] -> Fill(getLumiBlock(), (double) (nBadChNM[1]));
      }
    }
  }


  if (m_contNameDigits.size() > 0) {
    const TileDigitsContainer* DigitsCnt;

    if (evtStore()->retrieve(DigitsCnt, m_contNameDigits).isFailure()) {
      ATH_MSG_WARNING("Can't retrieve Digits from TES");
      m_contNameDigits = "";
    } else {

      uint16_t jumps_corruption[4][64][NCORRUPTED] = { { { 0u } } };
      TileDigitsContainer::const_iterator collItr = DigitsCnt->begin();
      TileDigitsContainer::const_iterator lastColl = DigitsCnt->end();

      int nBadDr = 0;
      int nBadDrNM = 0;

      for (; collItr != lastColl; ++collItr) {
        TileDigitsCollection::const_iterator digitsItr = (*collItr)->begin();
        TileDigitsCollection::const_iterator lastDigits = (*collItr)->end();

        int nBadCh = 0;
        int nBadChNM = 0;

        int fragId = (*collItr)->identify();
        int drawer = (fragId & 0x3F); // range 0-63
        int module = drawer + 1;  // range 1-64
        int ROS = fragId >> 8;  // range 1-4
        int partition = m_ros2partition[ROS]; // range 0-3

        int error;
        float dmin, dmax;
        for (; digitsItr != lastDigits; ++digitsItr) {
          const TileDigits * pDigits = (*digitsItr);
          HWIdentifier adcId = pDigits->adc_HWID();
          int channel = m_tileHWID->channel(adcId);
          int gain = m_tileHWID->adc(adcId);

          if ((error = TileRawChannelBuilder::CorruptedData(ROS, drawer, channel, gain, pDigits->samples(), dmin, dmax)) > 0) {
            ++nBadCh;
            if (msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG) << "LB " << getLumiBlock()
                              << " Evt " << getEvtNum()
                              << " " << m_PartNames[partition] << std::setfill('0') << std::setw(2) << module
                              << std::setfill(' ') << std::setw(3) << channel
                              << ((gain) ? " HG" : " LG")
                              << "   Samples: ";

              std::vector<float> vdigits = pDigits->samples();
              unsigned int nSamp=vdigits.size();
              for (unsigned int i=0; i<nSamp; ++i){
                msg(MSG::DEBUG) << vdigits[i] << " ";
              }
              msg(MSG::DEBUG) << "  error = " << TileRawChannelBuilder::BadPatternName(100000. + error * 10000) << endmsg;
            }
          
#if 0
	    /*
	     * debuging of partition mapping in different kinds of histograms
	     * by faking errors
	     * also change '>' to '>=' in condition 20 lines above
	     */
            if (partition + 10 == module && partition * 10 == channel) error = partition + 1;
            if (error == 0) continue;
#endif /* 0 */

            hist_BadChannelJump2D[partition]->Fill(module, channel);
            if (isModuleDCSgood(ROS, drawer) && dqStatus->isChanDQgood(ROS, drawer, channel)) {
              ++nBadChNM;
              hist_BadChannelJump2D_nonmask[partition]->Fill(module, channel);
              if (error <= NCORRUPTED)
                jumps_corruption[ROS - 1][drawer][error - 1] |= 1u << (unsigned int) (channel / 3);
            }
          }
        }
        if (nBadCh > 0) {
          ++nBadDr;
        }

        if (nBadChNM > 0) ++nBadDrNM;
      }
      //hist_BadDrawerJump -> Fill((double) nBadDr);
      //hist_BadDrawerJump_lb -> Fill(getLumiBlock(), (double) nBadDr);
      //hist_BadDrawerJump_nonmask -> Fill((double) nBadDrNM);
      //hist_BadDrawerJump_lb_nonmask -> Fill(getLumiBlock(), (double) nBadDrNM);
      for (int partition = 0; partition < NumPart; partition++)
        for (int drawer = 0; drawer < 64; drawer++)
          for (int error = 0; error < NCORRUPTED; error++)
            if (jumps_corruption[partition][drawer][error] > 0u)
              for (unsigned int dmu = 0u; dmu < 16u; dmu++)
                if (jumps_corruption[partition][drawer][error] & (1u << dmu))
                  fillOneErrHist(partition, drawer, dmu, NERR + error);
    }
  }
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragMonTool::fillOneErrHist(int ros, int drawer, int dmu, int error) {
/*---------------------------------------------------------*/

  hist_error[ros][drawer]->Fill(dmu, error);
  if (m_doOnline) hist_error_shadow[ros][drawer][m_last_lb % 10]->Fill(dmu, error);
}

/*---------------------------------------------------------*/
void TileDQFragMonTool::fillErrHist(int ros, int drawer) {
/*---------------------------------------------------------*/


  bool hasErr = false;
  int n_error_nonmask_DMU = 0;
  unsigned int cur_lb = getLumiBlock();

  if (m_last_lb < cur_lb) { // if new LB subtract the 10th old and move bins
    if (m_doOnline) {
      for (uint32_t lb = m_last_lb + 1; lb <= cur_lb; lb++) {
        hist_error[ros][drawer]->Add(hist_error_shadow[ros][drawer][lb % 10], -1.);
        hist_error_shadow[ros][drawer][lb % 10]->Reset();
      }
      ShiftTprofile(hist_error_lb[ros][drawer], cur_lb - m_last_lb);
    }
  }

  dqStatus = m_beamInfo->getDQstatus();

  if (!isModuleDCSgood(ros + 1, drawer)) {
    if (m_doOnline)
      hist_error_lb[ros][drawer]->Fill(0., -1.);
    else
      hist_error_lb[ros][drawer]->Fill(cur_lb, -1.);
  } else {
    for (int idmu = 0; idmu < NDMU; idmu++) { // loop over dmus
      int ichn = 3 * idmu;

      if (dqStatus->isChanDQgood(ros + 1, drawer, ichn)) {
        fillOneErrHist(ros, drawer, idmu, 0);
      } else {
        hasErr |= CheckhasErr(ros, drawer, idmu);
        if (CheckhasErr(ros, drawer, idmu)) n_error_nonmask_DMU++;
        if (dqStatus->checkHeaderFormatErr(ros + 1, drawer, idmu, 0) != 0) { // in case of format errors, we only fill this one
          fillOneErrHist(ros, drawer, idmu, 1);
        } else {
          if (dqStatus->checkHeaderParityErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 2);
          }
          if (dqStatus->checkMemoryParityErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 3);
          }
          if (dqStatus->checkFE_CRCErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 4);
          }
          if (dqStatus->checkROD_CRCErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 5);
          }
          if (dqStatus->checkBCIDErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 6);
          }
          if (dqStatus->checkSampleFormatErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 7);
          }
          if (dqStatus->checkSampleParityErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 8);
          }
          if (dqStatus->checkDoubleStrobeErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 9);
          }
          if (dqStatus->checkSingleStrobeErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 10);
          }
          if (dqStatus->checkGlobalCRCErr(ros + 1, drawer, 0) != 0) {
            fillOneErrHist(ros, drawer, idmu, 11);
          }
        }
      }
    } // loop over dmus
    if (m_doOnline)
      hist_error_lb[ros][drawer]->Fill(0., (double) n_error_nonmask_DMU / 16.);
    else
      hist_error_lb[ros][drawer]->Fill(cur_lb, (double) n_error_nonmask_DMU / 16.);
  }

  //if (hasErr) {
  //  hist_summary[ros]->Fill(drawer+1);
  // }

} //end of function
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragMonTool::fillGlobalHist(int ros, int drawer) {
  /*---------------------------------------------------------*/

  if (dqStatus->checkGlobalCRCErr(ros + 1, drawer, 0) != 0) { //if
    globalErrCount[ros][drawer]++;
    if (drawer < 32) {
      hist_global[0]->Fill(drawer + 1, (ros), 1.0);
    } else { // else
      hist_global[1]->Fill(drawer + 1, (ros), 1.0);
    } //else
  } // if
}
/*---------------------------------------------------------*/



/*---------------------------------------------------------*/
void TileDQFragMonTool::updateHistograms() {
  /*---------------------------------------------------------*/

  // if m_last_lb < getLumiBlock we need store cached events
  // for the latest event m_last_lb is valid
  //
  int drop_entries = 0;
  if (m_doOnline && m_last_lb < getLumiBlock()) {
    for (uint32_t lb = m_last_lb + 1; lb <= getLumiBlock(); lb++) {
      drop_entries += m_UpdateCount_shadow[lb % 10];
      m_UpdateCount_shadow[lb % 10] = 0u;
    }
  }

  for (int ros = 0; ros < 4; ros++) {  //loop over ros...
    for (int drawer = 0; drawer < 64; ++drawer) {
      for (int dmu = 0; dmu < NDMU; dmu++) {
        Int_t bin = hist_error[ros][drawer]->FindBin(dmu, NERR - 1);
        if (hist_error[ros][drawer]->GetBinContent(bin) > 0)
          hist_error[ros][drawer]->SetBinContent(bin, m_UpdateTotal + m_UpdateCount - drop_entries);
        hist_error[ros][drawer]->SetEntries(m_UpdateTotal + m_UpdateCount - drop_entries);
      }  // ...dmus
    }  //...drawers

  }  // loop over ros...

  m_UpdateTotal = m_UpdateTotal + m_UpdateCount - drop_entries;
  m_UpdateCount = 0;
}


/*---------------------------------------------------------*/
void TileDQFragMonTool::drawErrHist(int ros, int drawer) {
  /*---------------------------------------------------------*/

  std::ostringstream sStr;
  sStr << m_PartitionsLabels[ros] << std::setfill('0') << std::setw(2) << drawer + 1
      << std::setfill(' ');
  std::string moduleName = sStr.str();
  std::string histName, histTitle;

  sStr.str("");
  sStr << moduleName << "_dmuErr";
  histTitle = sStr.str();
  histName = moduleName;

  TCanvas* can = new TCanvas(histName.c_str(), histTitle.c_str(), 0, 0, 800, 400);
  can->SetGrid(1, 0);
  can->SetLeftMargin(0.20);
  can->SetBottomMargin(0.15);

  sStr.str("");
  TLatex m_Text;
  sStr << "The Total GlobalCRC Errors, in Drawer " << m_PartitionsLabels[ros] << std::setfill('0')
      << std::setw(2) << drawer + 1 << ", are: " << globalErrCount[ros][drawer];

  hist_error[ros][drawer]->SetStats(0);
  gStyle->SetPalette(51);
  hist_error[ros][drawer]->SetMarkerColor(kRed);
  hist_error[ros][drawer]->SetMarkerSize(1.2);
  hist_error[ros][drawer]->Draw("colztext");
  m_Text.DrawLatex(-3.6, -1.6, sStr.str().c_str());
  can->Update();

  if (m_savePng) {
    can->Print(TString(moduleName + "_hist_error.png"), "png");
  }
  if (m_savePs) {
    can->Print(TString(moduleName + "_hist_error.ps"), "ps");
  }
  delete can;
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragMonTool::drawGlobalHist() {
  /*---------------------------------------------------------*/

  TCanvas* can = new TCanvas("canaux", "canaux", 0, 0, 800, 400);

  can->SetGrid();
  can->SetLeftMargin(0.20);
  //hist_global[0]->SetBit(TH1::kCanRebin);
  hist_global[0]->SetStats(0);
  gStyle->SetPalette(51);
  hist_global[0]->SetMarkerColor(kRed);
  hist_global[0]->SetMarkerSize(1.2);
  hist_global[0]->Draw("colztext");
  can->Update();

  if (m_savePng) {
    can->Print("Global_Errors_part1.png", "png");
  }
  if (m_savePs) {
    can->Print("Global_Errors_part1.ps", "ps");
  }

  can->Clear();
  can->SetGrid();
  can->SetLeftMargin(0.20);
  //hist_global[1]->SetBit(TH1::kCanRebin);
  hist_global[1]->SetStats(0);
  gStyle->SetPalette(51);
  hist_global[1]->SetMarkerColor(kRed);
  hist_global[1]->SetMarkerSize(1.2);
  hist_global[1]->Draw("colztext");
  can->Update();

  if (m_savePng) {
    can->Print("Global_Errors_part2.png", "png");
  }
  if (m_savePs) {
    can->Print("Global_Errors_part2.ps", "ps");
  }
  delete can;

}

/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
StatusCode TileDQFragMonTool::procHistograms() {
  /*---------------------------------------------------------*/

  if (endOfRun && !m_doOnline) {
    ATH_MSG_INFO( "in procHistograms..." );

    updateHistograms();

    if (m_doPlots) {
      for (int ros = 0; ros < 4; ros++) {
        for (int drawer = 0; drawer < 64; ++drawer) {
          drawErrHist(ros, drawer);
        }
      }
      drawGlobalHist();
    }

  }

  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragMonTool::fillMasking()
/*---------------------------------------------------------*/
{
  bool sp_EB; // special treatment of EBA15, EBC18
  for (int ros = 0; ros < 4; ros++) { //LF careful ros is defined in range 1-4, but here goes in range 0-3.
    for (int drawer = 0; drawer < 64; ++drawer) {
      unsigned int idx = TileCalibUtils::getDrawerIdx(ros + 1, drawer);
      for (int ch = 0; ch < 48; ch += 3) {
        TileBchStatus status0 = m_tileBadChanTool->getAdcStatus(idx, ch, 1);
        status0 += m_tileBadChanTool->getAdcStatus(idx, ch, 0);
        TileBchStatus status1 = m_tileBadChanTool->getAdcStatus(idx, ch + 1, 1);
        status1 += m_tileBadChanTool->getAdcStatus(idx, ch + 1, 0);
        TileBchStatus status2 = m_tileBadChanTool->getAdcStatus(idx, ch + 2, 1);
        status2 += m_tileBadChanTool->getAdcStatus(idx, ch + 2, 0);

        if ((ros == 2 && drawer == 14) || (ros == 3 && drawer == 17))
          sp_EB = true; // EBA15, EBC18
        else
          sp_EB = false;

        if (status0.isBad() && status1.isBad() && status2.isBad()) {
          hist_error[ros][drawer]->Fill(ch / 3, 12, 1);
        } else if ((ros > 1) && ((ch == 18 && !sp_EB) || ch == 33)) { //disconnected channels for EBs
          if (status2.isBad()) hist_error[ros][drawer]->Fill(ch / 3, 12, 1);
        } else if ((ros < 2) && (ch == 30)) { //disconnected channels for LBs
          if (status2.isBad()) hist_error[ros][drawer]->Fill(ch / 3, 12, 1);
        } else if ((ros < 2) && (ch == 42)) { //disconnected channels for LBs
          if (status0.isBad() && status2.isBad()) hist_error[ros][drawer]->Fill(ch / 3, 12, 1);
        } else if ((ros > 1) && (ch == 24 || ch == 27 || ch == 42 || ch == 45)) { // void DMUs for EBs
          hist_error[ros][drawer]->Fill(ch / 3, 12, 1);
        } else if (sp_EB && (ch == 0)) { // void DMU 0 for EBA15, EBC18
          hist_error[ros][drawer]->Fill(ch / 3, 12, 1);
        } else if (sp_EB && (ch == 3)) { // disconnected PMT of DMU 1 for EBA15, EBC18
          if (status1.isBad() && status2.isBad()) hist_error[ros][drawer]->Fill(ch / 3, 12, 1);
        }
      } //chan loop
    } // drawer loop
  } // ros loop
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
bool TileDQFragMonTool::CheckhasErr(int ros, int drawer, int dmu) {
/*---------------------------------------------------------*/

  if (ros < 2) {
    if (hist_error[ros][drawer]->GetBinContent(dmu + 1, 13) > 0) return false;
  } else {
    if ((dmu == 8) || (dmu == 9) || (dmu == 14) || (dmu == 15)
        || (hist_error[ros][drawer]->GetBinContent(dmu + 1, 13) > 0))
      return false;
    else if (((drawer == 14) && (ros == 2)) || ((drawer == 17) && (ros == 3))) {
      if (dmu == 0) return false;
    }
  }

  return true;

}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
bool TileDQFragMonTool::isModuleDCSgood(int partition, int drawer) const {
/*---------------------------------------------------------*/

  if (m_checkDCS) {
    TileDCSSvc::TileDCSStatus Status = m_tileDCSSvc->getDCSSTATUS(partition,drawer);
    if (Status > TileDCSSvc::WARNING) {
      return false;
    } else {
      return true;
    }
  }
  return true;
}   
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
int TileDQFragMonTool::findDrawerErrors(const TileRawChannelCollection *coll) {
/*---------------------------------------------------------*/

  uint32_t DSPBCID         = coll->getFragDSPBCID();
  uint32_t GlobalCRCErr    = coll->getFragGlobalCRC() & 0x1;
  uint32_t FE_DMUmask      = coll->getFragFEChipMask();
  uint32_t ROD_DMUmask     = coll->getFragRODChipMask();
  uint32_t BCIDErr         = coll->getFragBCID();
  uint32_t MemoryParityErr = coll->getFragMemoryPar();
  uint32_t HeaderFormatErr = coll->getFragHeaderBit();
  uint32_t HeaderParityErr = coll->getFragHeaderPar();
  uint32_t SampleFormatErr = coll->getFragSampleBit();
  uint32_t SampleParityErr = coll->getFragSamplePar();
  uint32_t SingleStrobeErr = coll->getFragSstrobe();
  uint32_t DoubleStrobeErr = coll->getFragDstrobe();

  if (DSPBCID >= 0x7FFF
      && GlobalCRCErr
      && FE_DMUmask == 0xFFFF
      && ROD_DMUmask == 0xFFFF
      && BCIDErr == 0xFFFF
      && MemoryParityErr == 0xFFFF
      && HeaderFormatErr == 0xFFFF
      && HeaderParityErr == 0xFFFF
      && SampleFormatErr == 0xFFFF
      && SampleParityErr == 0xFFFF
      && SingleStrobeErr == 0xFFFF
      && DoubleStrobeErr == 0xFFFF) {
    // drawer is off - nothing to check
    return 1;
  }

  if (GlobalCRCErr) { // global CRC error - nothing to check
    return 2;
  }

  if (BCIDErr & 0x2) { // BCID in DMU1 is bad - everything is bad 
    return 3;
  }

  int frag = coll->identify();
  if (frag > 0x2ff) { // do not count non-existing DMUs in EB
    if (frag == 0x30e || frag == 0x411) {
      BCIDErr &= 0x3cff; 
    } else {
      BCIDErr &= 0x3cfe; 
    }
  }

  int n_badMB = 0;
  while (BCIDErr) {
    if (BCIDErr & 0xF) ++n_badMB;
    BCIDErr>>=4;
  }

  if (n_badMB == 4) { // BCID errors in all 4 motherboards
      return 4;
  }
  
  return 0;
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
int TileDQFragMonTool::findChannelErrors(const TileRawChannel *rch, int & gain) {
/*---------------------------------------------------------*/

  int error = 0;

  HWIdentifier adc_id = rch->adc_HWID();
  gain = m_tileHWID->adc(adc_id);

  if (m_skipGapCells && m_tileHWID->ros(adc_id) > 2) { // choose ext.barrel
    int chan2 = m_tileHWID->channel(adc_id) >> 1; // channel number divided by 2
    if (chan2 == 0 || chan2 == 6 || chan2 == 9) { // chan 0,1, 12,13, 18,19
      return error;
    }
  }

  float amp = rch->amplitude();

  if (amp < ((gain) ? m_negAmpHG : m_negAmpLG)) {
    if (m_skipMasked && m_tileBadChanTool->getAdcStatus(adc_id).isBad()) {
      error = -1;
    } else {
      error = 1;
    }
  }

  return error;
}
/*---------------------------------------------------------*/

