/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileRawChannelMonTool.cxx
// PACKAGE:  
//
// AUTHOR:   Alexander Solodkov
//
//
// ********************************************************************

#include "TileMonitoring/TileRawChannelMonTool.h"

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileEvent/TileDigitsContainer.h" 
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileInfo.h"
#include "StoreGate/ReadHandle.h"

#include "TH1S.h"
#include "TH2S.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TGraphAsymmErrors.h"
#include "TVirtualPad.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"
#include "TF1.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>


/*---------------------------------------------------------*/
TileRawChannelMonTool::TileRawChannelMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TilePaterMonTool(type, name, parent)
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_cispar(0)
  , m_nEventsTileMon(0)
  , m_calibUnit(TileRawChannelUnit::Invalid)
  , m_drawHists(true)
  , m_tileInfo(0)

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("bookAllDrawers", m_bookAll = false);
  declareProperty("overlaphists", m_overlaphists = false);
  declareProperty("book2D", m_book2D = true);

  // run type 1 - phys, 2 - las, 4 - ped, 8 - cis
  declareProperty("runType", m_runType = 0);
  declareProperty("bigain", m_bigain = true);
  declareProperty("TileRawChannelContainer", m_contName = "TileRawChannelOpt2"); //SG RC Container
  declareProperty("TileRawChannelContainerDSP", m_contNameDSP = "TileRawChannelCnt"); //SG DSP RC Cell Container

  declareProperty("AmpQ_down_Limit", m_DownLimit = 0.7);
  declareProperty("AmpQ_up_Limit", m_UpLimit = 1.3);
  declareProperty("AmpQ_loIntegralLimit", m_lo_IntegralLimit = 0.005);
  declareProperty("AmpQ_medIntegralLimit", m_med_IntegralLimit = 0.010);
  declareProperty("AmpQ_hiIntegralLimit", m_hi_IntegralLimit = 0.015);
  declareProperty("RatioErrorBar", m_useratioerror = true);
  declareProperty("PlotDSP", m_plotDsp = false);
  declareProperty("StoreGraph", m_storeGraph = true);
  m_path = "/Tile/RawChannel"; //ROOT file directory
  declareProperty("EfitThresh", m_efitThresh = 0.30); //threshold in pC for DSP check
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("SummaryUpdateFrequency", m_summaryUpdateFrequency = 0);
  declareProperty("ResetAfterSummaryUpdate", m_resetAfterSummaryUpdate = false);
  declareProperty("DoLaserSummaryVsPMT", m_doLaserSummaryVsPMT = false);
  declareProperty("MinAmpForCorrectedTime", m_minAmpForCorrectedTime = 0.5);
  declareProperty("TileInfoName", m_infoName = "TileInfo");
  declareProperty("TileDQstatus", m_DQstatusKey = "TileDQstatus");
  declareProperty("CalibUnit", m_intCalibUnit = (int)TileRawChannelUnit::Invalid);
}

/*---------------------------------------------------------*/
TileRawChannelMonTool::~TileRawChannelMonTool()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelMonTool::initialize()
/*---------------------------------------------------------*/
{

  ATH_MSG_INFO("in initialize()");

  m_data = std::make_unique<Data>();

  CHECK(m_tileToolEmscale.retrieve());

  m_nEventsTileMon = 0;

  memset(m_data->m_timeCov, 0, sizeof(m_data->m_timeCov));
  memset(m_data->m_timeCovCorr, 0, sizeof(m_data->m_timeCovCorr));

  CHECK(TilePaterMonTool::initialize());

  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );
  if (m_tileInfo->ADCmax() == 4095) m_is12bit = true;
  
  CHECK( m_DQstatusKey.initialize() );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelMonTool::bookHists()
/*---------------------------------------------------------*/
{

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "in bookHists()" << endmsg;
    msg(MSG::DEBUG) << "Using base path " << m_path << endmsg;
  }

  if (m_intCalibUnit>=0 && m_intCalibUnit<=3) {
    m_calibUnit = (TileRawChannelUnit::UNIT)m_intCalibUnit;
  }
  if (m_calibUnit == TileRawChannelUnit::Invalid) {
    if (m_runType == PhysRun) {
      m_calibUnit = TileRawChannelUnit::CesiumPicoCoulombs;
    } else {
      m_calibUnit = TileRawChannelUnit::PicoCoulombs;
    }
  }

  if (m_bookAll) {
    for (int ros = 1; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        bookHists(ros, drawer);

        if (m_plotDsp) bookDsp(ros, drawer);

      }
    }
  }

  /*  float X_axis[48];
   float X_errors[48];
   for (int i = 0; i < 48; i++) {
   X_axis[i] = i+0.5;
   X_errors[i] = 0.;
   }

   TGraph* test = new  TGraph(48,X_axis,X_errors);
   test->SetNameTitle("LFTESTLBC01","LFTESTLBC01");
   m_rootsvc->regGraph("/HIST1/Tile/RawChannel/Summary/LFTESTBC01", test);

   TGraph* test2 = new  TGraph(48,X_axis,X_errors);
   test2->SetNameTitle("LFTESTLBC02","LFTESTLBC02");
   m_rootsvc->regGraph("/HIST1/Tile/RawChannel/Summary/LFTESTBC02", test2);
   */

  //SetBookStatus(true);


  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
void TileRawChannelMonTool::bookHists(int ros, int drawer)
/*---------------------------------------------------------*/
{
  const char *gain[6] = { "_lo", "_hi", "", " low gain", " high gain", "" };

  std::ostringstream sStr;
  std::string moduleName = TileCalibUtils::getDrawerString(ros, drawer);
  std::string subDir = moduleName;
  std::string histName, histTitle;

  ATH_MSG_DEBUG("in bookHists() for module " << moduleName);

  // for bigain run book 2 histograms per channel
  // for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (int ch = 0; ch < 48; ++ch) {

    //    int pmt=m_cabling->channel2hole(ros,ch); // 1:48, negative for non-connected channels
    //    int pmt0 = ((pmt > 0)? pmt-1 : pmt+1); // 0:47, negaitve for non-connected channels
    sStr.str("");
    sStr << std::setfill('0') << std::setw(2) << ch << std::setfill(' ');
    //    sStr << std::setfill('0') << std::setw(2) << (abs(pmt)-1) << std::setfill(' ');
    std::string sCh = sStr.str(); // 00:47, always positive

    for (int gn = mingain; gn < maxgain; ++gn) {

      if ((m_runType == CisRun) || (m_runType == CisRamp)) {

        std::string HistName[4] = { "_amp_ratio_100", "_amp_ratio_5", "_time_100", "_time_5" };
        std::string HistTitle[4] = { " amp to charge ratio for 100 pF", " amp to charge ratio for 5 pF", " time for 100 pF", " time for 5 pF" };

        for (int type = 0; type < 4; type++) {
          sStr.str("");
          sStr << moduleName << "_ch_" << sCh << gain[gn] << HistName[type];
          histName = sStr.str();

          sStr.str("");
          sStr << moduleName << " CH " << ch << gain[3 + gn] << HistTitle[type];
          histTitle = sStr.str();

          if (type < 2)
            m_data->m_hist1[ros][drawer][ch][gn & 1].push_back(book1S(subDir, histName, histTitle, 101, -0.01, 2.01));
          else
            m_data->m_hist1[ros][drawer][ch][gn & 1].push_back(book1S(subDir, histName, histTitle, 101, -101.0, 101.0));
        }

        if (m_book2D) {
          std::string Hist2DName[4] = { "_amp_vs_q_100", "_amp_vs_q_5", "_time_vs_time_100", "_time_vs_time_5" };
          std::string Hist2DTitle[4] = { " amp vs charge 100 pF", " amp vs charge 5 pF", " time vs time 100 pF", " time vs time 5 pF" };
          float factor_charge = m_is12bit ? 2. : 1.;
          float factor_adc    = m_is12bit ? 4. : 1.;
          // Below, static_cast<float> is used to avoid warnings from -Wnarrowing
          float LowX_low2D[4] = { -4., -0.5, -0.25, -0.25 };
          float HighX_low2D[4] = { 804., 50.5, 25.25, 25.25 };
          float LowX_hi2D[4] = { static_cast<float>(-0.0625 * factor_charge), static_cast<float>(-0.0625 * factor_charge), -0.25, -0.25 };
          float HighX_hi2D[4] = { static_cast<float>(12.5625 * factor_charge), static_cast<float>(12.5625 * factor_charge), 25.25, 25.25 };
          float LowY_low2D[4] = { -25., -5.3125, -64.0, -64.0 };
          float HighY_low2D[4] = { 1025., 60.3125, 32.0, 32.0 };
          float LowY_hi2D[4] = { static_cast<float>(-25. * factor_adc), static_cast<float>(-25. * factor_adc), -64.0, -64.0 };
          float HighY_hi2D[4] = { static_cast<float>(1025. * factor_adc), static_cast<float>(1025. * factor_adc), 32.0, 32.0 };

          for (int type = 0; type < 4; type++) {

            sStr.str("");
            sStr << moduleName << "_ch_" << sCh << gain[gn] << Hist2DName[type];
            histName = sStr.str();

            sStr.str("");
            sStr << moduleName << " CH " << ch << gain[3 + gn] << Hist2DTitle[type];
            histTitle = sStr.str();

            switch (gn) {
              case 0: // low gain
                m_data->m_hist2[ros][drawer][ch][gn & 1].push_back(
                    book2S(subDir, histName, histTitle, 51, LowX_low2D[type], HighX_low2D[type], 160, LowY_low2D[type], HighY_low2D[type]));
                break;
              case 1: // high gain
                m_data->m_hist2[ros][drawer][ch][gn & 1].push_back(
                    book2S(subDir, histName, histTitle, 51, LowX_hi2D[type], HighX_hi2D[type], 160, LowY_hi2D[type], HighY_hi2D[type]));
                break;
              default: // single gain mode
                m_data->m_hist2[ros][drawer][ch][gn & 1].push_back(
                    book2S(subDir, histName, histTitle, 51, LowX_hi2D[type], HighX_hi2D[type], 160, LowY_hi2D[type], HighY_hi2D[type]));
            }
          } //loop over type hist
        } // if(book2D)

      } else { // not CIS run

        sStr.str("");
        sStr << moduleName << "_ch_" << sCh << gain[gn] << "_amp";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " CH " << ch << gain[3 + gn] << " amp";
        histTitle = sStr.str();

        if (m_runType == PedRun) {
          switch (gn) {
            case 0: // low gain
              m_data->m_hist1[ros][drawer][ch][gn].push_back(book1S(subDir, histName, histTitle, 101, -10.1, 10.1));
              break;
            case 1: // high gain
              m_data->m_hist1[ros][drawer][ch][gn].push_back(book1S(subDir, histName, histTitle, 101, -0.404, 0.404));
              break;
            default: // single gain mode
              m_data->m_hist1[ros][drawer][ch][gn & 1].push_back(book1S(subDir, histName, histTitle, 101, -10.1, 10.1));
          }
        } else {
          switch (gn) {
            case 0: // low gain
              if (m_overlaphists) {
                const Int_t nlg1 = 49;
                const Int_t nlg2 = 500;
                const Int_t nlg3 = 1027;
                Double_t xlgbin[nlg1 + nlg2 + nlg3 + 1];
                for(Int_t i = 0; i <= nlg1; ++i)
                  xlgbin[i] = -50.5+1.0*i;
                for(Int_t i = 1; i <= nlg2; ++i)
                  xlgbin[i + nlg1] = -1.5 + 0.05 * i;
                for(Int_t i = 1; i <= nlg3; ++i)
                  xlgbin[i + nlg1 + nlg2] = 23.5 + 1.0 * i;
                m_data->m_hist1[ros][drawer][ch][gn].push_back(book1Sx(subDir, histName, histTitle, nlg1 + nlg2 + nlg3, xlgbin));
              } else {
                m_data->m_hist1[ros][drawer][ch][gn].push_back(book1S(subDir, histName, histTitle, 1101, -50.5, 1050.5));
              }
              break;
            case 1: // high gain
              if (m_overlaphists)
                m_data->m_hist1[ros][drawer][ch][gn].push_back(book1S(subDir, histName, histTitle, 500, -1.5, 23.5));
              else
                m_data->m_hist1[ros][drawer][ch][gn].push_back(book1S(subDir, histName, histTitle, 826, -1.01, 15.51));
              break;
            default: // single gain mode
              if (m_runType == PhysRun) {
                m_data->m_hist1[ros][drawer][ch][gn & 1].push_back(book1S(subDir, histName, histTitle, 206, -0.55, 20.05));
              } else {
                m_data->m_hist1[ros][drawer][ch][gn & 1].push_back(book1S(subDir, histName, histTitle, 801, -0.5, 800.5));
              }
          }
        }

        sStr.str("");
        sStr << moduleName << "_ch_" << sCh << gain[gn] << "_time";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " CH " << ch << gain[3 + gn] << " time";
        histTitle = sStr.str();

        m_data->m_hist1[ros][drawer][ch][gn & 1].push_back(book1S(subDir, histName, histTitle, 201, -100.5, 100.5));
        //Lukas
        sStr.str("");
        sStr << moduleName << "_ch_" << sCh << gain[gn] << "_time_corr";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " CH " << ch << gain[3 + gn] << " time_corr";
        histTitle = sStr.str();

        m_data->m_hist1[ros][drawer][ch][gn & 1].push_back(book1S(subDir, histName, histTitle, 201, -100.5, 100.5));
        //Lukas
      }
    }
  }

  if (m_summaryUpdateFrequency > 0) bookSummaryHistograms(ros, drawer);
}

/*---------------------------------------------------------*/
void TileRawChannelMonTool::bookDsp(int ros, int drawer)
/*---------------------------------------------------------*/
{
  const char *gain[6] = { "_lo", "_hi", "", " low gain", " high gain", "" };


  std::ostringstream sStr;

  std::string moduleName = TileCalibUtils::getDrawerString(ros, drawer);
  std::string subDir = moduleName;
  std::string histName, histTitle;

  ATH_MSG_DEBUG("in bookDSP() for module " << moduleName);

  // for bigain run book 2 histograms per channel
  // for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (int gn = mingain; gn < maxgain; ++gn) {
    std::string finalsubDir = "Summary";
    sStr.str("");
    sStr << moduleName << gain[gn] << "_summary_chi2_amp";
    std::string finalhistName = sStr.str();

    sStr.str("");
    sStr << moduleName << gain[3 + gn] << " Summary chi2 versus amp";
    std::string finalhistTitle = sStr.str();

    switch (gn) {
      case 0: // low gain
        m_data->m_finalHistDsp2[ros][drawer][gn].push_back(book2F(finalsubDir, finalhistName, finalhistTitle, 200, -45.1, 855.1, 16, 0., 16.));
        break;
      case 1: // high gain
        m_data->m_finalHistDsp2[ros][drawer][gn].push_back(book2F(finalsubDir, finalhistName, finalhistTitle, 150, -7.0, 12.0, 16, 0., 16.));
        break;
      default: // single gain mode
        if (m_runType == PhysRun) {
          m_data->m_finalHistDsp2[ros][drawer][gn & 1].push_back(book2F(subDir, finalhistName, finalhistTitle, 150, -7.005, 7.005, 16, 0., 16.));

        } else {
          m_data->m_finalHistDsp2[ros][drawer][gn & 1].push_back(book2F(subDir, finalhistName, finalhistTitle, 200, -45.1, 855.1, 16, 0., 16.));
        }
    }

  }

  for (int ch = 0; ch < 48; ++ch) {

    //    int pmt=m_cabling->channel2hole(ros,ch); // 1:48, negative for non-connected channels
    //    int pmt0 = ((pmt > 0)? pmt-1 : pmt+1); // 0:47, negaitve for non-connected channels
    sStr.str("");
    //    sStr << std::setfill('0') << std::setw(2) << (abs(pmt)-1) << std::setfill(' ');
    sStr << std::setfill('0') << std::setw(2) << ch << std::setfill(' ');
    std::string sCh = sStr.str(); // 00:47, always positive

    for (int gn = mingain; gn < maxgain; ++gn) {

      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_dsp_amp";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " Dsp Amp";
      histTitle = sStr.str();

      switch (gn) {
        case 0: // low gain
          m_data->m_histDsp1[ros][drawer][ch][gn].push_back(book1F(subDir, histName, histTitle, 491, -50.5, 1049.34));
          break;
        case 1: // high gain
          m_data->m_histDsp1[ros][drawer][ch][gn].push_back(book1F(subDir, histName, histTitle, 413, -1.01, 15.51));
          break;
        default: // single gain mode
          if (m_runType == PhysRun) {
            m_data->m_histDsp1[ros][drawer][ch][gn & 1].push_back(book1F(subDir, histName, histTitle, 100, -0.55, 20.05));
          } else {
            m_data->m_histDsp1[ros][drawer][ch][gn & 1].push_back(book1F(subDir, histName, histTitle, 200, -0.5, 800.5));
          }
      }

      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_dsp_time";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " Dsp Time";
      histTitle = sStr.str();

      m_data->m_histDsp1[ros][drawer][ch][gn & 1].push_back(book1F(subDir, histName, histTitle, 101, -100.5, 100.5));

      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_dsp_chi2";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " Dsp Chi2";
      histTitle = sStr.str();

      m_data->m_histDsp1[ros][drawer][ch][gn & 1].push_back(book1F(subDir, histName, histTitle, 16, -0.5, 15.5));

      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_dsp-fit_amp_diff";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " Dsp-OF Amp difference";
      histTitle = sStr.str();

      switch (gn) {
        case 0: // low gain
          m_data->m_histDsp1[ros][drawer][ch][gn].push_back(book1F(subDir, histName, histTitle, 404, -1.01, 1.01));
          break;
        case 1: // high gain
          m_data->m_histDsp1[ros][drawer][ch][gn].push_back(book1F(subDir, histName, histTitle, 404, -1.01, 1.01));
          break;
        default: // single gain mode
          if (m_runType == PhysRun) {
            m_data->m_histDsp1[ros][drawer][ch][gn & 1].push_back(book1F(subDir, histName, histTitle, 404, -1.01, 1.01));
          } else {
            m_data->m_histDsp1[ros][drawer][ch][gn & 1].push_back(book1F(subDir, histName, histTitle, 404, -1.01, 1.01));
          }
      }
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_dsp-fit_time_diff";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " Dsp-OF Time diff";
      histTitle = sStr.str();

      m_data->m_histDsp1[ros][drawer][ch][gn & 1].push_back(book1F(subDir, histName, histTitle, 101, -2.02, 2.02));

      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_chi2_amp";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " Dsp Chi2 versus Amp";
      histTitle = sStr.str();

      switch (gn) {
        case 0: // low gain
          m_data->m_histDsp2[ros][drawer][ch][gn].push_back(book2F(subDir, histName, histTitle, 200, -45.1, 855.1, 16, 0., 16.));
          break;
        case 1: // high gain
          m_data->m_histDsp2[ros][drawer][ch][gn].push_back(book2F(subDir, histName, histTitle, 150, -7.0, 12.0, 16, 0., 16.));
          break;
        default: // single gain mode
          if (m_runType == PhysRun) {
            m_data->m_histDsp2[ros][drawer][ch][gn & 1].push_back(book2F(subDir, histName, histTitle, 150, -7.005, 7.005, 16, 0., 16.));
          } else {
            m_data->m_histDsp2[ros][drawer][ch][gn & 1].push_back(book2F(subDir, histName, histTitle, 200, -45.1, 855.1, 16, 0., 16.));
          }
      }

    } //end for over gains	
  } // end for over chans
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelMonTool::fillHists()
/*---------------------------------------------------------*/
{

  ATH_MSG_DEBUG("in fillHists()");

  if (m_nEventsTileMon % 1000 == 0) ATH_MSG_INFO(m_nEventsTileMon<<" events processed so far");
  // array of 16 CIS parameters
  const TileDQstatus* dqStatus = SG::makeHandle (m_DQstatusKey).get();
  m_cispar = dqStatus->cispar();
  ++m_nEventsTileMon;

  m_efitMap.clear();
  m_tfitMap.clear();

  //ndulchin: start filter code
  const TileDigitsContainer* DigitsCnt;
  CHECK(evtStore()->retrieve(DigitsCnt, "TileDigitsCnt"));

  TileDigitsContainer::const_iterator collItr2 = DigitsCnt->begin();
  TileDigitsContainer::const_iterator lastColl2 = DigitsCnt->end();

  for (; collItr2 != lastColl2; ++collItr2) {
    TileDigitsCollection::const_iterator digitsItr = (*collItr2)->begin();
    TileDigitsCollection::const_iterator lastDigits = (*collItr2)->end();

    if (digitsItr != lastDigits) {

      HWIdentifier adc_id = (*digitsItr)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);
      std::vector<uint32_t> headerVec = (*collItr2)->getFragChipHeaderWords();
      int headsize = headerVec.size();
      if (headsize > 16) headsize = 16;

      int fragId = (*collItr2)->identify();
      if (!std::binary_search(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end(), fragId)) {

        for (int dmu = 0; dmu < headsize; dmu++) {
          m_corrup[ros][drawer][0][dmu] = checkDmuHeader(&headerVec, dmu);
          m_corrup[ros][drawer][1][dmu] = checkDmuHeader(&headerVec, dmu);
        }

      } else {
        for (int dmu = 0; dmu < headsize; dmu++) {
          m_corrup[ros][drawer][0][dmu] = false;
          m_corrup[ros][drawer][1][dmu] = false;
        }
      }

      for (int dmu = headsize; dmu < 16; dmu++) {
        m_corrup[ros][drawer][0][dmu] = false;
        m_corrup[ros][drawer][1][dmu] = false;
      }

    }
  }

  //if (!isErr)  
  //ndulchin: end Filter code

  const TileRawChannelContainer* RawChannelCnt;
  CHECK(evtStore()->retrieve(RawChannelCnt, m_contName));

  TileRawChannelUnit::UNIT RChUnit = RawChannelCnt->get_unit();

  //Lukas
  double avgTimePerPart[5], sumTimeCh[5];
  int nCh[5];
  for (int iros = 0; iros < 5; iros++) {
    avgTimePerPart[iros] = 0;
    sumTimeCh[iros] = 0;
    nCh[iros] = 0;
  }  //for iros
     //Lukas

  for (int k = 0; k < 2; k++) {	//Lukas

    TileRawChannelContainer::const_iterator collItr = RawChannelCnt->begin();
    TileRawChannelContainer::const_iterator lastColl = RawChannelCnt->end();

    for (; collItr != lastColl; ++collItr) {

      TileRawChannelCollection::const_iterator chItr = (*collItr)->begin();
      TileRawChannelCollection::const_iterator lastCh = (*collItr)->end();

      if (chItr != lastCh) {

        HWIdentifier adc_id = (*chItr)->adc_HWID();
        int ros = m_tileHWID->ros(adc_id);
        int drawer = m_tileHWID->drawer(adc_id);

        if (m_data->m_hist1[ros][drawer][0][0].size() == 0) {
          //        m_bigain = (dqStatus->calibMode() == 1); // true if bi-gain run
          // we fill both high and low gain plots
          m_bigain = true;
          if (k == 0) bookHists(ros, drawer); //Lukas
        }

        std::vector<double> efitVec(m_bigain ? 96 : 48, 0.), tfitVec(m_bigain ? 96 : 48, 0.); //I need these variables later for comparison with dsp

        for (; chItr != lastCh; ++chItr) {

          const TileRawChannel * rch = (*chItr);

          adc_id = rch->adc_HWID();
          unsigned int chan = m_tileHWID->channel(adc_id);
          int adc = m_tileHWID->adc(adc_id);
          int gain = (m_bigain) ? adc : 0; // ignore gain in monogain run
          unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

          if ((m_cispar[8] == 0) /* || ( m_cispar[8] == abs(m_cabling->channel2hole(ros,chan)) )*/) { // channel is fired

            int cap = (m_cispar[7] > 10) ? 0 : 1; // 100 pF or 5 pF

            double amp = rch->amplitude();
            if (RChUnit != m_calibUnit) {
              //Put everything in PicoCoulomb by default for all run types
              //For Laser and Physcs calibrate in CesiumPicoCoulomb for all channels,
              // but the MBTS channels, for which we keep the calibration in PicoCoulombCesium pC for consistency
              //(no Cs calibration is possible)
              if (m_calibUnit == TileRawChannelUnit::CesiumPicoCoulombs) {
                if (ros < 3) //if LB then it's ok CesiumPicoCoulomb
                    {
                  amp = m_tileToolEmscale->channelCalib(drawerIdx, chan, adc, amp, RChUnit, TileRawChannelUnit::CesiumPicoCoulombs);
                } else // check that they are not MBTS channel
                {
                  int index, pmt;
                  /*Identifier cell_id =*/rch->cell_ID_index(index, pmt);
                  if (index < -1) // MBTS channel
                      {
                    amp = m_tileToolEmscale->channelCalib(drawerIdx, chan, adc, amp, RChUnit, TileRawChannelUnit::PicoCoulombs);
                  } else  //not MBTS channel
                  {
                    amp = m_tileToolEmscale->channelCalib(drawerIdx, chan, adc, amp, RChUnit, TileRawChannelUnit::CesiumPicoCoulombs);
                  }
                }
              }  //close if on CesiumPicoCoulomb units
              else { //calibrate in PicoCoulomb
                amp = m_tileToolEmscale->channelCalib(drawerIdx, chan, adc, amp, RChUnit, m_calibUnit);
              }
            } // no need to calibrate

            efitVec[chan + gain * 48] = amp;
            double time = rch->time();
            double timeCorr = 0;	//Lukas
            tfitVec[chan + gain * 48] = time;

            if (!m_corrup[ros][drawer][gain][chan / 3]) {
              if ((m_runType == CisRun) || (m_runType == CisRamp)) {
                if (k == 0) {	//Lukas
                  double charge = 0.;
                  if (m_cispar[6] < 1024) charge = m_cispar[6] * m_cispar[7] * (2. * 4.096 / 1023.);
                  double timeInj = m_cispar[5] * 0.104;

                  if (m_book2D) {
                    m_data->m_hist2[ros][drawer][chan][gain][cap]->Fill(charge, amp);
                  }

                  if (charge > 1.0) { //! ignore first step with charge ~0.5 pCb
                    //! apply default ADC->pCb const to get ratio around 1
                    // why ? double ratio = amp * ( (gain == 0) ? 800. : 12.5) / 1023.0 / charge;
                    double ratio = amp / charge;
                    //! shift upper half back by 25 ns to get one straight line instead of /|/
                    //if (m_cispar[5]>=140) timeInj -= 25.0;

                    if ((adc == 1 && charge > 11.5) || // 11.5 is safe upper limit
                        (adc == 0 && cap == 0 && charge < 15.0) || (adc == 0 && cap == 1 && charge < 10.0) || (timeInj > -10.5 && timeInj < -9.5)) {
                      //! do nothing in case of overflow in high gain
                      //! do nothing in case of small signals in low gain
                      //! do nothing for timeinj near the time slice change
                    } else {

                      m_data->m_hist1[ros][drawer][chan][gain][0 + cap]->Fill(ratio);
                      m_data->m_hist1[ros][drawer][chan][gain][2 + cap]->Fill(time);
                      if (m_book2D) {
                        m_data->m_hist2[ros][drawer][chan][gain][cap + 2]->Fill(timeInj, time);
                      }

                      /*TimeCov[ros][drawer][chan][gain][cap][0] += timeInj;
                       TimeCov[ros][drawer][chan][gain][cap][1] += timeInj*timeInj;
                       TimeCov[ros][drawer][chan][gain][cap][2] += time;
                       TimeCov[ros][drawer][chan][gain][cap][3] += time*time;
                       TimeCov[ros][drawer][chan][gain][cap][4] += timeInj*time;*/
                      ++m_data->m_timeCov[ros][drawer][chan][gain][cap][5];
                    }
                  }
                }		  //if k==0 //Lukas
              } else { // not CisRun
                if (k == 0) { //Lukas
                  m_data->m_hist1[ros][drawer][chan][gain][0]->Fill(amp, 1.0);
                  if (time != 0) { // we don't fill the time when it is exactly 0, which is a conventional value to say that it is not
                    // calculated when the difference between the max(samples)-min(samples)< threshold
                    m_data->m_hist1[ros][drawer][chan][gain][1]->Fill(time, 1.0);
                  }

                  m_data->m_timeCov[ros][drawer][chan][gain][0][0] += amp;
                  m_data->m_timeCov[ros][drawer][chan][gain][0][1] += amp * amp;
                  m_data->m_timeCov[ros][drawer][chan][gain][0][2] += time;
                  m_data->m_timeCov[ros][drawer][chan][gain][0][3] += time * time;
                  //TimeCov[ros][drawer][chan][gain][0][4] += amp*time;
                  ++m_data->m_timeCov[ros][drawer][chan][gain][0][5];
                  /*
                   //Lukas
                   if((ros==3 && drawer==14 && (chan==12 || chan==13 || chan==18 || chan==19))||(ros==4 && drawer==17 && (chan==12 || chan==13 || chan==18 || chan==19))){
                   }else if((ros==3 || ros==4) && (chan==0 || chan==1 || chan==12 || chan==13)){
                   }else{
                   sumTimeCh[ros][gain]+=time;
                   nCh[ros][gain]+=1;
                   }
                   //Lukas
                   */
                  //Lukas
                  if (isDisconnected(ros, drawer, chan) || amp < m_minAmpForCorrectedTime) continue;
                  if ((ros == 3 || ros == 4)
                      && (chan == 0 || chan == 1 || chan == 2 || chan == 3 || chan == 4 || chan == 5 || chan == 12 || chan == 13 || chan == 18
                          || chan == 19)) {
                  } else {
                    sumTimeCh[ros] += time;
                    nCh[ros] += 1;
                  }
                  //Lukas

                }			     //if k==0 //Lukas
                else if (k == 1) { //Lukas
                  if (isDisconnected(ros, drawer, chan) || amp < m_minAmpForCorrectedTime) continue;

                  timeCorr = time - avgTimePerPart[ros];			//Lukas
                  m_data->m_hist1[ros][drawer][chan][gain][2]->Fill(timeCorr, 1.0);		//Lukas

                  m_data->m_timeCovCorr[ros][drawer][chan][gain][0][0] += timeCorr;		//Lukas
                  m_data->m_timeCovCorr[ros][drawer][chan][gain][0][1] += timeCorr * timeCorr;		//Lukas
                  ++m_data->m_timeCovCorr[ros][drawer][chan][gain][0][2];			//Lukas

                }			//k==1 //Lukas
              } // end of nonCisRun
            }

          } else { // if channel is not fired
          }

        } // loop over channels
        if (k == 0) { //Lukas
          if (m_plotDsp) {
            m_efitMap.insert(make_pair(ros * 100 + drawer, efitVec)); // needed later for dspreco comparison
            m_tfitMap.insert(make_pair(ros * 100 + drawer, tfitVec)); // needed later for dspreco comparison
          }
        } //if k==0 //Lukas
      }
    }
    //Lukas
    if (k == 0) {
      for (int iros = 0; iros < 5; iros++) {
        if (nCh[iros] != 0) {
          avgTimePerPart[iros] = sumTimeCh[iros] / nCh[iros];
        } else {
          avgTimePerPart[iros] = 0;
        }
        sumTimeCh[iros] = 0;
        nCh[iros] = 0;
      } //for
    } //if k==0
      //Lukas
  } // loop over k //Lukas

  if (m_plotDsp) {
    if (fillDsp(m_efitMap, m_tfitMap).isFailure()) ATH_MSG_WARNING("Error filling DSP histograms");
  }


  if ((m_summaryUpdateFrequency > 0) && (m_nEventsTileMon % m_summaryUpdateFrequency == 0)) {
    m_drawHists = false;
    CHECK(fillSummaryHistograms());
    if (m_resetAfterSummaryUpdate) resetSummaryHistograms();
  }
  


  ////ndulchin: ends filter  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelMonTool::fillDsp(std::map<int, std::vector<double> > &efitMap, std::map<int, std::vector<double> > &tfitMap)
/*---------------------------------------------------------*/
{

  ATH_MSG_DEBUG("in fillDsp()");

  const TileRawChannelContainer* RawChannelCnt;
  CHECK(evtStore()->retrieve(RawChannelCnt, m_contNameDSP));

  TileRawChannelUnit::UNIT RChUnit = RawChannelCnt->get_unit();

  TileRawChannelContainer::const_iterator collItr = RawChannelCnt->begin();
  TileRawChannelContainer::const_iterator lastColl = RawChannelCnt->end();

  for (; collItr != lastColl; ++collItr) {

    TileRawChannelCollection::const_iterator chItr = (*collItr)->begin();
    TileRawChannelCollection::const_iterator lastCh = (*collItr)->end();

    if (chItr != lastCh) {

      HWIdentifier adc_id = (*chItr)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);

      if (m_data->m_histDsp1[ros][drawer][0][0].size() == 0) {
        m_bigain = true;
        bookDsp(ros, drawer);
      }

      for (; chItr != lastCh; ++chItr) {

        const TileRawChannel * rch = (*chItr);

        adc_id = rch->adc_HWID();
        unsigned int chan = m_tileHWID->channel(adc_id);
        int adc = m_tileHWID->adc(adc_id);
        int gain = (m_bigain) ? adc : 0; // ignore gain in monogain run
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

        if ((m_cispar[8] == 0) /* || ( m_cispar[8] == abs(m_cabling->channel2hole(ros,chan)) )*/
             && !m_corrup[ros][drawer][gain][chan / 3]) { // channel is fired and not corrupted
 
          double amp = rch->amplitude();
          if (RChUnit != m_calibUnit) {
            if (m_calibUnit != TileRawChannelUnit::CesiumPicoCoulombs) { //We put everythin in the same unit
              amp = m_tileToolEmscale->channelCalib(drawerIdx, chan, adc, amp, RChUnit, m_calibUnit);
            } else { // it is =TileRawChannelUnit::CesiumPicoCoulombs, careful with MBTS channels
              if (ros < 3) //if LB then it's ok CesiumPicoCoulomb
                  {
                amp = m_tileToolEmscale->channelCalib(drawerIdx, chan, adc, amp, RChUnit, TileRawChannelUnit::CesiumPicoCoulombs);
                //conversion factor if of the order of 1000MeV=1.05pC
              } else // check that they are not MBTS channel
              {
                int index, pmt;
                /*Identifier cell_id =*/rch->cell_ID_index(index, pmt);
                if (index < -1) // MBTS channel
                    {
                  amp = m_tileToolEmscale->channelCalib(drawerIdx, chan, adc, amp, RChUnit, TileRawChannelUnit::PicoCoulombs);
                } else  //not MBTS channel
                {
                  amp = m_tileToolEmscale->channelCalib(drawerIdx, chan, adc, amp, RChUnit, TileRawChannelUnit::CesiumPicoCoulombs);
                }
              }
            } // end if CesiumPicoCouloumb
          }

          double time = rch->time();
          double chi2 = rch->quality();

          m_data->m_histDsp1[ros][drawer][chan][gain][Edsp]->Fill(amp, 1.0);
          m_data->m_histDsp1[ros][drawer][chan][gain][Tdsp]->Fill(time, 1.0);
          m_data->m_histDsp1[ros][drawer][chan][gain][chi2dsp]->Fill(chi2, 1.0);
          m_data->m_histDsp2[ros][drawer][chan][gain][0]->Fill(amp, chi2, 1.0);
          m_data->m_finalHistDsp2[ros][drawer][gain][0]->Fill(amp, chi2, 1.0);

          std::map<int, std::vector<double> >::iterator it = efitMap.find(ros * 100 + drawer);
          if (it != efitMap.end()) {
            double efit = (*it).second.at(chan + gain * 48);
            it = tfitMap.find(ros * 100 + drawer);
            double tfit = (*it).second.at(chan + gain * 48);
            //convert from pC to ADC counts
            if (TMath::Abs(efit) > m_efitThresh) { // fill the histogram only if the efit is above threshold
              m_data->m_histDsp1[ros][drawer][chan][gain][Edsp_fit]->Fill((amp - efit) / efit, 1.0);
            }

            if (tfit != 0.) {
              m_data->m_histDsp1[ros][drawer][chan][gain][Tdsp_fit]->Fill((time - tfit), 1.0);
            }

          }
        }

      } // loop over channels

    }
  }
  return StatusCode::SUCCESS;
}




/*---------------------------------------------------------*/
void TileRawChannelMonTool::bookSummaryHistograms(int ros, int drawer)
/*---------------------------------------------------------*/
{


  const char *gain[6] = { "_lo", "_hi", "", " low gain", " high gain", "" };

  // for bigain run book 2 histograms per channel
  // for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  std::ostringstream sStr;
  
  if ((m_runType == CisRun) || (m_runType == CisRamp)) {

    const char *HistName[6] = { "_tslope", "_toffset", "_qratio", " Time slope", " Time offset", " Amp/Q ratio" };
    const char *CapName[4] = { "_100", "_5", " 100 pF", " 5 pF" };


    std::string moduleName = TileCalibUtils::getDrawerString(ros, drawer);
    std::string subDir = "Summary";
    std::string histName, histTitle;
    
    for (int gn = mingain; gn < maxgain; ++gn) {
      int adc = gn & 1;
      for (int cap = 0; cap < 2; ++cap) {
        for (int type = 0; type < 3; ++type) {

          sStr.str("");
          sStr << moduleName << gain[gn] << HistName[type] << CapName[cap];
          histName = sStr.str();
          sStr.str("");
          sStr << moduleName << gain[3 + gn] << HistName[3 + type] << CapName[2 + cap];
          histTitle = sStr.str();
          m_data->m_finalHist1[ros][drawer][adc][cap].push_back(book1F(subDir, histName, histTitle, 48, 0, 48));
        }
      } //end of loop over capacitors 
    } //end of loop over gn  

  } else { // not CisRun
    const char *HistName[10] = { "_amp", "_rms", "_sigma", "_time", "_time_corr", " Amplitude", " RMS of amplitude", " Sigma amp from Gaussian fit",
        " Average time and RMS", " Average time corr. and RMS" };
    //Lukas

    std::string moduleName = TileCalibUtils::getDrawerString(ros, drawer);
    std::string subDir = "Summary";
    std::string histName, histTitle;
    
    for (int gn = mingain; gn < maxgain; ++gn) {

      int adc = gn & 1;

      //for (int type = 0; type < 4; ++type) {
      for (int type = 0; type < 5; ++type) {	//Lukas

        sStr.str("");
        sStr << moduleName << gain[gn] << HistName[type];
        histName = sStr.str();
        sStr.str("");
        //sStr << moduleName << gain[3+gn] << HistName[4+type];
        sStr << moduleName << gain[3 + gn] << HistName[5 + type];	//Lukas
        histTitle = sStr.str();
        m_data->m_finalHist1[ros][drawer][adc][0].push_back(book1F(subDir, histName, histTitle, 48, 0.0, 48.0));

        std::string hTitle(histTitle);

        if (m_doLaserSummaryVsPMT && m_runType == LasRun) {

          hTitle += " (Even PMTs)";
          m_data->m_summaryPmts[ros][drawer][adc][0].push_back(book1F(subDir + "/pmt", histName + "_pmt_even", hTitle, 49, -0.5, 48.5));
          m_data->m_summaryPmts[ros][drawer][adc][0].back()->SetMarkerStyle(22);
          m_data->m_summaryPmts[ros][drawer][adc][0].back()->SetMarkerColor(2);
          m_data->m_summaryPmts[ros][drawer][adc][0].back()->SetMarkerSize(0.75);

          hTitle = histTitle;
          hTitle += " (Odd PMTs)";
          m_data->m_summaryPmts[ros][drawer][adc][1].push_back(book1F(subDir + "/pmt", histName + "_pmt_odd", hTitle, 49, -0.5, 48.5));
          m_data->m_summaryPmts[ros][drawer][adc][1].back()->SetMarkerStyle(23);
          m_data->m_summaryPmts[ros][drawer][adc][1].back()->SetMarkerColor(4);
          m_data->m_summaryPmts[ros][drawer][adc][1].back()->SetMarkerSize(0.75);

          if (ros < 3 && m_data->m_summaryPmts[0][drawer][adc][0].size() < (unsigned int)(type + 1)) {

            histName.replace(histName.begin(), histName.begin() + 3, "LB");
            histTitle.replace(histTitle.begin(), histTitle.begin() + 3, "LB");
            hTitle = histTitle;

            hTitle += " (LBA even PMTs + LBC odd PMTs: negative)";
            m_data->m_summaryPmts[0][drawer][adc][0].push_back(book1F(subDir + "/pmt", histName + "_pmt_LBA-even_LBC-odd", hTitle, 97, -48.5, 48.5));
            m_data->m_summaryPmts[0][drawer][adc][0].back()->SetMarkerStyle(22);
            m_data->m_summaryPmts[0][drawer][adc][0].back()->SetMarkerColor(2);
            m_data->m_summaryPmts[0][drawer][adc][0].back()->SetMarkerSize(0.75);

            hTitle = histTitle;
            hTitle += " (LBA odd PMTs + LBC even PMTs: negative)";
            m_data->m_summaryPmts[0][drawer][adc][1].push_back(book1F(subDir + "/pmt", histName + "_pmt_LBA-odd_LBC-even", hTitle, 97, -48.5, 48.5));
            m_data->m_summaryPmts[0][drawer][adc][1].back()->SetMarkerStyle(23);
            m_data->m_summaryPmts[0][drawer][adc][1].back()->SetMarkerColor(4);
            m_data->m_summaryPmts[0][drawer][adc][1].back()->SetMarkerSize(0.75);
          }
        }
      }

    } //end of loop over gn
  }

}


/*---------------------------------------------------------*/
void TileRawChannelMonTool::resetSummaryHistograms()
/*---------------------------------------------------------*/
{

  memset(m_data->m_timeCov, 0, sizeof(m_data->m_timeCov));
  memset(m_data->m_timeCovCorr, 0, sizeof(m_data->m_timeCovCorr));

  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (int ros = 1; ros < 5; ++ros) {
    for (int drawer = 0; drawer < 64; ++drawer) {
      if (m_data->m_hist1[ros][drawer][0][0].size() != 0) {
        for (int ch = 0; ch < 48; ++ch) {
          for (int gn = mingain; gn < maxgain; ++gn) {
            for (auto h : m_data->m_hist1[ros][drawer][ch][gn]) h->Reset();
            for (auto h : m_data->m_hist2[ros][drawer][ch][gn]) h->Reset();
          }
        }
      }
    }
  }
}


/*---------------------------------------------------------*/
StatusCode TileRawChannelMonTool::fillSummaryHistograms()
/*---------------------------------------------------------*/
{

  ATH_MSG_INFO("in fillFfinalHiststograms()");

  memset(m_data->m_rangeQ, 0, sizeof(m_data->m_rangeQ));

  TF1 * fit_gaus = new TF1("g", "gaus");

  // for bigain run book 2 histograms per channel
  // for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  if ((m_runType == CisRun) || (m_runType == CisRamp)) {
    for (int ros = 1; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        if (m_data->m_hist1[ros][drawer][0][0].size() != 0) {
          for (int gn = mingain; gn < maxgain; ++gn) {

            int adc = gn & 1;
            for (int cap = 0; cap < 2; ++cap) {
              for (int ch = 0; ch < 48; ++ch) {

                //                int pmt = abs(m_cabling->channel2hole(ros,ch)); // number in range [1,48]

                TF1* polyfunc = 0;
                if (m_book2D) polyfunc = GetTimeFitFunc(m_data->m_hist2[ros][drawer][ch][adc][cap + 2]);
                if (polyfunc) {
                  m_data->m_finalHist1[ros][drawer][adc][cap][0]->SetBinContent(ch + 1, polyfunc->GetParameter(1));
                  if (polyfunc->GetParError(1) > 5) {
                    m_data->m_finalHist1[ros][drawer][adc][cap][0]->SetBinError(ch + 1, 5.);
                  } else {
                    m_data->m_finalHist1[ros][drawer][adc][cap][0]->SetBinError(ch + 1, polyfunc->GetParError(1));
                  }
                  m_data->m_finalHist1[ros][drawer][adc][cap][1]->SetBinContent(ch + 1, polyfunc->GetParameter(0));
                  if (polyfunc->GetParError(0) > 5) {
                    m_data->m_finalHist1[ros][drawer][adc][cap][1]->SetBinError(ch + 1, 5.);
                  } else {
                    m_data->m_finalHist1[ros][drawer][adc][cap][1]->SetBinError(ch + 1, polyfunc->GetParError(0));
                  }

                  delete polyfunc; //important!
                }

                else {
                  m_data->m_finalHist1[ros][drawer][adc][cap][0]->SetBinContent(ch + 1, 0.);
                  m_data->m_finalHist1[ros][drawer][adc][cap][0]->SetBinError(ch + 1, 0.);
                  m_data->m_finalHist1[ros][drawer][adc][cap][1]->SetBinContent(ch + 1, 0.);
                  m_data->m_finalHist1[ros][drawer][adc][cap][1]->SetBinError(ch + 1, 0.);
                }

                TH1S * hist = m_data->m_hist1[ros][drawer][ch][adc][cap];
                int nbins = hist->GetNbinsX();
                int minbin = 1;
                for (; minbin < nbins; ++minbin) {
                  if (hist->GetBinContent(minbin) > 0.0) {
                    break;
                  }
                }
                int maxbin = nbins;
                for (; maxbin > 1; --maxbin) {
                  if (hist->GetBinContent(maxbin) > 0.0) {
                    break;
                  }
                }

                double mean = hist->GetMean();
                double xmin = hist->GetBinCenter(minbin);
                double xmax = hist->GetBinCenter(maxbin);

                //rangeErrorBar(xmin,xmax,mean);
                if (m_useratioerror) { // /use ratioerror bar or rangeerror bar ?
                  ratioErrorBar(hist, xmin, xmax, mean);
                } else {
                  rangeErrorBar(xmin, xmax, mean);
                }

                //std::cout << hist->GetName() <<" mean-xmin= "<< mean-xmin <<"\n";
                m_data->m_rangeQ[adc][cap][0][ch] = mean;
                m_data->m_rangeQ[adc][cap][1][ch] = std::max(0.0, mean - xmin);
                m_data->m_rangeQ[adc][cap][2][ch] = std::max(0.0, xmax - mean);

                m_data->m_finalHist1[ros][drawer][adc][cap][2]->SetBinContent(ch + 1, mean);
              }
            } //end of loop over capacitors 
          } //end of loop over gn 

          if(m_drawHists) {
            std::string moduleName = TileCalibUtils::getDrawerString(ros, drawer);
            drawHists(ros, drawer, moduleName); 
          } 
        }
      } //end of loop over drawer
    } //end of loop over ros

  } else { // not CisRun

    for (int ros = 1; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        if (m_data->m_hist1[ros][drawer][0][0].size() != 0) {
          for (int gn = mingain; gn < maxgain; ++gn) {

            int adc = gn & 1;
            for (int ch = 0; ch < 48; ++ch) {

              //              int pmt = abs(m_cabling->channel2hole(ros,ch)); // number in range [1,48]

              double Amp = 0.0, ErrA = 0.0, RMS = 0.0, ErrR = 0.0;
              double Time = 0.0, RMST = 0.0, Sigma = 0.0, ErrS = 0.0;
              double NEvents = m_data->m_timeCov[ros][drawer][ch][adc][0][5];

              double TimeCorr = 0.0, RMSTCorr = 0.0;				//Lukas
              double NEventsCorr = m_data->m_timeCovCorr[ros][drawer][ch][adc][0][2];	//Lukas

              if (NEvents > 0.0) {

                Amp = m_data->m_timeCov[ros][drawer][ch][adc][0][0] / NEvents;
                RMS = m_data->m_timeCov[ros][drawer][ch][adc][0][1] / NEvents - Amp * Amp;
                if (RMS > 0.0) {
                  RMS = sqrt(RMS);
                  ErrA = RMS / sqrt(NEvents);
                  ErrR = RMS / sqrt(2 * NEvents);
                } else {
                  RMS = 0.0;
                }

                Time = m_data->m_timeCov[ros][drawer][ch][adc][0][2] / NEvents;
                RMST = m_data->m_timeCov[ros][drawer][ch][adc][0][3] / NEvents - Time * Time;
                if (RMST > 0.0) {
                  RMST = sqrt(RMST);
                } else {
                  RMST = 0.0;
                }
              }

              //Lukas
              if (NEventsCorr > 0.0) {
                TimeCorr = m_data->m_timeCovCorr[ros][drawer][ch][adc][0][0] / NEventsCorr;
                RMSTCorr = m_data->m_timeCovCorr[ros][drawer][ch][adc][0][1] / NEventsCorr - TimeCorr * TimeCorr;
                if (RMSTCorr > 0.0) {
                  RMSTCorr = sqrt(RMSTCorr);
                } else {
                  RMSTCorr = 0.0;
                }
              }
              //Lukas

              if (m_data->m_hist1[ros][drawer][ch][adc][0]->GetEntries() > 0) {
                if (adc == 0 && m_overlaphists) { // We have for LB histograms with variable-width bins, not suitable for a fit
                  TH1S *h4fit=new TH1S(*(m_data->m_hist1[ros][drawer][ch][adc][0]));
                  h4fit->Scale(1,"width");
                  h4fit->Fit("g", "NQ");
                  delete(h4fit);
                } else {
                  m_data->m_hist1[ros][drawer][ch][adc][0]->Fit("g", "NQ");
                }
                Sigma = fit_gaus->GetParameter(2);
                ErrS = fit_gaus->GetParError(2);
              }

              m_data->m_finalHist1[ros][drawer][adc][0][0]->SetBinContent(ch + 1, Amp);
              m_data->m_finalHist1[ros][drawer][adc][0][0]->SetBinError(ch + 1, ErrA);
              m_data->m_finalHist1[ros][drawer][adc][0][1]->SetBinContent(ch + 1, RMS);
              m_data->m_finalHist1[ros][drawer][adc][0][1]->SetBinError(ch + 1, ErrR);
              m_data->m_finalHist1[ros][drawer][adc][0][2]->SetBinContent(ch + 1, Sigma);
              m_data->m_finalHist1[ros][drawer][adc][0][2]->SetBinError(ch + 1, ErrS);
              m_data->m_finalHist1[ros][drawer][adc][0][3]->SetBinContent(ch + 1, Time);
              m_data->m_finalHist1[ros][drawer][adc][0][3]->SetBinError(ch + 1, RMST);
              m_data->m_finalHist1[ros][drawer][adc][0][4]->SetBinContent(ch + 1, TimeCorr);	//Lukas
              m_data->m_finalHist1[ros][drawer][adc][0][4]->SetBinError(ch + 1, RMSTCorr);	//Lukas

              if (m_doLaserSummaryVsPMT && m_runType == LasRun) {

                int pmt = m_cabling->channel2hole(ros, ch);
                if (pmt < 0) continue;

                int fiber = (pmt % 2);

                m_data->m_summaryPmts[ros][drawer][adc][fiber][0]->SetBinContent(pmt + 1, Amp);
                m_data->m_summaryPmts[ros][drawer][adc][fiber][0]->SetBinError(pmt + 1, ErrA);
                m_data->m_summaryPmts[ros][drawer][adc][fiber][1]->SetBinContent(pmt + 1, RMS);
                m_data->m_summaryPmts[ros][drawer][adc][fiber][1]->SetBinError(pmt + 1, ErrR);
                m_data->m_summaryPmts[ros][drawer][adc][fiber][2]->SetBinContent(pmt + 1, Sigma);
                m_data->m_summaryPmts[ros][drawer][adc][fiber][2]->SetBinError(pmt + 1, ErrS);
                m_data->m_summaryPmts[ros][drawer][adc][fiber][3]->SetBinContent(pmt + 1, Time);
                m_data->m_summaryPmts[ros][drawer][adc][fiber][3]->SetBinError(pmt + 1, RMST);
                m_data->m_summaryPmts[ros][drawer][adc][fiber][4]->SetBinContent(pmt + 1, TimeCorr);	//Lukas
                m_data->m_summaryPmts[ros][drawer][adc][fiber][4]->SetBinError(pmt + 1, RMSTCorr);	//Lukas

                if (ros == 1) {

                  int bin = m_data->m_summaryPmts[0][drawer][adc][fiber][0]->FindBin(pmt);

                  m_data->m_summaryPmts[0][drawer][adc][fiber][0]->SetBinContent(bin, Amp);
                  m_data->m_summaryPmts[0][drawer][adc][fiber][0]->SetBinError(bin, ErrA);
                  m_data->m_summaryPmts[0][drawer][adc][fiber][1]->SetBinContent(bin, RMS);
                  m_data->m_summaryPmts[0][drawer][adc][fiber][1]->SetBinError(bin, ErrR);
                  m_data->m_summaryPmts[0][drawer][adc][fiber][2]->SetBinContent(bin, Sigma);
                  m_data->m_summaryPmts[0][drawer][adc][fiber][2]->SetBinError(bin, ErrS);
                  m_data->m_summaryPmts[0][drawer][adc][fiber][3]->SetBinContent(bin, Time);
                  m_data->m_summaryPmts[0][drawer][adc][fiber][3]->SetBinError(bin, RMST);
                  m_data->m_summaryPmts[0][drawer][adc][fiber][4]->SetBinContent(bin, TimeCorr);	//Lukas
                  m_data->m_summaryPmts[0][drawer][adc][fiber][4]->SetBinError(bin, RMSTCorr);	//Lukas

                }

                if (ros == 2) {

                  int bin = m_data->m_summaryPmts[0][drawer][adc][1 - fiber][0]->FindBin(-pmt);

                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][0]->SetBinContent(bin, Amp);
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][0]->SetBinError(bin, ErrA);
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][1]->SetBinContent(bin, RMS);
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][1]->SetBinError(bin, ErrR);
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][2]->SetBinContent(bin, Sigma);
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][2]->SetBinError(bin, ErrS);
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][3]->SetBinContent(bin, Time);
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][3]->SetBinError(bin, RMST);
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][4]->SetBinContent(bin, TimeCorr);	//Lukas
                  m_data->m_summaryPmts[0][drawer][adc][1 - fiber][4]->SetBinError(bin, RMSTCorr);	//Lukas

                }
              }

            }
          } //end of loop over gn
  
          if(m_drawHists){
            std::string moduleName = TileCalibUtils::getDrawerString(ros, drawer);
            drawHists(ros, drawer, moduleName); 
            if (m_plotDsp) {
              if (finalDsp(ros, drawer).isFailure()) ATH_MSG_WARNING("Error finalizing DSP histograms");
            }
          }

        }
      } //loop over drawer
    } //loop over ros
  }

  delete fit_gaus;

  return StatusCode::SUCCESS;
}




/*---------------------------------------------------------*/
StatusCode TileRawChannelMonTool::finalHists()
/*---------------------------------------------------------*/
{
  ATH_MSG_INFO("in finalHists()");

  if (m_summaryUpdateFrequency == 0) {
    for (int ros = 1; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        if (m_data->m_hist1[ros][drawer][0][0].size() != 0) {
          bookSummaryHistograms(ros, drawer);
        }
      }
    }
  }

  m_drawHists = true;
  CHECK(fillSummaryHistograms());


  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelMonTool::finalDsp(int ros, int drawer)
/*---------------------------------------------------------*/
{

  if (m_data->m_histDsp1[ros][drawer][0][0].size() != 0) {

    ///get algorithm and number of iterations from bsflags
    const TileRawChannelContainer* RawChannelCntDsp;

    unsigned int iteration = 0u;
    unsigned int algorithm = 0u;
    if (evtStore()->retrieve(RawChannelCntDsp, m_contNameDSP).isSuccess()) {
      uint32_t info = RawChannelCntDsp->get_bsflags() >> 24u;
      iteration = info & 3u;
      algorithm = (info >> 2u) & 1u;

      // iteration: "NI", "I1", "I2", "I3"
      // algorithm: "OF1", "OF2"
    }

    const char *alg_name[2] = { "OF1", "OF2" };
    const char * iter_name[4] = { "NI", "I1", "I2", "I3" };

    const char *part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
    const char *gain[4] = { "_lo", "_hi", " low gain", " high gain" };

    int mingain = 0;
    int maxgain = 2;

//    const char *HistName[9] = {"_dspfit_ampdiff","_dspfit_timediff","_dspamp",
//                               " Average (DSP-"," Average DSP-", " Average DSP-",
//                               " - OFF-OF2-I) divided by OFF-OF2-I Amplitudes and RMS"," - OFF-OF2-I Time and RMS", " Amplitudes and RMS"};
    const char *HistName[9] = { "_dspfit_ampdiff", "_dspfit_timediff", "_dspamp", " Avg. (DSP-", " Average DSP-", " Average DSP-",
        " - OFF-OF2-I) / OFF Amplitudes and RMS", " - OFF-OF2-I Time and RMS", " Amplitudes and RMS" };

    std::ostringstream sStr;
    sStr << part[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ');
    std::string moduleName = sStr.str();
    std::string subDir = "Summary";
    std::string histName, histTitle;

    for (int gn = mingain; gn < maxgain; ++gn) {

      int adc = gn & 1;

      for (int type = sumEdsp_fit; type < NsumDsp; ++type) {
        sStr.str("");
        sStr << moduleName << gain[gn] << HistName[type];
        histName = sStr.str();
        sStr.str("");
        sStr << moduleName << gain[2 + gn] << HistName[3 + type] << alg_name[algorithm] << "-" << iter_name[iteration] << HistName[6 + type];
        histTitle = sStr.str();
        m_data->m_finalHistDsp1[ros][drawer][adc].push_back(book1F(subDir, histName, histTitle, 48, 0.0, 48.0));
      }

      //Create hbar histograms to be used in drawDsp
      sStr.str("");
      sStr << moduleName << gain[gn] << "_dspfit_amphbar";
      histName = sStr.str();
      sStr.str("");
      sStr << moduleName << gain[2 + gn] << " (DSP-" << alg_name[algorithm] << "-" << iter_name[iteration]
          << " - OFF-OF2-I) divided by OFF Amplitudes for all chans";
      histTitle = sStr.str();
      m_data->m_hBarDsp1[ros][drawer][adc].push_back(book1F(subDir, histName, histTitle, 404, -1.01, 1.01));

      sStr.str("");
      sStr << moduleName << gain[gn] << "_dspfit_timehbar";
      histName = sStr.str();
      sStr.str("");
      sStr << moduleName << gain[2 + gn] << " (DSP-" << alg_name[algorithm] << "-" << iter_name[iteration] << " - OFF-OF2-I) Time for all chans";
      histTitle = sStr.str();
      m_data->m_hBarDsp1[ros][drawer][adc].push_back(book1F(subDir, histName, histTitle, 101, -2.02, 2.02));

      for (int ch = 0; ch < 48; ++ch) {
        //	int pmt = abs(m_cabling->channel2hole(ros,ch)); // number in range [1,48]

        if (m_data->m_histDsp1[ros][drawer][ch][adc][Edsp_fit]->GetEntries() > 0) {
          double dspmean = m_data->m_histDsp1[ros][drawer][ch][adc][Edsp]->GetMean();
          double dsprms = m_data->m_histDsp1[ros][drawer][ch][adc][Edsp]->GetRMS();
          double emean = m_data->m_histDsp1[ros][drawer][ch][adc][Edsp_fit]->GetMean();
          double erms = m_data->m_histDsp1[ros][drawer][ch][adc][Edsp_fit]->GetRMS();
          double tmean = m_data->m_histDsp1[ros][drawer][ch][adc][Tdsp_fit]->GetMean();
          double trms = m_data->m_histDsp1[ros][drawer][ch][adc][Tdsp_fit]->GetRMS();

          m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp]->SetBinContent(ch + 1, dspmean);
          m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp]->SetBinError(ch + 1, dsprms);
          m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->SetBinContent(ch + 1, emean);
          m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->SetBinError(ch + 1, erms);
          m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->SetBinContent(ch + 1, tmean);
          m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->SetBinError(ch + 1, trms);

          m_data->m_hBarDsp1[ros][drawer][adc][sumEdsp_fit]->Add(m_data->m_histDsp1[ros][drawer][ch][adc][Edsp_fit]);
          m_data->m_hBarDsp1[ros][drawer][adc][sumTdsp_fit]->Add(m_data->m_histDsp1[ros][drawer][ch][adc][Tdsp_fit]);

        }
      }
    } // close loops over chans, gain

    //    log << MSG::WARNING << "LUCA Calling drawDsp" << endreq;
    drawDsp(ros, drawer, moduleName);

  } //close if there are histograms for this Module

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelMonTool::checkHists(bool /* fromFinalize */)
/*---------------------------------------------------------*/
{

  ATH_MSG_INFO("in checkHists()");

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileRawChannelMonTool::drawHists(int ros, int drawer, const std::string& moduleName)
/*---------------------------------------------------------*/
{

  ATH_MSG_DEBUG("in drawHists()");

  int maxgain = (m_bigain) ? 2 : 1;
  double ms = (m_bigain) ? 0.75 : 1.0; // marker size

  bool do_plots = m_savePng || m_savePs || m_saveSvg;

  if ((m_runType == CisRun) || (m_runType == CisRamp)) {

    float X_axis[48];
    float X_errors[48];
    for (int i = 0; i < 48; i++) {
      X_axis[i] = i + 0.5;
      X_errors[i] = 0.;
    }

    const char *gain[6] = { "_lo", "_hi", "", " low gain", " high gain", "" };
    const char *CapName[4] = { "_100", "_5", " 100 pF", " 5 pF" };
    if (maxgain == 1) gain[0] = gain[1] = gain[2] = gain[3] = gain[4] = gain[5];

    /*    TGraph* test = new  TGraph(48,X_errors,X_errors);
     test->SetName(("/HIST1/Tile/RawChannel/Summary/LFTEST"+moduleName).c_str());
     m_rootsvc->regGraph("/HIST1/Tile/RawChannel/Summary/LFTEST"+moduleName, test);
     */

    TCanvas * Can = NULL;
    if (do_plots) {
      Can = new TCanvas("amp_ratio", "amp_ratio", 402 * maxgain, 588);
      Can->Divide(maxgain, 2);
      gStyle->SetOptStat(0);
      gStyle->SetTitleFontSize(0.1);
    }

    TLine line(0., 1., 48., 1.); //Draw a green line to guide the sight
    line.SetLineColor(3);

    std::string subDir = "Summary";
    std::vector<TGraphAsymmErrors*> grapherrVec;

    for (int adc = 0; adc < maxgain; ++adc) {
      for (int cap = 0; cap < 2; ++cap) {

        std::ostringstream sStr;
        sStr << moduleName << gain[adc] << "_tails" << CapName[cap];
        std::string graphName = sStr.str();
        sStr.str("");
        sStr << moduleName << gain[adc + 3] << " Mean Amp/Q ratio and up/down tails " << CapName[2 + cap];
        std::string graphTitle = sStr.str();

        TVirtualPad * pad;
        if (do_plots) {
          pad = Can->cd(cap * maxgain + adc + 1);
          pad->SetTopMargin(0.15);
          pad->SetGridx();
        }

        TGraphAsymmErrors * final_Egraph = bookGraphAsymmErrors(subDir, graphName, graphTitle, 48, X_axis, m_data->m_rangeQ[adc][cap][0], X_errors, X_errors,
            m_data->m_rangeQ[adc][cap][1], m_data->m_rangeQ[adc][cap][2]);
        grapherrVec.push_back(final_Egraph);

        m_data->m_finalHist1[ros][drawer][adc][cap][2]->SetStats(kFALSE);
        m_data->m_finalHist1[ros][drawer][adc][cap][2]->SetMarkerStyle(21);
        m_data->m_finalHist1[ros][drawer][adc][cap][2]->SetMarkerSize(ms);
        m_data->m_finalHist1[ros][drawer][adc][cap][2]->SetLabelSize(0.06, "X");
        m_data->m_finalHist1[ros][drawer][adc][cap][2]->SetLabelSize(0.06, "Y");
        m_data->m_finalHist1[ros][drawer][adc][cap][2]->SetMaximum(2.2);
        m_data->m_finalHist1[ros][drawer][adc][cap][2]->SetMinimum(-0.2);
        if (do_plots) m_data->m_finalHist1[ros][drawer][adc][cap][2]->Draw("P0");
        
        final_Egraph->SetMarkerStyle(21);
        final_Egraph->SetMarkerSize(ms);
        final_Egraph->SetMaximum(2.2);
        final_Egraph->SetMinimum(-0.2);
        final_Egraph->GetXaxis()->SetRangeUser(0, 48);
        if (do_plots) {
          final_Egraph->Draw("P0");
          line.Draw();
        }

      }
    }

    if (m_savePng) {
      Can->Print(TString(moduleName + "_amp_ratio.png"), "png");
    }
    if (m_savePs) {
      Can->Print(TString(moduleName + "_amp_ratio.ps"), "ps");
    }
    if (m_saveSvg) {
      Can->Print(TString(moduleName + "_amp_ratio.svg"), "svg");
    }
    if (do_plots) delete Can;

// we have to remove TGraphAsymmErrors in 13.0.10, because THistSvc doesn't handle them well
    if (!m_storeGraph) {
      std::vector<TGraphAsymmErrors*>::const_iterator it = grapherrVec.begin();
      std::vector<TGraphAsymmErrors *>::const_iterator itend = grapherrVec.end();
      for (; it != itend; ++it) {
        if ((removeTObj(*it)).isFailure()) ATH_MSG_WARNING("Problems removing TObj");
      }
    }

    if (do_plots) {
      Can = new TCanvas("fit_time", "fit_time", 402 * maxgain, 588);
      Can->Divide(maxgain, 4);
      gStyle->SetOptStat(0);
      gStyle->SetTitleFontSize(0.1);
    }

    double maxy[2] = { 1.4, 15.0 };
    double miny[2] = { -0.1, -15.0 };

    for (int adc = 0; adc < maxgain; ++adc) {
      for (int cap = 0; cap < 2; ++cap) {
        for (int type = 0; type < 2; ++type) {

          TVirtualPad * pad;
          if (do_plots) {
            pad = Can->cd((type + cap * 2) * maxgain + adc + 1);
            pad->SetTopMargin(0.15);
            pad->SetGridx();
          }

          if (m_data->m_finalHist1[ros][drawer][adc][cap][type]->GetMaximum() < 0.9 * maxy[type])
            m_data->m_finalHist1[ros][drawer][adc][cap][type]->SetMaximum(maxy[type]);
          if (m_data->m_finalHist1[ros][drawer][adc][cap][type]->GetMinimum() > (miny[type] + 0.1 * TMath::Abs(miny[type])))
            m_data->m_finalHist1[ros][drawer][adc][cap][type]->SetMinimum(miny[type]);

          m_data->m_finalHist1[ros][drawer][adc][cap][type]->SetMarkerStyle(21);
          m_data->m_finalHist1[ros][drawer][adc][cap][type]->SetMarkerSize(ms);
          m_data->m_finalHist1[ros][drawer][adc][cap][type]->SetLabelSize(0.10, "X");
          m_data->m_finalHist1[ros][drawer][adc][cap][type]->SetLabelSize(0.10, "Y");
          if (do_plots) {
            m_data->m_finalHist1[ros][drawer][adc][cap][type]->Draw("P0");
            
            line.SetLineColor(3);
            line.Draw();
          }
        }
      }
    }

    if (m_savePng) {
      Can->Print(TString(moduleName + "_fit_time.png"), "png");
    }
    if (m_savePs) {
      Can->Print(TString(moduleName + "_fit_time.ps"), "ps");
    }
    if (m_saveSvg) {
      Can->Print(TString(moduleName + "_fit_time.svg"), "svg");
    }
    if (do_plots) delete Can;

  } else {
    if ((m_runType == LasRun) || (m_runType == LedRun)) { //same stuff but with Laser fancy colors
      LaserFancyPlotting(ros, drawer, maxgain, moduleName);
    } else {

      TCanvas * Can = NULL;
      if (do_plots) {
        Can = new TCanvas("fit_amp", "fit_amp", 402 * maxgain, 588);
        Can->Divide(maxgain, 3);
        gStyle->SetOptStat(0);
        gStyle->SetTitleFontSize(0.1);
      }

      double maxy[6] = { 1.0, 0.05, 1.0, 0.05, 25.0, 25.0 };
      double miny[6] = { -1.0, -0.05, 0.0, 0.0, -25.0, -25.0 };

      for (int adc = 0; adc < maxgain; ++adc) {

        double max0 = m_data->m_finalHist1[ros][drawer][adc][0][0]->GetMaximum();
        double max1 = m_data->m_finalHist1[ros][drawer][adc][0][1]->GetMaximum();
        double max2 = m_data->m_finalHist1[ros][drawer][adc][0][2]->GetMaximum();
        double max3 = m_data->m_finalHist1[ros][drawer][adc][0][3]->GetMaximum();
        double min0 = m_data->m_finalHist1[ros][drawer][adc][0][0]->GetMinimum();
        double min1 = m_data->m_finalHist1[ros][drawer][adc][0][1]->GetMinimum();
        double min2 = m_data->m_finalHist1[ros][drawer][adc][0][2]->GetMinimum();
        double min3 = m_data->m_finalHist1[ros][drawer][adc][0][3]->GetMinimum();

        TVirtualPad * pad;
        if (do_plots) {
          pad = Can->cd(adc + 1);
          pad->SetTopMargin(0.15);
          pad->SetGridx();
        }

        if (max0 < 0.9 * maxy[adc]) m_data->m_finalHist1[ros][drawer][adc][0][0]->SetMaximum(maxy[adc]);
        if (min0 > (miny[adc] + 0.1 * TMath::Abs(miny[adc]))) m_data->m_finalHist1[ros][drawer][adc][0][0]->SetMinimum(miny[adc]);

        m_data->m_finalHist1[ros][drawer][adc][0][0]->SetMarkerStyle(21);
        m_data->m_finalHist1[ros][drawer][adc][0][0]->SetMarkerSize(ms);
        m_data->m_finalHist1[ros][drawer][adc][0][0]->SetLabelSize(0.08, "X");
        m_data->m_finalHist1[ros][drawer][adc][0][0]->SetLabelSize(0.08, "Y");
        if (do_plots) {
          m_data->m_finalHist1[ros][drawer][adc][0][0]->Draw("E0");
        }

        if (do_plots) {
          pad = Can->cd(maxgain + adc + 1);
          pad->SetTopMargin(0.15);
          pad->SetGridx();
        }

        //     if (max2 > 5 * max1) max2 = 2 * max1; // don't put crazy fit results on plot
        if (max1 < maxy[2 + adc]) {
          if (max2 < maxy[2 + adc])
            m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMaximum(maxy[2 + adc]);
          else
            m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMaximum(max2);
        } else {
          if (max1 < max2) m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMaximum(max2);
        }

        if (min1 > miny[2 + adc]) {
          if (min2 > miny[2 + adc])
            m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMinimum(miny[2 + adc]);
          else
            m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMinimum(min2);
        } else {
          if (min1 > min2) m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMinimum(min2);
        }

        m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMarkerStyle(21);
        m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMarkerSize(ms);
        m_data->m_finalHist1[ros][drawer][adc][0][1]->SetLabelSize(0.08, "X");
        m_data->m_finalHist1[ros][drawer][adc][0][1]->SetLabelSize(0.08, "Y");
        if (do_plots) {
          m_data->m_finalHist1[ros][drawer][adc][0][1]->Draw("P0");
        }

        m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMaximum(m_data->m_finalHist1[ros][drawer][adc][0][1]->GetMaximum());
        m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMinimum(m_data->m_finalHist1[ros][drawer][adc][0][1]->GetMinimum());
        m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMarkerStyle(25);
        m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMarkerSize(ms);
        m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMarkerColor(4);
        m_data->m_finalHist1[ros][drawer][adc][0][2]->SetLabelSize(0.08, "X");
        m_data->m_finalHist1[ros][drawer][adc][0][2]->SetLabelSize(0.08, "Y");
        if (do_plots) {
          m_data->m_finalHist1[ros][drawer][adc][0][2]->Draw("sameP0");
        }

        if (do_plots) {
          pad = Can->cd(2 * maxgain + adc + 1);
          pad->SetTopMargin(0.15);
          pad->SetGridx();
        }

        if (max3 < 0.9 * maxy[4 + adc]) m_data->m_finalHist1[ros][drawer][adc][0][3]->SetMaximum(maxy[4 + adc]);
        if (min3 > (miny[4 + adc] + 0.1 * TMath::Abs(miny[4 + adc]))) m_data->m_finalHist1[ros][drawer][adc][0][3]->SetMinimum(miny[4 + adc]);

        m_data->m_finalHist1[ros][drawer][adc][0][3]->SetMarkerStyle(21);
        m_data->m_finalHist1[ros][drawer][adc][0][3]->SetMarkerSize(ms);
        m_data->m_finalHist1[ros][drawer][adc][0][3]->SetLabelSize(0.08, "X");
        m_data->m_finalHist1[ros][drawer][adc][0][3]->SetLabelSize(0.08, "Y");
        if (do_plots) {
          m_data->m_finalHist1[ros][drawer][adc][0][3]->Draw("E0");
        }

      } //end of loop over gain

      if (m_savePng) {
        Can->Print(TString(moduleName + "_fit_amp.png"), "png");
      }
      if (m_savePs) {
        Can->Print(TString(moduleName + "_fit_amp.ps"), "ps");
      }
      if (m_saveSvg) {
        Can->Print(TString(moduleName + "_fit_amp.svg"), "svg");
      }
      if (do_plots) delete Can;
    }
  }
}

/*---------------------------------------------------------*/
void TileRawChannelMonTool::drawDsp(int ros, int drawer, const std::string& moduleName)
/*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  int maxgain = (m_bigain) ? 2 : 1;
  double ms = (m_bigain) ? 0.75 : 1.0; // marker size
  bool do_plots = m_savePng || m_savePs || m_saveSvg;

  TCanvas * Can = NULL; // for -Wmaybe-uninitialized
  if (do_plots) {
    Can = new TCanvas("dsp_amp", "dsp_amp", 402 * maxgain, 588);
    Can->Divide(maxgain, 3);
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
  }

  TLine line(0., 0., 48., 0.); //Draw a green line to guide the sight
  line.SetLineColor(3);

  double maxy[6] = { 0.05, 0.05, 0.5, 0.5, 10.0, 10.0 };
  double miny[6] = { -0.05, -0.05, -0.5, -0.5, -0.05, -0.05 };
  double norm[2] = { 1., 1 };
  for (int adc = 0; adc < maxgain; ++adc) {

    double maxEdsp = m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->GetMaximum();
    double minEdsp = m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->GetMinimum();
    double maxTdsp = m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->GetMaximum();
    double minTdsp = m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->GetMinimum();
    double maxchidsp = m_data->m_finalHistDsp2[ros][drawer][adc][0]->GetMaximum();
    double minchidsp = m_data->m_finalHistDsp2[ros][drawer][adc][0]->GetMinimum();

    TVirtualPad * pad;
    if (do_plots) {
      pad = Can->cd(adc + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
    }

    if (maxEdsp < 0.9 * maxy[adc]) m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->SetMaximum(maxy[adc]);
    if (minEdsp > miny[adc] + 0.1 * TMath::Abs(miny[adc])) m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->SetMinimum(miny[adc]);

    m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->SetMarkerStyle(21);
    m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->SetMarkerSize(ms);
    m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->SetLabelSize(0.08, "X");
    m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->SetLabelSize(0.08, "Y");
    if (do_plots) m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->Draw("");

    //Now we add the 1d histogram on the y-axis
    if (m_data->m_hBarDsp1[ros][drawer][adc][sumEdsp_fit]->GetMaximum() > 0.1) { //normalize the scale to get into the frame
      norm[sumEdsp_fit] = 47. / m_data->m_hBarDsp1[ros][drawer][adc][sumEdsp_fit]->GetMaximum();
    }
    m_data->m_hBarDsp1[ros][drawer][adc][sumEdsp_fit]->Scale(norm[sumEdsp_fit]);
    //hbardsp1[ros][drawer][adc][sumEdsp_fit]->SetFillStyle(3350);
    m_data->m_hBarDsp1[ros][drawer][adc][sumEdsp_fit]->SetFillColor(38);
    if (do_plots) {
      m_data->m_hBarDsp1[ros][drawer][adc][sumEdsp_fit]->Draw("hbar,same");

      m_data->m_finalHistDsp1[ros][drawer][adc][sumEdsp_fit]->Draw("E0,same");

      line.Draw();

      pad = Can->cd(maxgain + adc + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
    }

    if (maxTdsp < 0.9 * maxy[2 + adc]) m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->SetMaximum(maxy[2 + adc]);
    if (minTdsp > miny[2 + adc] + 0.1 * TMath::Abs(miny[2 + adc])) m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->SetMinimum(miny[2 + adc]);

    m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->SetMarkerStyle(21);
    m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->SetMarkerSize(ms);
    m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->SetLabelSize(0.08, "X");
    m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->SetLabelSize(0.08, "Y");
    if (do_plots) m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->Draw("");

    if (m_data->m_hBarDsp1[ros][drawer][adc][sumTdsp_fit]->GetMaximum() > 0.1) { //normalize the scale to get into the frame
      norm[sumTdsp_fit] = 47. / m_data->m_hBarDsp1[ros][drawer][adc][sumTdsp_fit]->GetMaximum();
    }
    m_data->m_hBarDsp1[ros][drawer][adc][sumTdsp_fit]->Scale(norm[sumTdsp_fit]);
    //      hbardsp1[ros][drawer][adc][sumTdsp_fit]->SetFillStyle(3350);
    m_data->m_hBarDsp1[ros][drawer][adc][sumTdsp_fit]->SetFillColor(38);
    if (do_plots) {
      m_data->m_hBarDsp1[ros][drawer][adc][sumTdsp_fit]->Draw("hbar,same");

      m_data->m_finalHistDsp1[ros][drawer][adc][sumTdsp_fit]->Draw("E0,same");

      line.Draw();

      pad = Can->cd(2 * maxgain + adc + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
      pad->SetGridy();
    }

    if (maxchidsp < 0.9 * maxy[4 + adc]) m_data->m_finalHistDsp2[ros][drawer][adc][0]->SetMaximum(maxy[4 + adc]);
    if (minchidsp > miny[4 + adc] + 0.1 * TMath::Abs(miny[4 + adc])) m_data->m_finalHistDsp2[ros][drawer][adc][0]->SetMinimum(miny[4 + adc]);

    m_data->m_finalHistDsp2[ros][drawer][adc][0]->SetLabelSize(0.08, "X");
    m_data->m_finalHistDsp2[ros][drawer][adc][0]->SetLabelSize(0.08, "Y");
    if (do_plots) {
      gStyle->SetPalette(1);
      m_data->m_finalHistDsp2[ros][drawer][adc][0]->Draw("zcol");
    }

  } //end of loop over gain

  if (m_savePng) {
    Can->Print(TString(moduleName + "_dsp_amp.png"), "png");
  }
  if (m_savePs) {
    Can->Print(TString(moduleName + "_dsp_amp.ps"), "ps");
  }
  if (m_saveSvg) {
    Can->Print(TString(moduleName + "_dsp_amp.svg"), "svg");
  }
  if (do_plots) delete Can;

  for (int adc = 0; adc < maxgain; ++adc) {
    m_data->m_hBarDsp1[ros][drawer][adc][sumEdsp_fit]->Scale(1 / norm[sumEdsp_fit]); //back to normal
    m_data->m_hBarDsp1[ros][drawer][adc][sumTdsp_fit]->Scale(1 / norm[sumTdsp_fit]); //back to normal
  }
}

/*---------------------------------------------------------*/
void TileRawChannelMonTool::rangeErrorBar(double& xmin, double& xmax, double mean) {
  /*---------------------------------------------------------*/
//! This method was bing used in 2006 for calculating the error bar length
//! It is off by default, but it can be switched on by the usage of
//! the property  m_useratioerror=false
//! This method set the error bar long as the interval from ean to the extreeme 
//! value of the sample distribution. If the extreeme value is below the threhold, 
//! then the error bar length is set to 0.
  if (m_DownLimit < mean && mean < m_UpLimit) {
    if (xmin > m_DownLimit) xmin = mean;
    if (xmax < m_UpLimit) xmax = mean;
  }

}

/*---------------------------------------------------------*/
void TileRawChannelMonTool::ratioErrorBar(TH1S* hist, double& xmin, double& xmax, double mean) {
  /*---------------------------------------------------------*/
//! This method calculates the error bars of amp vs Q summary plots.
//! The error bar depends on the percentage of events outside a certain limit.
//! If the  percentage of events outside a certain limit is below the threshold,
//! then the error bar length is set to 0.
//! There are 3 different thresholds and a lower and an upper limit.
//! Threshold, limits and the error bar method usage are all settable by jo
//! The are 3 different error bars lenghts for low, medium and high severity.
//! IMPORTANT: In case the mean of the distribution is outside the limits, then 
//! the error bars are calculated as with rangeErrorBar method, as it won't have 
//! much sense calculating it in this way.
  if (m_DownLimit < mean && mean < m_UpLimit) {

    if (xmin > m_DownLimit) {
      xmin = mean;
    } else {
      int lo_thrbin = hist->FindBin(m_DownLimit); //
      double integral = hist->Integral(0, lo_thrbin); // we also want underflow, in case... Integral() is calculated including the contents of both limiting bins. As 0.7 falls in the middle of a bin, we are calculating the integral by excess
      double ratio = integral / (hist->GetEntries());
      if (ratio > m_hi_IntegralLimit) {
        xmin = mean - 1.05;
      } //larger error bar
      else if (ratio > m_med_IntegralLimit) {
        xmin = mean - 0.7;
      } //intermediate error bar
      else if (ratio > m_lo_IntegralLimit) {
        xmin = mean - 0.35;
      } //small error bar
      else {
        xmin = mean;
      }
    }

    if (xmax < m_UpLimit) {
      xmax = mean;
    } else {
      int hi_thrbin = hist->FindBin(m_UpLimit);
      double integral = hist->Integral(hi_thrbin, (hist->GetNbinsX() + 1)); //we also want overflow, in case...  Integral() is calculated including the contents of both limiting bins. As 1.3 falls in the middle of a bin, we are calculating the integral by excess.
      double ratio = integral / (hist->GetEntries());
      if (ratio > m_hi_IntegralLimit) {
        xmax = mean + 1.05;
      } //larger error bar
      else if (ratio > m_med_IntegralLimit) {
        xmax = mean + 0.7;
      } //intermediate error bar
      else if (ratio > m_lo_IntegralLimit) {
        xmax = mean + 0.35;
      } //small error bar
      else {
        xmax = mean;
      }
    }
  }
  //! LF: If mean <0.7 or mean>1.3, we leave the range as error bar, as the ratio Error bar won't have any sense
}

//!Time Slope parameters for CIS runs. Was done once using correlation between X and Y, but this is not capable of coping 
//!with 
/*---------------------------------------------------------*/
TF1* TileRawChannelMonTool::GetTimeFitFunc(TH2S* hist2d) {
  /*---------------------------------------------------------*/
//the TF1 pointer is created with new, please use a delete after the parameters have been extracted!
  if (hist2d) {
    TProfile* prof = hist2d->ProfileX();

    if (prof) {
      TH1S hist("hist", "TMP Histo", prof->GetNbinsX(), prof->GetBinLowEdge(1), prof->GetBinLowEdge(prof->GetNbinsX() + 1));
      float lastbin = -99.;
      float lasti = 0.;
      float shift = 0.;
      for (int i = 1; i < prof->GetNbinsX() + 1; i++) {
        if (prof->GetBinError(i) > 1e-7) {
          if ((shift < 1.) && ((prof->GetBinContent(i) - (lastbin + (i - lasti))) < -15.)) { //allow only 1 shift and only of negative sign
            shift = 25.;
          }
          lasti = i;
          lastbin = prof->GetBinContent(i);
          hist.SetBinContent(i, prof->GetBinContent(i) + shift);
          hist.SetBinError(i, prof->GetBinError(i));
        }
      } // end for loop on the histogram bins
      TF1* polfun = new TF1("polfun", "pol1", 0., 25.); //remember to delete!
      hist.Fit("polfun", "NQ");

      polfun->SetParameter(0, polfun->GetParameter(0) - 25.); // shift by -25 ns to be consistent with previous definition
      delete prof; //not needed, but these profiles appears in the root files, even if they are not booked
      return polfun; //rember to delete!
    }
  }
  return 0;
}
/*---------------------------------------------------------*/
bool TileRawChannelMonTool::checkDmuHeader(std::vector<uint32_t>* headerVec, int dmu)
/*---------------------------------------------------------*/
{
  bool err = false;

  if (checkDmuHeaderFormat((*headerVec)[dmu])) {
    err = true;

  }
  if (checkDmuHeaderParity((*headerVec)[dmu])) {
    err = true;

  }
  if (((*headerVec)[dmu] >> 25) & 0x1) {
    //Memory Parity Error
    err = true;

  }
  if (((*headerVec)[dmu] >> 24) & 0x1) {
    //Single Strobe Error
    err = true;

  }
  if (((*headerVec)[dmu] >> 23) & 0x1) {
    //Double Strobe Error
    err = true;

  }

  return err;
}
/*---------------------------------------------------------*/
void TileRawChannelMonTool::LaserFancyPlotting(int ros, int drawer, int maxgain, const std::string& moduleName) {
  /*---------------------------------------------------------*/

  ATH_MSG_DEBUG("in LaserFancyPlotting...");

  //TCanvas * Can = new TCanvas("fit_amp","fit_amp",402*maxgain,588);
  bool do_plots = m_savePng || m_savePs || m_saveSvg;
  TCanvas * Can = NULL;
  if (do_plots) {
    Can = new TCanvas("fit_amp", "fit_amp", 402 * maxgain, 735);	//Lukas
    //Can->Divide(maxgain, 4);
    Can->Divide(maxgain, 5);	//Lukas
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
  }
  TLine *line = new TLine();
  line->SetLineWidth(2);
  line->SetLineStyle(7);
  line->SetLineColor(3);

  float ms = (m_bigain) ? 0.75 : 1.0; // marker size

  double maxy[6] = { 5.0, 0.5, 1.0, 0.05, 25.0, 25.0 };
  double miny[6] = { -5.0, -0.5, 0.0, 0.0, -25.0, -25.0 };

  // Mean value
  TH1F* final_empty[2]; //adc, type
  TH1F* final_odd[2]; //adc, type
  TH1F* final_even[2]; //adc, type

  // define TH1 that will contain the plots of variance/mean
  TH1F* pmtGain_empty[2];
  TH1F* pmtGain_odd[2];
  TH1F* pmtGain_even[2];

  for (int g = 0; g < 2; g++) {

    std::ostringstream hn;
    hn << "empty_" << g;
    final_empty[g] = new TH1F(hn.str().c_str(), m_data->m_finalHist1[ros][drawer][g][0][0]->GetTitle(), 48, 0, 48);
    hn.str("");
    hn << "odd_" << g;
    final_odd[g] = new TH1F(hn.str().c_str(), m_data->m_finalHist1[ros][drawer][g][0][0]->GetTitle(), 48, 0, 48);
    hn.str("");
    hn << "even_" << g;
    final_even[g] = new TH1F(hn.str().c_str(), m_data->m_finalHist1[ros][drawer][g][0][0]->GetTitle(), 48, 0, 48);

    //---- Histograms for the ratio Var/Mean
    // titles...
    std::string title;
    const char* gain[2] = { "low", "high" };
    const char* moduleNames[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
    if (drawer < 10)
      title = Form("%s0%i %s gain, Variance/Mean", moduleNames[ros], drawer + 1, gain[g]);
    else
      title = Form("%s%i %s gain, Variance/Mean", moduleNames[ros], drawer + 1, gain[g]);

    std::ostringstream sStr;
    std::string subDir = "Summary";
    sStr.str("");
    sStr << moduleNames[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ') << gain[g] << "_pmtGain_empty";
    std::string histName = sStr.str();
    pmtGain_empty[g] = book1F(subDir, histName, title, 48, 0.0, 48.0);
    //    pmtGain_empty[g]= new TH1F(Form("pmtGain_empty_%i", g),title.c_str(), 48,0,48);

    sStr.str("");
    sStr << moduleNames[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ') << gain[g] << "_pmtGain_odd";
    histName = sStr.str();
    pmtGain_odd[g] = book1F(subDir, histName, title, 48, 0.0, 48.0);
    //    pmtGain_odd[g]  = new TH1F(Form("pmtGain_odd_%i", g),  title.c_str(), 48,0,48);

    sStr.str("");
    sStr << moduleNames[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ') << gain[g] << "_pmtGain_even";
    histName = sStr.str();
    pmtGain_even[g] = book1F(subDir, histName, title, 48, 0.0, 48.0);
    //pmtGain_even[g] = new TH1F(Form("pmtGain_even_%i", g), title.c_str(), 48,0,48);
  }

  // loop over the 2 gains values
  for (int adc = 0; adc < maxgain; ++adc) {

    double max0 = m_data->m_finalHist1[ros][drawer][adc][0][0]->GetMaximum();
    //double max1 = final_hist1[ros][drawer][adc][0][1]->GetMaximum();
    //double max2 = final_hist1[ros][drawer][adc][0][2]->GetMaximum();
    double max3 = m_data->m_finalHist1[ros][drawer][adc][0][3]->GetMaximum();
    double max4 = m_data->m_finalHist1[ros][drawer][adc][0][4]->GetMaximum();	//Lukas
    double min0 = m_data->m_finalHist1[ros][drawer][adc][0][0]->GetMinimum();
    //double min1 = final_hist1[ros][drawer][adc][0][1]->GetMinimum();
    //double min2 = final_hist1[ros][drawer][adc][0][2]->GetMinimum();
    double min3 = m_data->m_finalHist1[ros][drawer][adc][0][3]->GetMinimum();
    double min4 = m_data->m_finalHist1[ros][drawer][adc][0][4]->GetMinimum();	//Lukas

    if (max0 > 0.) {
      final_empty[adc]->SetMaximum(1.05 * max0);
    } else {
      final_empty[adc]->SetMaximum(0.);
    }

    if ((max0 - min0) > 0) {
      final_empty[adc]->SetMinimum(max0 - 1.05 * (max0 - min0));
    }

    // Select pad 1 and 2
    TVirtualPad * pad;
    if (do_plots) {
      pad = Can->cd(adc + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
    }

    // bin=1 corresponds to the first pmt (#0)
    double Kapa = 1.30e-3;
    for (int ch = 0; ch < 48; ++ch) {
      if (isDisconnected(ros, drawer, ch)) {
        final_empty[adc]->SetBinContent(ch + 1, m_data->m_finalHist1[ros][drawer][adc][0][0]->GetBinContent(ch + 1) + 0.01);
        final_empty[adc]->SetBinError(ch + 1, m_data->m_finalHist1[ros][drawer][adc][0][0]->GetBinError(ch + 1) + 0.01);
        pmtGain_empty[adc]->SetBinContent(ch + 1, 0.01);
        pmtGain_empty[adc]->SetBinError(ch + 1, 0.01);
      }
      // connected channels
      else {
        double mean = m_data->m_finalHist1[ros][drawer][adc][0][0]->GetBinContent(ch + 1);
        double rms = m_data->m_finalHist1[ros][drawer][adc][0][1]->GetBinContent(ch + 1);
        double dmean = m_data->m_finalHist1[ros][drawer][adc][0][0]->GetBinError(ch + 1);
        double drms = m_data->m_finalHist1[ros][drawer][adc][0][1]->GetBinError(ch + 1);
        // even pmts
        int pmt = abs(m_cabling->channel2hole(ros, ch)); //extra safe: abs should not be necessary, because channels are connected.
        if (pmt % 2 == 0) {
          final_even[adc]->SetBinContent(ch + 1, m_data->m_finalHist1[ros][drawer][adc][0][0]->GetBinContent(ch + 1) + 0.001);
          final_even[adc]->SetBinError(ch + 1, m_data->m_finalHist1[ros][drawer][adc][0][0]->GetBinError(ch + 1) + 0.001);

          //			    if (bin!=0)
          //			      log<<MSG::DEBUG<<"["<<ros<<"]["<<drawer+1<<"]["<<adc<<"]["<< bin <<"] : mean="
          //	     << mean <<", var="<< rms*rms<<"\tVar/mean(corrected)="<< (rms*rms/mean) - Kapa*mean <<endreq;
          if (mean > 10e-3) {
            pmtGain_even[adc]->SetBinContent(ch + 1, (rms * rms / mean) - Kapa * mean);
            pmtGain_even[adc]->SetBinError(ch + 1, (rms * rms / mean) * sqrt((2 * drms / rms) * (2 * drms / rms) + (dmean / mean) * (dmean / mean)));
          } else {
            pmtGain_even[adc]->SetBinContent(ch + 1, -0.5);
            pmtGain_even[adc]->SetBinError(ch + 1, 0.001);
          }
        }
        // odd pmts
        else {
          final_odd[adc]->SetBinContent(ch + 1, m_data->m_finalHist1[ros][drawer][adc][0][0]->GetBinContent(ch + 1) + 0.001);
          final_odd[adc]->SetBinError(ch + 1, m_data->m_finalHist1[ros][drawer][adc][0][0]->GetBinError(ch + 1) + 0.001);

          // log<<MSG::DEBUG<<"["<<ros<<"]["<<drawer+1<<"]["<<adc<<"]["<< bin <<"] : mean="
          // << mean <<", var="<< rms*rms<<"\tVar/mean(corrected)="<< (rms*rms/mean) - Kapa*mean <<endreq;
          if (mean > 10e-3) {
            pmtGain_odd[adc]->SetBinContent(ch + 1, (rms * rms / mean) - Kapa * mean);
            pmtGain_odd[adc]->SetBinError(ch + 1, (rms * rms / mean) * sqrt((2 * drms / rms) * (2 * drms / rms) + (dmean / mean) * (dmean / mean)));
          } else {
            pmtGain_odd[adc]->SetBinContent(ch + 1, -0.5);
            pmtGain_odd[adc]->SetBinError(ch + 1, 0.001);
          }
        }
      }
    }

    // we are in pads 1 and 2
    // retrieve the max and min of the plots
    double max = pmtGain_odd[adc]->GetMaximum();
    if (pmtGain_even[adc]->GetMaximum() > max) max = 1.05 * pmtGain_even[adc]->GetMaximum();
    double min = pmtGain_odd[adc]->GetMinimum();
    if (pmtGain_even[adc]->GetMinimum() < min) min = pmtGain_even[adc]->GetMinimum() - 0.05 * fabs(pmtGain_even[adc]->GetMinimum());
    if (max < 0.20) max = 0.20;
    if (min > -0.10) min = -0.10;
    //log<<MSG::INFO<<"Min and Max : "<< min <<"; "<< max <<enreq;		
    pmtGain_empty[adc]->SetMarkerStyle(21);
    pmtGain_even[adc]->SetMarkerStyle(22);
    pmtGain_odd[adc]->SetMarkerStyle(23);
    pmtGain_empty[adc]->SetMarkerSize(ms);
    pmtGain_even[adc]->SetMarkerSize(ms);
    pmtGain_odd[adc]->SetMarkerSize(ms);
    pmtGain_empty[adc]->SetLabelSize(0.08, "X");
    pmtGain_empty[adc]->SetLabelSize(0.08, "Y");
    pmtGain_even[adc]->SetMarkerColor(2);
    pmtGain_odd[adc]->SetMarkerColor(4);
    pmtGain_empty[adc]->GetYaxis()->SetRangeUser(min, max);
    if (do_plots) {
      pmtGain_empty[adc]->Draw("e");
      pmtGain_even[adc]->Draw("same,e");
      pmtGain_odd[adc]->Draw("same,e");
      line->DrawLine(0, -0.01, 48, -0.01);
      line->DrawLine(0, 0.15, 48, 0.15);

      // Select pads 3 and 4
      pad = Can->cd(maxgain + adc + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
    }

    // Plot the evolution of the mean vs #pmt number (with different colours for odd and even pmts)
    if (max0 < 0.9 * maxy[adc]) final_empty[adc]->SetMaximum(maxy[adc]);
    if (min0 > miny[adc] + 0.1 * TMath::Abs(miny[adc])) final_empty[adc]->SetMinimum(miny[adc]);

    final_empty[adc]->SetMarkerStyle(21);
    final_even[adc]->SetMarkerStyle(22);
    final_odd[adc]->SetMarkerStyle(23);
    final_empty[adc]->SetMarkerSize(ms);
    final_even[adc]->SetMarkerSize(ms);
    final_odd[adc]->SetMarkerSize(ms);
    final_empty[adc]->SetLabelSize(0.08, "X");
    final_empty[adc]->SetLabelSize(0.08, "Y");
    final_even[adc]->SetMarkerColor(2);
    final_odd[adc]->SetMarkerColor(4);
    if (do_plots) {
      final_empty[adc]->Draw("e");
      final_even[adc]->Draw("same,e");
      final_odd[adc]->Draw("same,e");

      // Select pads 5 and 6 and plot the RMS for each pmt
      pad = Can->cd(2 * maxgain + adc + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
    }

    m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMarkerStyle(21);
    m_data->m_finalHist1[ros][drawer][adc][0][1]->SetMarkerSize(ms);
    m_data->m_finalHist1[ros][drawer][adc][0][1]->SetLabelSize(0.08, "X");
    m_data->m_finalHist1[ros][drawer][adc][0][1]->SetLabelSize(0.08, "Y");
    if (do_plots) m_data->m_finalHist1[ros][drawer][adc][0][1]->Draw("P0");

    m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMaximum(m_data->m_finalHist1[ros][drawer][adc][0][1]->GetMaximum());
    m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMinimum(m_data->m_finalHist1[ros][drawer][adc][0][1]->GetMinimum());
    m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMarkerStyle(25);
    m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMarkerSize(ms);
    m_data->m_finalHist1[ros][drawer][adc][0][2]->SetMarkerColor(4);
    m_data->m_finalHist1[ros][drawer][adc][0][2]->SetLabelSize(0.08, "X");
    m_data->m_finalHist1[ros][drawer][adc][0][2]->SetLabelSize(0.08, "Y");
    if (do_plots) m_data->m_finalHist1[ros][drawer][adc][0][2]->Draw("sameP0");

    // Select pads 7 and 8
    if (do_plots) {
      pad = Can->cd(3 * maxgain + adc + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
    }

    if (max3 < 0.9 * maxy[4 + adc]) m_data->m_finalHist1[ros][drawer][adc][0][3]->SetMaximum(maxy[4 + adc]);
    if (min3 > miny[4 + adc] + 0.1 * TMath::Abs(miny[4 + adc])) m_data->m_finalHist1[ros][drawer][adc][0][3]->SetMinimum(miny[4 + adc]);

    m_data->m_finalHist1[ros][drawer][adc][0][3]->SetMarkerStyle(21);
    m_data->m_finalHist1[ros][drawer][adc][0][3]->SetMarkerSize(ms);
    m_data->m_finalHist1[ros][drawer][adc][0][3]->SetLabelSize(0.08, "X");
    m_data->m_finalHist1[ros][drawer][adc][0][3]->SetLabelSize(0.08, "Y");
    m_data->m_finalHist1[ros][drawer][adc][0][3]->Draw("E0");

    //Lukas
    // Select pads 9 and 10
    if (do_plots) {
      pad = Can->cd(4 * maxgain + adc + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
    }

    if (max4 < 0.9 * maxy[4 + adc]) m_data->m_finalHist1[ros][drawer][adc][0][4]->SetMaximum(maxy[4 + adc]);
    if (min4 > miny[4 + adc] + 0.1 * TMath::Abs(miny[4 + adc])) m_data->m_finalHist1[ros][drawer][adc][0][4]->SetMinimum(miny[4 + adc]);

    m_data->m_finalHist1[ros][drawer][adc][0][4]->SetMarkerStyle(21);
    m_data->m_finalHist1[ros][drawer][adc][0][4]->SetMarkerSize(ms);
    m_data->m_finalHist1[ros][drawer][adc][0][4]->SetLabelSize(0.08, "X");
    m_data->m_finalHist1[ros][drawer][adc][0][4]->SetLabelSize(0.08, "Y");
    if (do_plots) m_data->m_finalHist1[ros][drawer][adc][0][4]->Draw("E0");
    //Lukas

  }    //end of loop over gain

  if (m_savePng) {
    Can->Print(TString(moduleName + "_fit_amp.png"), "png");
  }
  if (m_savePs) {
    Can->Print(TString(moduleName + "_fit_amp.ps"), "ps");
  }
  if (m_saveSvg) {
    Can->Print(TString(moduleName + "_fit_amp.svg"), "svg");
  }
  //Can->Print(TString(moduleName+"_fit_amp.cxx"),"cxx");
  if (do_plots) delete Can;
  delete line;
  for (int g = 0; g < 2; g++) {
    delete final_empty[g];
    delete final_odd[g];
    delete final_even[g];
    //delete  pmtGain_empty[g];
    //delete  pmtGain_odd[g];
    //delete  pmtGain_even[g];
  }
}
