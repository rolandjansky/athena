/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEventInfo/EventInfo.h"

#include "CaloEvent/CaloCellContainer.h"

#include "TileMonitoring/TileTBPulseMonTool.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileEvent/TileCell.h"
#include "TileConditions/TileBadChanTool.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>

/*---------------------------------------------------------*/
TileTBPulseMonTool::TileTBPulseMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_tileBadChanTool("TileBadChanTool")
  , m_pulseHist2{ {{{}}} }
  , m_pulseProfile{ {{{}}} }
  , m_isFirstEvent(true)

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("TileDigitsContainer", m_digitsContainerName = "TileDigitsCnt");
  declareProperty("TileRawChannelContainer", m_rawChannelContainerName = "TileRawChannelOpt2");

  declareProperty("UseDemoCabling", m_useDemoCabling = 0); // if set to 2015 - assume TB 2015 cabling

  m_path = "/Tile/TestBeam/PulseShape"; //ROOT File relative directory
}

/*---------------------------------------------------------*/
TileTBPulseMonTool::~TileTBPulseMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileTBPulseMonTool:: initialize() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in initialize()" );


  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileTBPulseMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "::bookHistograms() :  m_path =  " << m_path  );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTBPulseMonTool::bookHistogramsPerModule(int ros, int drawer) {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "::bookHistograms() :  m_path =  " << m_path  );

  const std::string part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
  const std::string gain[2] = { "lo", "hi"};
  const std::string pmtNames[2] = {"PMT_Up", "PMT_Down"};


  std::string histName;
  std::string profName;
  std::string histTitle;

  std::string runNumStr = getRunNumStr();
  std::string moduleName;
  std::string cellName;
  std::string subDir;

  for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
    for (unsigned int adc = 0; adc < TileCalibUtils::MAX_GAIN; ++adc) {


      int index, pmt;
      int channel1 = channel;
      if (m_useDemoCabling == 2015 && ros == 4 && drawer == 1) {
        int pmt2channel[48] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
                               26,25,24,29,31,32,27,28,30,35,34,33,38,37,43,44,41,40,39,36,42,47,46,45};
        channel1 = pmt2channel[channel];
      } else if ( (m_useDemoCabling >= 2016 && m_useDemoCabling <= 2019)
      && (ros == 2 && (drawer == 1 || drawer>2)) ) {
        int pmt2channel[48] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
                               26,25,24,29,28,27,32,31,30,35,34,33,38,37,36,41,40,39,44,43,42,47,46,45};
        channel1 = pmt2channel[channel];
      } else if ( (m_useDemoCabling >= 2018)
      && (ros == 4 && drawer>=2) ) {
        int pmt2channelEB[48] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
                                 31,32,30,35, 33,34, 38,37,41,40,39,36, 26,25,24,29,28,27, 44,43,42,47,46,45};
        channel1 = pmt2channelEB[channel];
      }

      /*Identifier cell_id = */ (TileCablingService::getInstance())->h2s_cell_id_index (ros, drawer, channel1, index, pmt);


      moduleName = TileCalibUtils::getDrawerString(ros, drawer);
      cellName = getCellName(ros, channel1);

      if (ros < 3 && !cellName.empty() && cellName[0] == 'B' && cellName != "B9") {
  cellName.insert(1, 1, 'C');
      }

      std::string pmtName = (pmt < 0) ? ("Channel_" + std::to_string(channel)) : pmtNames[pmt];

      std::string name = cellName + "_" + moduleName  + "_" + pmtName + "_" + gain[adc];
      histName = "pulseShape_" + name;
      profName = "pulseShapeProfile_" + name;
      histTitle = "Run " + runNumStr + " " + moduleName + " Channel " + std::to_string(channel) + " Gain " + gain[adc];

      subDir = part[ros] + "/" + moduleName;


      m_pulseHist2[ros][drawer][channel][adc] = book2F(subDir, histName, histTitle, 100, -100, 100, 100, -0.2, 1.5);
      m_pulseProfile[ros][drawer][channel][adc] = bookProfile(subDir, profName, histTitle, 100, -100, 100);

      m_pulseHist2[ros][drawer][channel][adc]->GetXaxis()->SetTitle("time [ns]");
      m_pulseHist2[ros][drawer][channel][adc]->GetYaxis()->SetTitle("Normalized Units");
      m_pulseProfile[ros][drawer][channel][adc]->GetXaxis()->SetTitle("time [ns]");
      m_pulseProfile[ros][drawer][channel][adc]->GetYaxis()->SetTitle("Normalized Units");

    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTBPulseMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in fillHistograms()" );


  if (m_isFirstEvent) {
    // fill event info like L1 trigger type, run number, etc...
    fillEvtInfo();
    initFirstEvent();
  }

  float amplitude[5][64][48][2];
  float time[5][64][48][2];
  float ped[5][64][48][2];

  const TileDigitsContainer* digitsContainer;
  ATH_CHECK( evtStore()->retrieve(digitsContainer, m_digitsContainerName) );
  const TileRawChannelContainer* rawChannelContainer;
  ATH_CHECK(evtStore()->retrieve(rawChannelContainer, m_rawChannelContainerName));

  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

      if (rawChannelCollection->empty()) continue;

      HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);

      for (const TileRawChannel* rch : *rawChannelCollection) {

        adc_id = rch->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        int adc = m_tileHWID->adc(adc_id);

        amplitude[ros][drawer][channel][adc] = rch->amplitude();
        time[ros][drawer][channel][adc] = rch->time();

        float peds = rch->pedestal();
        peds  = peds - int( (peds +500 ) * 1e-4) * 10000;
        ped[ros][drawer][channel][adc] = peds;


      }

  }

  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {

      if (digitsCollection->empty()) continue;


      HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);
      if ( !m_pulseHist2[ros][drawer][0][0] ) {
        ATH_CHECK( bookHistogramsPerModule(ros,drawer) );

      }



      for (const TileDigits* tile_digits : *digitsCollection) {

        adc_id = tile_digits->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        int adc = m_tileHWID->adc(adc_id);


        std::vector<float> digits = tile_digits->samples();
        ATH_MSG_DEBUG("Ros: " << ros
                              << " drawer: " << drawer
                              << " channel: " << channel
                              << " gain: " << adc
                              << " amp: " << amplitude[ros][drawer][channel][adc]
                              << " time: " << time[ros][drawer][channel][adc] );


        int sampleI = 0;
        if ( time[ros][drawer][channel][adc] != 0 && amplitude[ros][drawer][channel][adc] > 0.) {
          for (double digit : digits) {

            float sampleT = (sampleI - 3)* 25. - time[ros][drawer][channel][adc];
            float sampleE = (digit - ped[ros][drawer][channel][adc] ) / amplitude[ros][drawer][channel][adc];
            m_pulseHist2[ros][drawer][channel][adc]->Fill( sampleT, sampleE);
            m_pulseProfile[ros][drawer][channel][adc]->Fill( sampleT, sampleE);
            sampleI++;
          }
        }



     }

  }//end loop digitsContainer








  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
StatusCode TileTBPulseMonTool::procHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in procHistograms()" );

  m_isFirstEvent = true;

  return StatusCode::SUCCESS;
}





// Operations to be done only once at the first event
/*---------------------------------------------------------*/
void TileTBPulseMonTool::initFirstEvent() {


}
