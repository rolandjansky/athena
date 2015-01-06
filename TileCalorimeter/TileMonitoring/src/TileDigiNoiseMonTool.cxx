/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDigiNoiseMonTool
// PACKAGE:  TileMonitoring  
//
//           Produce DigiNoise folder in tilemon.root for monitoring digital noise
//           Input is RAW
//           Output are the noisemap 2D histograms per partition/gain/LFN and HFN
//
// AUTHOR:   Christophe Clement  (largely based on TileDigiMonTool.cxx)
//
// TODO
// -- filter bad channels from the noise maps
//
// ********************************************************************

#include "TileMonitoring/TileDigiNoiseMonTool.h"

#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileBadChanTool.h"


#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>


/*---------------------------------------------------------*/
TileDigiNoiseMonTool::TileDigiNoiseMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TilePaterMonTool(type, name, parent)
  , m_beamInfo("TileBeamInfoProvider")
  , m_tileBadChanTool("TileBadChanTool")
  , m_DQstatus(0)
  , m_nEvents(0)
  , m_nSamples(0)
  , m_nEventsProcessed(0)

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("bookAllDrawers", m_bookAll = false);
  declareProperty("book2D", m_book2D = false);

  // run type 1 - phys, 2 - las, 4 - ped, 8 - cis, 9- mono
  declareProperty("runType", m_runType = 8);
  declareProperty("bigain", m_bigain = true);
  declareProperty("TileRawChannelContainerDSP", m_contNameDSP = "TileRawChannelCnt");
  declareProperty("SummaryUpdateFrequency", m_summaryUpdateFrequency = 0);

  m_path = "/Tile/DigiNoise"; //ROOT File relative directory
}

/*---------------------------------------------------------*/
TileDigiNoiseMonTool::~TileDigiNoiseMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool:: initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in initialize()" );

  CHECK( m_beamInfo.retrieve() );
  CHECK(m_tileBadChanTool.retrieve());

  m_nEvents = 0;
  memset(SumPed1,    0, sizeof(SumPed1));
  memset(SumPed2,    0, sizeof(SumPed2));
  memset(SumRms1,    0, sizeof(SumRms1));
  memset(SumRms2,    0, sizeof(SumRms2));

  return TilePaterMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::bookHists()
/*---------------------------------------------------------*/
{ 

  ATH_MSG_DEBUG( "TileDigiNoiseMonTool::bookHists() :  m_path =  " << m_path  );
  
  
  const char *part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
  const char *gain[6] = { "lo", "hi", "", " low gain", " high gain", "" };
  const char *type[4] = { "lfn", "hfn", "Low Freq. Noise", "High Freq. Noise" };
  char histName[50];
  char histTitle[50];

  int mingain=(m_bigain) ? 0 : 2;
  int maxgain=(m_bigain) ? 2 : 3;

  std::string module_name;
  std::string cell_name;
  std::string channel_name;

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (int gn = mingain; gn < maxgain; ++gn) {
      int adc = gn & 1;
      for (int noisetype = 0; noisetype < 2; ++noisetype) {

        sprintf(histName, "noisemap_%s_%s_%s", part[ros], gain[adc], type[noisetype]);
        sprintf(histTitle, "Noise map %s %s %s", part[ros], gain[3 + gn], type[noisetype + 2]);

        ATH_MSG_DEBUG( "in bookHists() :: booking noise_map histo : " << histName );

        final_noise_map[ros][gn][noisetype] = book2F("", histName, histTitle, 64, 0.5, 64.5, 48, -0.5, 47.5);

        for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; drawer += 2) {
          module_name = TileCalibUtils::getDrawerString(ros, drawer);
          final_noise_map[ros][gn][noisetype]->GetXaxis()->SetBinLabel(drawer + 1, module_name.c_str());
        }

        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
          cell_name = getCellName(ros, channel);

          channel_name = cell_name + (cell_name.empty() ? "ch" : "_ch") + std::to_string(channel);
          final_noise_map[ros][gn][noisetype]->GetYaxis()->SetBinLabel(channel + 1, channel_name.c_str());
        }

        final_noise_map[ros][gn][noisetype]->GetZaxis()->SetTitle("Noise [ADC]");
        final_noise_map[ros][gn][noisetype]->GetZaxis()->SetRangeUser(0, 10);
        final_noise_map[ros][gn][noisetype]->SetTitle(histTitle);

      } // end loop over noise type (hfn, lfn)
    } // end gain loop
  } // end loop over ros/partitions

  //SetBookStatus(true);
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::fillHists() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in fillHists()" );

  ++m_nEvents;

  m_DQstatus = m_beamInfo->getDQstatus();

  const TileDigitsContainer* DigitsCnt;
  CHECK( evtStore()->retrieve(DigitsCnt, "TileDigitsCnt") );

  TileDigitsContainer::const_iterator collItr = DigitsCnt->begin();
  TileDigitsContainer::const_iterator lastColl = DigitsCnt->end();

  unsigned int drawerIdx(TileCalibUtils::MAX_DRAWERIDX);

  for (; collItr != lastColl; ++collItr) {

    TileDigitsCollection::const_iterator digitsItr = (*collItr)->begin();
    TileDigitsCollection::const_iterator lastDigits = (*collItr)->end();

    if (digitsItr != lastDigits) {
      HWIdentifier adc_id = (*digitsItr)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);
      drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    }

    if (digitsItr != lastDigits) {

      HWIdentifier adc_id = (*digitsItr)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);



      double mean_tmp[48][2][16];
      memset(mean_tmp, 0, sizeof(mean_tmp));

      for (; digitsItr != lastDigits; ++digitsItr) {

        adc_id = (*digitsItr)->adc_HWID();
        int chan = m_tileHWID->channel(adc_id);
        int gain = (m_bigain) ? m_tileHWID->adc(adc_id) : 0; // ignore gain in monogain run

        bool good  = m_DQstatus->isAdcDQgood(ros, drawer, chan, gain) && m_beamInfo->isChanDCSgood(ros, drawer, chan);

        if (good) {
          TileBchStatus status = m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain);
          good = ! status.isBad();
        }

        if(!good)continue;


        std::vector<float> vdigits = (*digitsItr)->samples();

        double meansamp = 0.0;
        double rmssamp = 0.0;
        unsigned int dsize = vdigits.size();
        if (dsize > 16) {
          ATH_MSG_ERROR( "length of digits vector " << dsize << " - greater than 16 !" );
          dsize = 16;
        }

        for (unsigned int i = 0; i < dsize; ++i) {
          double dig = vdigits[i];
          meansamp += dig;
          rmssamp += dig * dig;
          mean_tmp[chan][gain][i] = dig;

        }

        if (dsize > 0) {
          double ped = vdigits[0];

          SumPed1[ros][drawer][chan][gain] += ped;
          SumPed2[ros][drawer][chan][gain] += ped * ped;

          if (dsize > 1) {
            meansamp /= dsize;
            rmssamp = rmssamp / dsize - meansamp * meansamp;
            rmssamp = (rmssamp > 0.0) ? sqrt(rmssamp * dsize / (dsize - 1)) : 0.0;

            SumRms1[ros][drawer][chan][gain] += rmssamp;
            SumRms2[ros][drawer][chan][gain] += rmssamp * rmssamp;
          }
        }
      } // digits

    } //check if Digits in module

  }

  ++m_nEventsProcessed;

  if ((m_summaryUpdateFrequency > 0) && (m_nEvents % m_summaryUpdateFrequency == 0)) return finalHists();

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::finalHists() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in finalHists()" );


  // for bigain run book 2 histograms per channel
  // for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

      for (int gn = mingain; gn < maxgain; ++gn) {
        int adc = gn & 1;

        for (int ch = 0; ch < 48; ++ch) {
          double Ped = 0.0, PedRMS = 0.0, RMSHi = 0.0;

          int nevents = m_nEvents;
          if (nevents > 0) {
            Ped = SumPed1[ros][drawer][ch][adc] / nevents;
            RMSHi = SumRms1[ros][drawer][ch][adc] / nevents;

            if (nevents > 1) {
              PedRMS = SumPed2[ros][drawer][ch][adc] / nevents - Ped * Ped;
              PedRMS = (PedRMS > 0.0) ? sqrt(PedRMS * nevents / (nevents - 1)) : 0.0;
            }
          }

          // fill the 2D noise map  CC 2014-07-14
          // LFN : type=1, HFN : type=2

          final_noise_map[ros][adc][0]->SetBinContent(drawer + 1, ch + 1, PedRMS); // this is the LFN (RMS of DIGI0)
          final_noise_map[ros][adc][0]->SetEntries(m_nEventsProcessed);

          final_noise_map[ros][adc][1]->SetBinContent(drawer + 1, ch + 1, RMSHi); // this is the HFN (MeanOf(RMS over the 7 digis) )
          final_noise_map[ros][adc][1]->SetEntries(m_nEventsProcessed);

        } // channnel loop for (int ch = 0; ch < 48; ++ch) {
      } // gain loop                             
    } //loop over drawer
  } //loop over ros

  return StatusCode::SUCCESS;
} // finalHists()



/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::checkHists(bool /* fromFinalize */)
/*---------------------------------------------------------*/
{

  ATH_MSG_INFO( "in checkHists()" );

  return StatusCode::SUCCESS;
}

