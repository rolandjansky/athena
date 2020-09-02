/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileRawChannelMonTool.cxx
// PACKAGE:  
//
// AUTHOR:   Lukas Plazak
//
//
// ********************************************************************

// Tile includes
#include "TileMonitoring/TileRawChannelTimeMonTool.h"

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileRawChannelContainer.h"

// Athena includes
#include "StoreGate/ReadHandle.h"

#include "TProfile.h"
#include "TProfile2D.h"


#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>


/*---------------------------------------------------------*/
TileRawChannelTimeMonTool::TileRawChannelTimeMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileBadChanTool("TileBadChanTool")
  , m_dqStatus(0)
  , m_doOnline(false)
  , m_oldLumiblock(-1)
  , m_deltaLumiblock(0)
  , m_nEvents(0)
  , m_bookProfHistOnce(5, false)
  , m_profile2dHist{}
  , m_profileHist{}
  , m_partitionTimeLB{}
  , m_thresholds{10.0, 40.0}
  , m_timeCorrectionLBA(-15.18)
  , m_timeCorrectionLBC(-15.37)
  , m_timeCorrectionEBA(47.65)
  , m_timeCorrectionEBC(47.42)
  , m_partitionTimeCorrection{{0}}
  , m_timeDifferenceBetweenROS{{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}}
  , m_nLumiblocks(3000)
	 /*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

	// run type 1 - phys, 2 - las, 4 - ped, 8 - cis
  declareProperty("runType", m_runType = 0);
  declareProperty("LowGainThreshold", m_lowGainThreshold = 10.0);
  declareProperty("HiGainThreshold", m_hiGainThreshold = 40.0);
  declareProperty("bigain", m_bigain = true);
  declareProperty("doOnline"               , m_doOnline = false); //online mode
  declareProperty("TileRawChannelContainer", m_contName = "TileRawChannelOpt2"); //SG RC Container
  declareProperty("TileBadChanTool"        , m_tileBadChanTool);
  declareProperty("TimeCorrectionLBA", m_timeCorrectionLBA);
  declareProperty("TimeCorrectionLBC", m_timeCorrectionLBC);
  declareProperty("TimeCorrectionEBA", m_timeCorrectionEBA);
  declareProperty("TimeCorrectionEBC", m_timeCorrectionEBC);
  declareProperty("NumberOfLumiblocks", m_nLumiblocks = 3000);
  declareProperty("TileDQstatus", m_DQstatusKey = "TileDQstatus");
  declareProperty("CheckDCS", m_checkDCS = false);

  m_path = "/Tile/RawChannelTime"; 
}

/*---------------------------------------------------------*/
TileRawChannelTimeMonTool::~TileRawChannelTimeMonTool()
  /*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelTimeMonTool::initialize()
  /*---------------------------------------------------------*/
{

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK(m_tileBadChanTool.retrieve());

  m_nEvents = 0;
  m_thresholds[0] = m_lowGainThreshold;
  m_thresholds[1] = m_hiGainThreshold;

  m_partitionTimeCorrection = {{0, 
                                m_timeCorrectionLBA, 
                                m_timeCorrectionLBC, 
                                m_timeCorrectionEBA, 
                                m_timeCorrectionEBC}};

  ATH_CHECK(TileFatherMonTool::initialize());

  ATH_CHECK( m_DQstatusKey.initialize() );

  if (m_checkDCS) {
    ATH_CHECK( m_tileDCS.retrieve() );
  } else {
    m_tileDCS.disable();
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelTimeMonTool::bookHists()
  /*---------------------------------------------------------*/
{
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "in bookHists()" << endmsg;
    msg(MSG::DEBUG) << "Using base path " << m_path << endmsg;
  }

  std::string partitionName[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };

  std::string run("Run ");
  if (m_manager) run += std::to_string(m_manager->runNumber());
  else run += "unknown";

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {

      m_partitionTimeLB[ros] =  book2F("Summary", "TileAverageTimeLB_" + partitionName[ros], 
                                       run + " Partition " + partitionName[ros] + ": Average time vs LumiBlock;LumiBlocks;t [ns]", 
                                       m_nLumiblocks, -0.5, m_nLumiblocks - 0.5, 151, -75, 75);

    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      bookHists(ros, drawer);
    }
  }

  for (const std::pair<int, int>& roses : m_timeDifferenceBetweenROS) {
    int ros1 = roses.first;
    int ros2 = roses.second;
    
    if (m_doOnline) {
      m_partitionTimeDifferenceLB.push_back( bookProfile("Summary", "TileAverageTimeDifferenceLB_" + partitionName[ros1] + "-" + partitionName[ros2],
                                                         run + ": Average time between " + partitionName[ros1] + " and " +  partitionName[ros2] + 
                                                         " vs LumiBlock;Last LumiBlocks;t [ns]",
                                                         100, -99.5, 0.5) );
    } else {
      m_partitionTimeDifferenceLB.push_back( bookProfile("Summary", "TileAverageTimeDifferenceLB_" + partitionName[ros1] + "-" + partitionName[ros2],
                                                         run + ": Average time between " + partitionName[ros1] + " and " +  partitionName[ros2] + 
                                                         " vs LumiBlock;LumiBlocks;t [ns]",
                                                         m_nLumiblocks, -0.5, m_nLumiblocks - 0.5) );
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileRawChannelTimeMonTool::bookHists(int ros, int drawer) {
  /*---------------------------------------------------------*/

  std::string partitionName[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };

  std::string run("Run ");
  if (m_manager) run += std::to_string(m_manager->runNumber());
  else run += "unknown";


  std::string moduleName = TileCalibUtils::getDrawerString(ros, drawer);
  std::string subDir = moduleName;
  std::string histName, histTitle;

  ATH_MSG_DEBUG("in bookHists() for module " << moduleName);

  for(int dig = 0; dig < 8; ++dig){

    histName = moduleName +  "_digitizer_" + std::to_string(dig + 1);
    histTitle = run + ", " + moduleName + ", Digi" + std::to_string(dig + 1) + ": Time vs LumiBlock";

    if(m_doOnline){
      m_profileHist[ros][drawer][dig]=bookProfile(subDir, histName, histTitle, 100, -99.5, 0.5);
      m_profileHist[ros][drawer][dig]->GetXaxis()->SetTitle("Last LumiBlocks");
    }else{
      m_profileHist[ros][drawer][dig]=bookProfile(subDir, histName, histTitle, m_nLumiblocks, -0.5, m_nLumiblocks - 0.5);
      m_profileHist[ros][drawer][dig]->GetXaxis()->SetTitle("LumiBlock");
    }
  }

  if (!m_bookProfHistOnce[ros]) {

    histName = partitionName[ros];
    histName += "_avgTime";

    histTitle = run + " Partition " + partitionName[ros] + ": Average Time [ns]";

    subDir = "Summary";

    m_profile2dHist[ros] = bookProfile2D(subDir, histName, histTitle, 64, 0.5, 64.5, 48, -0.5, 47.5, -50, 50);

    std::string module_name;
    std::string cell_name;
    std::string channel_name;

    for (unsigned int drw = 0; drw < TileCalibUtils::MAX_DRAWER; drw += 2) {
      module_name = TileCalibUtils::getDrawerString(ros, drw);
      m_profile2dHist[ros]->GetXaxis()->SetBinLabel(drw + 1, module_name.c_str());
    }

    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      cell_name = getCellName(ros, channel);
      channel_name = cell_name + (cell_name.empty() ? "ch" : "_ch") + std::to_string(channel);
      m_profile2dHist[ros]->GetYaxis()->SetBinLabel(channel + 1, channel_name.c_str());
    }

    m_bookProfHistOnce[ros] = true;
  }

}

/*---------------------------------------------------------*/
StatusCode TileRawChannelTimeMonTool::fillHists()
  /*---------------------------------------------------------*/
{

  static const int ch2digi[48] = {7, 7, 7,  7, 7, 7,
                                  6, 6, 6,  6, 6, 6,
                                  5, 5, 5,  5, 5, 5,
                                  4, 4, 4,  4, 4, 4,
                                  3, 3, 3,  3, 3, 3,
                                  2, 2, 2,  2, 2, 2,
                                  1, 1, 1,  1, 1, 1,
                                  0, 0, 0,  0, 0, 0};

  ATH_MSG_DEBUG("in fillHists()");
  fillEvtInfo();
  if (m_nEvents % 1000 == 0) ATH_MSG_INFO(m_nEvents<<" events processed so far");
  ++m_nEvents;

  m_dqStatus = SG::makeHandle (m_DQstatusKey).get();
  int32_t current_lumiblock = getLumiBlock();

  const TileRawChannelContainer* RawChannelCnt;
  ATH_CHECK(evtStore()->retrieve(RawChannelCnt, m_contName));

  std::vector<double> avgTimePerPart(TileCalibUtils::MAX_ROS, 0.0);
  std::vector<double> sumTimeCh(TileCalibUtils::MAX_ROS, 0.0);
  std::vector<int>  nCh(TileCalibUtils::MAX_ROS, 0);

  for (int k = 0; k < 2; k++) {  //k=0 - computing avg time, k=1 applying avg time

    TileRawChannelContainer::const_iterator collItr = RawChannelCnt->begin();
    TileRawChannelContainer::const_iterator lastColl = RawChannelCnt->end();

    for (; collItr != lastColl; ++collItr) {

      TileRawChannelCollection::const_iterator chItr = (*collItr)->begin();
      TileRawChannelCollection::const_iterator lastCh = (*collItr)->end();

      if (chItr != lastCh) {

        HWIdentifier adc_id = (*chItr)->adc_HWID();
        int ros = m_tileHWID->ros(adc_id);
        int drawer = m_tileHWID->drawer(adc_id);

        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

        for (; chItr != lastCh; ++chItr) {


          const TileRawChannel* rch = (*chItr);
          adc_id = rch->adc_HWID();
          unsigned int chan = m_tileHWID->channel(adc_id);
          
          if (isDisconnected(ros, drawer, chan)) continue;

          int adc = m_tileHWID->adc(adc_id);

          bool good  = m_dqStatus->isAdcDQgood(ros, drawer, chan, adc) && isChanDCSgood(ros, drawer, chan);

          if (good) {
            TileBchStatus status = m_tileBadChanTool->getAdcStatus(drawerIdx, chan, adc);
            good = ! ( status.isBad() ||  status.isBadTiming() );
          }

          if ( !good || (rch->amplitude()) < m_thresholds[adc] ) continue;

          double time = rch->time();
          double timeCorr = 0;

          if (k == 0) { 

            if ((ros == 3 || ros == 4)
                && (chan == 0 || chan == 1 || chan == 2 || chan == 3 || chan == 4 || chan == 5 || chan == 12 || chan == 13 || chan == 18
                  || chan == 19)) {
            } else {
              sumTimeCh[ros] += time;
              nCh[ros] += 1;
            }


          }  else if (k == 1) { //if k==0 
          
            timeCorr = time - avgTimePerPart[ros];      

            m_profile2dHist[ros]->Fill(drawer + 1, chan, timeCorr);

            if(m_doOnline) {

              m_profileHist[ros][drawer][ch2digi[chan]]->Fill(0., timeCorr);

            } else {// End of Online
              m_profileHist[ros][drawer][ch2digi[chan]]->Fill(current_lumiblock, timeCorr);
            }
          }      //k==1 
        } // loop over channels
      }
    }

    if (k == 0) {

      float partitionTime[5] = {0};

      for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
        if (nCh[ros] != 0) {
          avgTimePerPart[ros] = sumTimeCh[ros] / nCh[ros];
        } else {
          avgTimePerPart[ros] = 0;
        }

        partitionTime[ros] = avgTimePerPart[ros] - m_partitionTimeCorrection[ros];
        m_partitionTimeLB[ros]->Fill(current_lumiblock, partitionTime[ros]);

        sumTimeCh[ros] = 0;
        nCh[ros] = 0;
      } //for


      if(m_doOnline) {
        
        if(m_oldLumiblock == -1) {
          m_oldLumiblock = current_lumiblock;
        }
        
        m_deltaLumiblock = current_lumiblock - m_oldLumiblock;
        
        if(m_deltaLumiblock > 0) {//move bins
        
          for (TProfile* timeDifferenceLB : m_partitionTimeDifferenceLB) {
            ShiftTprofile(timeDifferenceLB, m_deltaLumiblock);          
          }

          for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
            for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
              for (unsigned int digi = 0; digi < 8; ++digi) {
                ShiftTprofile(m_profileHist[ros][drawer][digi], m_deltaLumiblock);
              }
            }
          }

          m_oldLumiblock = current_lumiblock;
        }

        int rosesPairIndex(0);
        for (const std::pair<int, int>& roses : m_timeDifferenceBetweenROS) {
          int ros1 = roses.first;
          int ros2 = roses.second;
          
          m_partitionTimeDifferenceLB[rosesPairIndex]->Fill(0., partitionTime[ros1] - partitionTime[ros2]);
          ++rosesPairIndex;
        }
      
      } else {
        int rosesPairIndex(0);
        for (const std::pair<int, int>& roses : m_timeDifferenceBetweenROS) {
          int ros1 = roses.first;
          int ros2 = roses.second;
          
          m_partitionTimeDifferenceLB[rosesPairIndex]->Fill(current_lumiblock, partitionTime[ros1] - partitionTime[ros2]);
          ++rosesPairIndex;
        }
      }

    } //if k==0
  } // loop over k 
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelTimeMonTool::finalHists()
  /*---------------------------------------------------------*/
{
  ATH_MSG_INFO("in finalHists()");

  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
StatusCode TileRawChannelTimeMonTool::checkHists(bool /* fromFinalize */)
  /*---------------------------------------------------------*/
{
  ATH_MSG_INFO("in checkHists()");

  return StatusCode::SUCCESS;
}


bool TileRawChannelTimeMonTool::isChanDCSgood (int ros, int drawer, int channel) const
{
  if (!m_checkDCS) return true;
  TileDCSState::TileDCSStatus status = m_tileDCS->getDCSStatus(ros, drawer, channel);

  if (status > TileDCSState::WARNING) {
    ATH_MSG_DEBUG("Module=" << TileCalibUtils::getDrawerString(ros, drawer)
                  << " channel=" << channel
                  << " masking becasue of bad DCS status=" << status);
    return false;
  }

  return true;
}
