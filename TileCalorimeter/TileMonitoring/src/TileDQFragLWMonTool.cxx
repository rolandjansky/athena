/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDQFragLWMonTool.cxx
// PACKAGE:  
//
// AUTHOR:   Natanael Nunes de Moura Junior
//
//
// ********************************************************************

#include "TileMonitoring/TileDQFragLWMonTool.h"

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileDCSSvc.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileRecUtils/TileRawChannelBuilder.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"

/*---------------------------------------------------------*/
TileDQFragLWMonTool::TileDQFragLWMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileBadChanTool("TileBadChanTool")
  , m_tileDCSSvc("TileDCSSvc",name)
  , m_beamInfo("TileBeamInfoProvider")
  , m_dqStatus(0)
  , m_globalErrCount{}
  , m_global{}
  , m_mismatchedL1TriggerType{nullptr}
  , m_badChannelJump2D{}
  , m_badChannelNeg2D{}
  , m_badChannelJump2DNotMasked{}
  , m_badChannelNeg2DNotMasked{}
  , m_errors{{}}
  , m_errorsLB{{}}
  , m_badPulseQuality{nullptr}
  , m_noAllDigitsInDrawer(nullptr)
  , m_isFirstEvent(true)
  , m_nLumiblocks(3000)
  , m_qualityCut(254.0)
  , m_nEventsWithAllDigits(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("TileRawChannelContainer", m_rawChannelContainerName = "TileRawChannelFixed"); //SG offline RCh Container
  declareProperty("TileDigitsContainer",     m_digitsContainerName =  "TileDigitsCnt");     // Digits
  declareProperty("NegAmpHG", m_negAmpHG = -200.);  // cut on negative amplitude in high gain
  declareProperty("NegAmpLG", m_negAmpLG = -15.);   // cut on negative amplitude in low gain
  declareProperty("JumpDeltaHG",  m_jumpDeltaHG = 99.9); // minimal jump in high gain
  declareProperty("JumpDeltaLG",  m_jumpDeltaLG = 49.9); // minimal jump in low gain
  declareProperty("PedDetlaHG",   m_pedDeltaHG  =  4.1); // max variation of "const" value in high gain
  declareProperty("PedDetlaLG",   m_pedDeltaLG  =  4.1); // max variation of "const" value in low gain
  declareProperty("SkipMasked",   m_skipMasked = true);  // ignore masked channels or not
  declareProperty("SkipGapCells", m_skipGapCells = true);// ignore MBTS and E1-E4 cells in negE detection
  declareProperty("doOnline", m_doOnline = false); // online mode
  declareProperty("CheckDCS", m_checkDCS = false);
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("NumberOfLumiblocks", m_nLumiblocks = 3000);
  declareProperty("QualityCut", m_qualityCut = 254.0);

  m_path = "/Tile/DMUErrors";
  // starting up the label variable....
  m_errorsLabels.clear();
  m_errorsLabels.push_back("OK");                 // Error: 0
  m_errorsLabels.push_back("HEADER_FORM");        // Error: 1
  m_errorsLabels.push_back("HEADER_PAR");         // Error: 2
  m_errorsLabels.push_back("MEMO_PAR");           // Error: 3
  m_errorsLabels.push_back("FE_CRC");             // Error: 4
  m_errorsLabels.push_back("ROD_CRC");            // Error: 5
  m_errorsLabels.push_back("BCID");               // Error: 6
  m_errorsLabels.push_back("SAMPLE_FORM");        // Error: 7
  m_errorsLabels.push_back("SAMPLE_PAR");         // Error: 8
  m_errorsLabels.push_back("DOUBLE_STB");         // Error: 9
  m_errorsLabels.push_back("SINGLE_STB");         // Error: 10
  m_errorsLabels.push_back("GLOBAL_CRC");         // Error: 11
  m_errorsLabels.push_back("DUMMY_FRAG");         // Error: 12
  m_errorsLabels.push_back("NO_RECO_FFAG");       // Error: 13
  m_errorsLabels.push_back("MASKED");             // Error: 14
  m_errorsLabels.push_back("ALL_M_BAD_DCS");      // Error: 15
  m_errorsLabels.push_back("ANY_CH_BAD_HV");      // Error: 16
  assert( m_errorsLabels.size() == NERROR );
  // corrupted data
  m_errorsLabels.push_back("0 -> 1023");          // Error: NERROR - 1 + 1
  m_errorsLabels.push_back("Zeros");              // Error: NERROR - 1 + 2
  m_errorsLabels.push_back("Two 1023 + ped");     // Error: NERROR - 1 + 3
  m_errorsLabels.push_back("Jump 2 levels");      // Error: NERROR - 1 + 4
  m_errorsLabels.push_back("Single Up + ped");    // Error: NERROR - 1 + 5
  m_errorsLabels.push_back("Single Dn + ped");    // Error: NERROR - 1 + 6
  m_errorsLabels.push_back("Single Up + sig");    // Error: NERROR - 1 + 7
  m_errorsLabels.push_back("Single Dn + sig");    // Error: NERROR - 1 + 8
  m_errorsLabels.push_back("Ped>200 LG");         // Error: NERROR - 1 + 9
  m_errorsLabels.push_back("Single Dn LG_s0");    // Error: NERROR - 1 + 10
  m_errorsLabels.push_back("Single Dn LG_s6");    // Error: NERROR - 1 + 11
  m_errorsLabels.push_back("Up LG_s0_s6 or Gap"); // Error: NERROR - 1 + 12
  m_errorsLabels.push_back("Dn LG_s0_s6 or Gap"); // Error: NERROR - 1 + 13
  assert( m_errorsLabels.size() == (NERROR+NCORRUPTED) );
  
  m_partitionsLabels.push_back("LBA");
  m_partitionsLabels.push_back("LBC");
  m_partitionsLabels.push_back("EBA");
  m_partitionsLabels.push_back("EBC");
  
  
  m_PartNames[PartEBA] = "EBA";
  m_PartNames[PartLBA] = "LBA";
  m_PartNames[PartLBC] = "LBC";
  m_PartNames[PartEBC] = "EBC";

  std::vector<unsigned int> roses = {3, 1, 2, 4};
 
  for (int part = 0; part < NumPart; ++part) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      if (drawer % 2 == 1) m_moduleLabel[part].push_back(" ");
      else m_moduleLabel[part].push_back(TileCalibUtils::getDrawerString(roses[part], drawer));
    }
    
    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      std::string cell_name = getCellName(roses[part], channel);
      if (cell_name.empty()) cell_name += "ch";
      else cell_name += "_ch";
       m_cellchLabel[part].push_back(cell_name + std::to_string(channel));
    }
  }
  
}

/*---------------------------------------------------------*/
TileDQFragLWMonTool::~TileDQFragLWMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileDQFragLWMonTool:: initialize() {
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
StatusCode TileDQFragLWMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileDQFragLWMonTool::bookFirstEventHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in bookFirstEventHistograms" );

  std::string runNumStr = getRunNumStr();      
    
    
  // The First Global Histogram
  std::vector<std::string> globalName,globalTitle;
  globalName.push_back("TileGlobalCRC_errors_top");
  globalName.push_back("TileGlobalCRC_errors_bottom");
  globalTitle.push_back("Run " + runNumStr + ": Global Error Top [1,32] (entries = events)");
  globalTitle.push_back("Run " + runNumStr + ": Global Error Bottom[33,64] (entries = events)");
    
  m_global[0] = book2ILW("", globalName[0], globalTitle[0], 32, 0.5, 32.5, 4, 0., 4.);
  m_global[0]->GetXaxis()->SetTitle("Drawer");
  SetBinLabel(m_global[0]->GetYaxis(), m_partitionsLabels);
    
  // The Second Global Histogram
  m_global[1] = book2ILW("", globalName[1], globalTitle[1], 32, 32.5, 64.5, 4, 0., 4);
  SetBinLabel(m_global[1]->GetYaxis(), m_partitionsLabels);
  m_global[1]->GetXaxis()->SetTitle("Drawer");

  // The Global Histogram with mismatched trigger types
  std::vector<std::string> partitionsLabels{"LBA", "LBC", "EBA", "EBC"};
  m_mismatchedL1TriggerType = book2ILW("", "TileMismatchedL1TriggerType",
                                       "Run " + runNumStr + ": Mismatched L1 Trigger Type (entries = events)", 
                                       64, 0.5, 64.5, 4, 1., 5.);

  m_mismatchedL1TriggerType->GetXaxis()->SetTitle("Module");
  SetBinLabel(m_mismatchedL1TriggerType->GetYaxis(), partitionsLabels);

  // The Global Histogram with incomplete set of digits in a drawer
  m_noAllDigitsInDrawer = book2ILW("", "TileNoAllDigits",
                                   "Run " + runNumStr + ": No all digits in event with Trigger Type = 0x82 (entries = events)", 
                                   64, 0.5, 64.5, 4, 1., 5.);
 
  m_noAllDigitsInDrawer->GetXaxis()->SetTitle("Module");
  SetBinLabel(m_noAllDigitsInDrawer->GetYaxis(), partitionsLabels);

  for (unsigned int module = 1; module < TileCalibUtils::MAX_DRAWER; ++module) {
    if (module % 2 != 0) {
      std::string moduleName = (module < 10) ? "0" : "";
      moduleName += std::to_string(module);
      m_mismatchedL1TriggerType->GetXaxis()->SetBinLabel(module, moduleName.c_str());
      m_noAllDigitsInDrawer->GetXaxis()->SetBinLabel(module, moduleName.c_str());
    };
  }
  
  // Histograms of bad drawers
  std::string badDrawersDir = "BadDrawers";

  if (m_rawChannelContainerName.size() > 0) {

    for (int p = 0; p < NumPart; p++) {

      m_badChannelNeg2D[p] = book2ILW(badDrawersDir, "TileBadChannelsNegMap" + m_PartNames[p],
                                      "# Negative Amp in " + m_PartNames[p], 64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(m_badChannelNeg2D[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(m_badChannelNeg2D[p]->GetYaxis(), m_cellchLabel[p]);
      
      m_badChannelNeg2DNotMasked[p] = book2ILW(badDrawersDir, "TileBadChannelsNegMapNonMask" + m_PartNames[p],
                                               "# Not Masked Negative Amp in " + m_PartNames[p], 64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(m_badChannelNeg2DNotMasked[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(m_badChannelNeg2DNotMasked[p]->GetYaxis(), m_cellchLabel[p]);

      m_badPulseQuality[p] = book2ILW(badDrawersDir, "TileBadPulseQuality" + m_PartNames[p],
                                      "Run " + runNumStr + ": " + m_PartNames[p] +
                                      " Bad pulse shape or #chi^{2} from Optimal filtering algorithm",
                                      64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(m_badPulseQuality[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(m_badPulseQuality[p]->GetYaxis(), m_cellchLabel[p]);

    }
  } else {

    m_badChannelNeg2D[0] = nullptr;
    m_badChannelNeg2D[1] = nullptr;
    m_badChannelNeg2D[2] = nullptr;
    m_badChannelNeg2D[3] = nullptr;

    m_badChannelNeg2DNotMasked[0] = nullptr;
    m_badChannelNeg2DNotMasked[1] = nullptr;
    m_badChannelNeg2DNotMasked[2] = nullptr;
    m_badChannelNeg2DNotMasked[3] = nullptr;
  }

  if (m_digitsContainerName.size() > 0) {

    for (int p = 0; p < NumPart; p++) {

      m_badChannelJump2D[p] = book2ILW(badDrawersDir, "TileBadChannelsJumpMap" + m_PartNames[p],
                                       "# Jump errors in " + m_PartNames[p], 64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(m_badChannelJump2D[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(m_badChannelJump2D[p]->GetYaxis(), m_cellchLabel[p]);
      
      m_badChannelJump2DNotMasked[p] = book2ILW(badDrawersDir, "TileBadChannelsJumpMapNonMask" + m_PartNames[p],
                                                "# not masked Jump errors in " + m_PartNames[p], 64, 0.5, 64.5, 48, -0.5, 47.5);
      SetBinLabel(m_badChannelJump2DNotMasked[p]->GetXaxis(), m_moduleLabel[p]);
      SetBinLabel(m_badChannelJump2DNotMasked[p]->GetYaxis(), m_cellchLabel[p]);
    }
  } else {

    m_badChannelJump2D[0] = nullptr;
    m_badChannelJump2D[1] = nullptr;
    m_badChannelJump2D[2] = nullptr;
    m_badChannelJump2D[3] = nullptr;

    m_badChannelJump2DNotMasked[0] = nullptr;
    m_badChannelJump2DNotMasked[1] = nullptr;
    m_badChannelJump2DNotMasked[2] = nullptr;
    m_badChannelJump2DNotMasked[3] = nullptr;
  }

  memset(m_globalErrCount, 0, sizeof(m_globalErrCount));

  for (unsigned int ros = 0; ros < 4; ros++) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer){
      bookErrorsHistograms(ros, drawer);
    }
  }
  
  
}
/*---------------------------------------------------------*/



/*---------------------------------------------------------*/
void TileDQFragLWMonTool::bookErrorsHistograms(unsigned int ros, unsigned int drawer) {
/*---------------------------------------------------------*/


  std::string moduleName = TileCalibUtils::getDrawerString(ros + 1, drawer);
  std::string histName;
  std::string histTitle;
  std::string histlbName;
  std::string histlbTitle;
 
  std::string runNumStr = getRunNumStr();
 
  histTitle = "Run " + runNumStr + ": " + moduleName + " DMU Header Errors";

  histName = "TileDigiErrors" + moduleName;
 
  histlbTitle = "Run " + runNumStr + ": " + moduleName + " (#total_events - #ok_events) / (#total_events)";
  histlbName = "FracTileDigiErrors" + moduleName;
 
   
  m_errors[ros][drawer] = book2ILW("", histName.c_str(), histTitle.c_str(), 
                                   16, 0.0, 16.0, NERROR + NCORRUPTED, 0., NERROR + NCORRUPTED);
  m_errors[ros][drawer]->GetXaxis()->SetTitle("DMU");
  SetBinLabel(m_errors[ros][drawer]->GetYaxis(), m_errorsLabels);  

  m_errorsLB[ros][drawer] = bookProfileLW("", histlbName.c_str(), histlbTitle.c_str(), m_nLumiblocks, -0.5, m_nLumiblocks - 0.5, -1.1, 1.1);
  m_errorsLB[ros][drawer]->GetXaxis()->SetTitle("LumiBlock");

  m_errorsLB[ros][drawer]->GetYaxis()->SetTitle("Fraction of Digital errors");


}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
StatusCode TileDQFragLWMonTool::fillHistograms() {
/*---------------------------------------------------------*/


  fillEvtInfo();

  if (m_isFirstEvent) { // first event of the run, fill masked dmu
    bookFirstEventHistograms();
    fillMasking();
    m_isFirstEvent = false;
  }

  if (getL1info() == 0x82) {
    ++m_nEventsWithAllDigits;
  }

  fillBadDrawer();
  for (unsigned int ros = 0; ros < 4; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer){
      fillErrorsHistograms(ros, drawer);
      fillGlobalHistograms(ros, drawer);
    }
  }

  m_global[0]->SetEntries(m_nEvents);
  m_global[1]->SetEntries(m_nEvents);

  m_mismatchedL1TriggerType->SetEntries(m_nEvents);
  m_noAllDigitsInDrawer->SetEntries(m_nEventsWithAllDigits);

  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragLWMonTool::fillBadDrawer() {
/*---------------------------------------------------------*/


  m_dqStatus = m_beamInfo->getDQstatus();


  uint32_t lvl1info = getL1info();
  bool phys = (lvl1info == 0) || (((lvl1info >> Trig_b7) & 1) == 1);
  if (phys && m_rawChannelContainerName.size() > 0) {
    const TileRawChannelContainer* rawChannelContainer;

    if(evtStore()->retrieve(rawChannelContainer, m_rawChannelContainerName).isFailure()) {
      ATH_MSG_WARNING( "Can't retrieve offline RawChannel from TES" );
      m_rawChannelContainerName = "";
    } else {

      TileRawChannelUnit::UNIT rChUnit = rawChannelContainer->get_unit();
      if (rChUnit != TileRawChannelUnit::ADCcounts && rChUnit != TileRawChannelUnit::OnlineADCcounts ) {
        ATH_MSG_WARNING( "RawChannel units are not ADC counts - WILL NOT CHECK NEGATIVE AMPLITUDES" );

      } else {
          
        const TileDigitsContainer* digitsContainer = 0;
        TileDigitsContainer::const_iterator digitsCollectionIt;
        TileDigitsContainer::const_iterator lastDigitsCollectionIt;
        if (m_digitsContainerName.size() > 0) {
          if(evtStore()->retrieve(digitsContainer, m_digitsContainerName).isFailure()) {
            ATH_MSG_WARNING( "Can't retrieve Digits from TES" );
            m_digitsContainerName = "";
            digitsContainer = 0;
          } else {
            digitsCollectionIt  = digitsContainer->begin();
            lastDigitsCollectionIt = digitsContainer->end();
          }
        }
    
        int nBadCh[2] = {0, 0};
        int nBadChNM[2] = {0, 0};
       
        for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

          int fragId = rawChannelCollection->identify();
          int drawer = (fragId & 0x3F); // range 0-63
          int module = drawer + 1;  // range 1-64
          int ROS = fragId >> 8;  // range 1-4
          int partition = m_ros2partition[ROS]; // range 0-3

          for (const TileRawChannel* rawChannel : *rawChannelCollection) {

            HWIdentifier adcId = rawChannel->adc_HWID();
            int channel = m_tileHWID->channel(adcId);
            int gain = m_tileHWID->adc(adcId);

            float pedestal = rawChannel->pedestal(); // errors are saved in ped as 100000 + 10000*error
            float quality = rawChannel->quality(); //

            if ((pedestal > 80000. || quality > m_qualityCut)
                && !(m_tileBadChanTool->getAdcStatus(adcId).isBad()
                     || (m_checkDCS && m_tileDCSSvc->statusIsBad(ROS, drawer, channel)))) {

              m_badPulseQuality[partition]->Fill(module, channel);

            }

            if (findChannelErrors(rawChannel, gain) > 0) {
              ++nBadCh[gain];

              m_badChannelNeg2D[partition]->Fill(module, channel);
              if (isModuleDCSgood(ROS, drawer) && m_dqStatus->isChanDQgood(ROS, drawer, channel)) {
                ++nBadChNM[gain];
                m_badChannelNeg2DNotMasked[partition]->Fill(module, channel);

                if (pedestal > 100000. && digitsContainer) {
                  while (((*digitsCollectionIt)->identify() != fragId) && (digitsCollectionIt != lastDigitsCollectionIt)) {
                    ++digitsCollectionIt;
                  }

                  if (digitsCollectionIt != lastDigitsCollectionIt) {

                    for (const TileDigits* pDigits : **digitsCollectionIt) {

                      if (m_tileHWID->channel(pDigits->adc_HWID()) == channel) {

                        msg(MSG::INFO) << "LB " << getLumiBlock()
                                       << " Evt " << getEvtNum()
                                       << " " << m_PartNames[partition]
                                       << std::setfill('0') << std::setw(2) << module
                                       << std::setfill(' ') << std::setw(3) << channel
                                       << ((gain)?" HG":" LG") << " neg amp: "
                                       << std::setprecision(2) << std::fixed << std::setw(7)
                                       << rawChannel->amplitude()
                                       << "   Samples: " << std::setprecision(0);

                        std::vector<float> vdigits = pDigits->samples();
                        unsigned int nSamp = vdigits.size();
                        for (unsigned int i = 0; i < nSamp; ++i) {
                          msg(MSG::INFO) << vdigits[i] << " ";
                        }

                        msg(MSG::INFO) << "  error = " << TileRawChannelBuilder::BadPatternName(pedestal) << endmsg;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }


  if (m_digitsContainerName.size() > 0) {
    const TileDigitsContainer* digitsContainer;

    if (evtStore()->retrieve(digitsContainer, m_digitsContainerName).isFailure()) {
      ATH_MSG_WARNING("Can't retrieve Digits from TES");
      m_digitsContainerName = "";
    } else {

      uint16_t jumps_corruption[4][64][NCORRUPTED] = { { { 0u } } };

      int nBadDr = 0;
      int nBadDrNM = 0;

      for (const TileDigitsCollection* digitsCollection : *digitsContainer) {

        int nBadCh = 0;
        int nBadChNM = 0;

        int fragId = digitsCollection->identify();
        int drawer = (fragId & 0x3F); // range 0-63
        int module = drawer + 1;  // range 1-64
        int ROS = fragId >> 8;  // range 1-4
        int partition = m_ros2partition[ROS]; // range 0-3

        if (getL1info() != digitsCollection->getLvl1Type()) {
          m_mismatchedL1TriggerType->Fill(module, ROS, 1.0);
        }

        if (getL1info() == 0x82) {

          unsigned int nBadOrDisconnectedChannels(0);
          unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ROS, drawer);
          for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
            if (m_tileBadChanTool->getChannelStatus(drawerIdx, channel).isBad()
                || (isDisconnected(ROS, drawer, channel)) ) {
              ++nBadOrDisconnectedChannels;
            }
          }

          unsigned int nRequiredChannels(TileCalibUtils::MAX_CHAN - nBadOrDisconnectedChannels);
          if (digitsCollection->size() < nRequiredChannels) {
            m_noAllDigitsInDrawer->Fill(module, ROS, 1.0);
            ATH_MSG_VERBOSE("No All channels with digits (Trigger Type: 0x82) in module " 
                            << TileCalibUtils::getDrawerString(ROS, drawer) 
                            << ", present channels: " << digitsCollection->size()
                            << ", required channels: " << nRequiredChannels);
          }

        }

        int error;
        float dmin, dmax;

        for (const TileDigits* pDigits : *digitsCollection) {

          HWIdentifier adcId = pDigits->adc_HWID();
          int channel = m_tileHWID->channel(adcId);
          int gain = m_tileHWID->adc(adcId);

          error = TileRawChannelBuilder::CorruptedData(ROS, drawer, channel, gain, pDigits->samples(), dmin, dmax);

          if ( (error > 0) &&
              !(isDisconnected(ROS, drawer, channel) || m_tileBadChanTool->getAdcStatus(adcId).isBad()) ) {
            ++nBadCh;
            if (msgLvl(MSG::DEBUG)) {
              msg(MSG::DEBUG) << "LB " << getLumiBlock()
                              << " Evt " << getEvtNum()
                              << " " << m_PartNames[partition] << std::setfill('0') << std::setw(2) << module
                              << std::setfill(' ') << std::setw(3) << channel
                              << ((gain) ? " HG" : " LG")
                              << "   Samples: ";

              std::vector<float> vdigits = pDigits->samples();
              unsigned int nSamp = vdigits.size();
              for (unsigned int i = 0; i < nSamp; ++i){
                msg(MSG::DEBUG) << vdigits[i] << " ";
              }
              msg(MSG::DEBUG) << "  error = " << TileRawChannelBuilder::BadPatternName(100000. + error * 10000) << endmsg;
            }
          

            m_badChannelJump2D[partition]->Fill(module, channel);
            if (isModuleDCSgood(ROS, drawer) && m_dqStatus->isChanDQgood(ROS, drawer, channel)) {
              ++nBadChNM;
              m_badChannelJump2DNotMasked[partition]->Fill(module, channel);
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

      for (unsigned int partition = 0; partition < NumPart; partition++)
        for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer)
          for (int error = 0; error < NCORRUPTED; error++)
            if (jumps_corruption[partition][drawer][error] > 0u)
              for (unsigned int dmu = 0u; dmu < 16u; dmu++)
                if (jumps_corruption[partition][drawer][error] & (1u << dmu))
                  fillOneError(partition, drawer, dmu, NERROR + error);
    }
  }
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragLWMonTool::fillOneError(unsigned int ros, unsigned int drawer, int dmu, int error) {
/*---------------------------------------------------------*/
  m_errors[ros][drawer]->Fill(dmu, error);
}

/*---------------------------------------------------------*/
void TileDQFragLWMonTool::fillErrorsHistograms(unsigned int ros, unsigned int drawer) {
/*---------------------------------------------------------*/


  int n_error_nonmask_DMU = 0;
  unsigned int cur_lb = getLumiBlock();

  m_dqStatus = m_beamInfo->getDQstatus();

  if (!isModuleDCSgood(ros + 1, drawer)) {

    m_errorsLB[ros][drawer]->Fill(cur_lb, -1.);

    for (int idmu = 0; idmu < NDMU; ++idmu) fillOneError(ros, drawer, idmu, DCSERROR);

  } else {
    int status = m_dqStatus->checkGlobalErr(ros + 1, drawer, 0);
    int err = 0;
    if (status & (TileFragStatus::ALL_FF | TileFragStatus::ALL_00)) {
      err = 12;
    } else if (status & (TileFragStatus::NO_FRAG | TileFragStatus::NO_ROB)) {
      err = 13;
    }

    for (int idmu = 0; idmu < NDMU; idmu++) { // loop over dmus
      int ichn = 3 * idmu;

      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros + 1, drawer);
      if (m_checkDCS 
          && ((m_tileDCSSvc->statusIsBad(ros + 1, drawer, ichn)
               && !m_tileBadChanTool->getChannelStatus(drawerIdx, ichn).contains(TileBchPrbs::NoHV)
               && !m_tileBadChanTool->getChannelStatus(drawerIdx, ichn).contains(TileBchPrbs::WrongHV))
              || (m_tileDCSSvc->statusIsBad(ros + 1, drawer, ichn + 1) 
                  && !m_tileBadChanTool->getChannelStatus(drawerIdx, ichn + 1).contains(TileBchPrbs::NoHV)
                  && !m_tileBadChanTool->getChannelStatus(drawerIdx, ichn + 1).contains(TileBchPrbs::WrongHV))
              || (m_tileDCSSvc->statusIsBad(ros + 1, drawer, ichn + 2) 
                  && !m_tileBadChanTool->getChannelStatus(drawerIdx, ichn + 2).contains(TileBchPrbs::NoHV)
                  && !m_tileBadChanTool->getChannelStatus(drawerIdx, ichn + 2).contains(TileBchPrbs::WrongHV)))) {
        
        fillOneError(ros, drawer, idmu, HVERROR);
      }

      if (m_dqStatus->isChanDQgood(ros + 1, drawer, ichn)) {
        fillOneError(ros, drawer, idmu, 0);
      } else {
        if (checkHasErrors(ros, drawer, idmu)) n_error_nonmask_DMU++;

        if (err) {
          fillOneError(ros, drawer, idmu, err);
        } else if (m_dqStatus->checkHeaderFormatErr(ros + 1, drawer, idmu, 0) != 0) { // in case of format errors, we only fill this one
          fillOneError(ros, drawer, idmu, 1);
        } else {
          if (m_dqStatus->checkHeaderParityErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 2);
          }
          if (m_dqStatus->checkMemoryParityErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 3);
          }
          if (m_dqStatus->checkFE_CRCErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 4);
          }
          if (m_dqStatus->checkROD_CRCErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 5);
          }
          if (m_dqStatus->checkBCIDErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 6);
          }
          if (m_dqStatus->checkSampleFormatErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 7);
          }
          if (m_dqStatus->checkSampleParityErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 8);
          }
          if (m_dqStatus->checkDoubleStrobeErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 9);
          }
          if (m_dqStatus->checkSingleStrobeErr(ros + 1, drawer, idmu, 0) != 0) {
            fillOneError(ros, drawer, idmu, 10);
          }
          if (m_dqStatus->checkGlobalCRCErr(ros + 1, drawer, 0) != 0) {
            fillOneError(ros, drawer, idmu, 11);
          }
        }
      }
    } // loop over dmus

      m_errorsLB[ros][drawer]->Fill(cur_lb, (double) n_error_nonmask_DMU / 16.);
  }

} //end of function
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragLWMonTool::fillGlobalHistograms(unsigned int ros, unsigned int drawer) {
  /*---------------------------------------------------------*/

  if (m_dqStatus->checkGlobalCRCErr(ros + 1, drawer, 0) != 0) { //if
    m_globalErrCount[ros][drawer]++;
    if (drawer < 32) {
      m_global[0]->Fill(drawer + 1, (ros));
    } else { // else
      m_global[1]->Fill(drawer + 1, (ros));
    } //else
  } // if
}
/*---------------------------------------------------------*/



/*---------------------------------------------------------*/
StatusCode TileDQFragLWMonTool::procHistograms() {
  /*---------------------------------------------------------*/

  for (unsigned int ros = 0; ros < 4; ++ros) {  //loop over ros...
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      for (int iDMU = 1; iDMU <= NDMU; ++iDMU) {
        if (m_errors[ros][drawer]) {
          if (m_errors[ros][drawer]->GetBinContent(iDMU, NERROR - 2) > 0)
            m_errors[ros][drawer]->SetBinContent(iDMU, NERROR - 2, m_nEvents - 1);
          m_errors[ros][drawer]->SetEntries(m_nEvents - 1);
        }
      }  // ...dmus
    }  //...drawers
  }  // loop over ros...

  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void TileDQFragLWMonTool::fillMasking() {
/*---------------------------------------------------------*/

  bool sp_EB; // special treatment of EBA15, EBC18
  for (unsigned int ros = 0; ros < 4; ++ros) { //LF careful ros is defined in range 1-4, but here goes in range 0-3.
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      unsigned int idx = TileCalibUtils::getDrawerIdx(ros + 1, drawer);
      for (int ch = 0; ch < 48; ch += 3) {
        int dmu = ch / 3;
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
          m_errors[ros][drawer]->Fill(dmu, MASKEDERROR, 1);
        } else if ((ros > 1) && ((ch == 18 && !sp_EB) || ch == 33)) { //disconnected channels for EBs
          if (status2.isBad()) m_errors[ros][drawer]->Fill(dmu, MASKEDERROR, 1);
        } else if ((ros < 2) && (ch == 30)) { //disconnected channels for LBs
          if (status2.isBad()) m_errors[ros][drawer]->Fill(dmu, MASKEDERROR, 1);
        } else if ((ros < 2) && (ch == 42)) { //disconnected channels for LBs
          if (status0.isBad() && status2.isBad()) m_errors[ros][drawer]->Fill(dmu, MASKEDERROR, 1);
        } else if ((ros > 1) && (ch == 24 || ch == 27 || ch == 42 || ch == 45)) { // void DMUs for EBs
          m_errors[ros][drawer]->Fill(dmu, MASKEDERROR, 1);
        } else if (sp_EB && (ch == 0)) { // void DMU 0 for EBA15, EBC18
          m_errors[ros][drawer]->Fill(dmu, MASKEDERROR, 1);
        } else if (sp_EB && (ch == 3)) { // disconnected PMT of DMU 1 for EBA15, EBC18
          if (status1.isBad() && status2.isBad()) m_errors[ros][drawer]->Fill(dmu, MASKEDERROR, 1);
        }
      } //chan loop
    } // drawer loop
  } // ros loop


}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
bool TileDQFragLWMonTool::checkHasErrors(unsigned int ros, unsigned int drawer, int dmu) {
/*---------------------------------------------------------*/

  if (ros < 2) {
    if (m_errors[ros][drawer]->GetBinContent(dmu + 1, MASKEDERROR + 1) > 0) return false;
  } else {
    if ((dmu == 8) || (dmu == 9) || (dmu == 14) || (dmu == 15)
        || (m_errors[ros][drawer]->GetBinContent(dmu + 1, MASKEDERROR + 1) > 0))
      return false;
    else if (((drawer == 14) && (ros == 2)) || ((drawer == 17) && (ros == 3))) {
      if (dmu == 0) return false;
    }
  }

  return true;

}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
bool TileDQFragLWMonTool::isModuleDCSgood(unsigned int partition, unsigned int drawer) const {
/*---------------------------------------------------------*/

  if (m_checkDCS) {
    TileDCSSvc::TileDCSStatus Status = m_tileDCSSvc->getDCSSTATUS(partition, drawer);
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
int TileDQFragLWMonTool::findDrawerErrors(const TileRawChannelCollection *coll) {
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
int TileDQFragLWMonTool::findChannelErrors(const TileRawChannel *rch, int & gain) {
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

