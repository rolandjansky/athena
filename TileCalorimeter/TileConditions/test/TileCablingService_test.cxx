/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"

#include "StoreGate/setupStoreGate.h"

#include "TileCablingService_common_test.cxx"


using Tile = TileCalibUtils;

namespace {

  enum {LBA = 1, LBC = 2, EBA = 3, EBC = 4};

  // Tells if a channel is disconnected or not
  // Special modules are considered too.
  bool isDisconnected(int ros, int drawer, int channel) {

    static const int chMapLB[Tile::MAX_CHAN] = {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                                 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                                 , 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0
                                                 , 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };

    static const int chMapEB[Tile::MAX_CHAN] = {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                                 , 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0
                                                 , 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0
                                                 , 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 };

    static const int chMapEBsp[Tile::MAX_CHAN] = {   1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
                                                   , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                                                   , 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0
                                                   , 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 };


    if (ros < EBA) { // LB, all standard. Channels 30,31,43 are disconnected
      return chMapLB[channel];
    } else {
      // EB, EBA15 and EBC18 are special
      if (((ros == EBA) && (drawer == 14)) || ((ros == EBC) && (drawer == 17))) {
        return chMapEBsp[channel];
      } else  {//EB standard module
        return chMapEB[channel];
      }
    } //end if LB else EB

  }
}

void test1() {

  std::cout << "test1\n";

  const TileCablingService* cabling = TileCablingService::getInstance();

  const TileHWID* tileHWID = cabling->getTileHWID();
  const TileID* tileID = cabling->getTileID();
  const TileTBID* tileTBID = cabling->getTileTBID();


  std::cout << "SW MAX CELLS: " << tileID->cell_hash_max() << std::endl;
  std::cout << "SW MAX PMT: " << tileID->pmt_hash_max() << std::endl;

  IdContext adcContext = tileID->adc_context();
  unsigned int maxAdcHash = tileID->adc_hash_max();


  // TEST: SWID -> HWID -> SWID

  std::cout << "SW MAX ADC: " << maxAdcHash << std::endl;


  for (IdentifierHash adcHash = 0; adcHash < maxAdcHash; adcHash += 1) {
    Identifier swid;
    if (tileID->get_id(adcHash, swid, &adcContext) == 0) {

      HWIdentifier hwid = cabling->s2h_adc_id(swid);
      assert(swid != hwid);

      if (hwid.is_valid()) {

        if (tileID->section(swid) == TileID::GAPDET) {

          int ros = tileHWID->ros(hwid);
          int drawer = tileHWID->drawer(hwid);
          
          // Skip merged E1 cells
          if (tileID->sample(swid) == TileID::SAMP_E
              && tileID->tower(swid) == 10 // E1 tower
              && cabling->E1_merged_with_run2plus(ros, drawer)
              && cabling->E1_merged_with_run2plus(ros, drawer) == tileID->module(swid)) continue;

          // Skip not connected C10 and D4
          if (!cabling->TileGap_connected(swid)) continue;

        }

        Identifier swid2 = cabling->h2s_adc_id(hwid);
        if (swid != swid2) std::cout << to_string(swid, tileID) 
                                     << " [" << to_string(hwid, tileHWID) << "] != " 
                                     << to_string(swid2, tileID) << std::endl;
        assert(swid == swid2);
      } else {
        std::cout << "ERROR: can not convert SWID [" << to_string(swid, tileID) << "] to HWID!!!" << std::endl;
      }
      
    } else {
      std::cout << "WARNING: can not convert hash [" << adcHash << "] to ID!!!" << std::endl;
    }
  }



  // TEST: HWID -> SWID -> HWID

  adcContext = tileHWID->adc_context();
  maxAdcHash = tileHWID->adc_hash_max();

  std::cout << "HW MAX CHANNELS: " << tileHWID->channel_hash_max() << std::endl;
  std::cout << "HW MAX ADC: " << maxAdcHash << std::endl;

  for (IdentifierHash adcHash = 0; adcHash < maxAdcHash; adcHash += 1) {
    HWIdentifier hwid;
    if (tileHWID->get_id(adcHash, hwid, &adcContext) == 0) {

      if (tileHWID->ros(hwid) == 0) continue;

      Identifier swid = cabling->h2s_adc_id(hwid);
      assert(swid != hwid);
      if (swid.is_valid() && !tileTBID->is_tiletb(swid)) {
        HWIdentifier hwid2 = cabling->s2h_adc_id(swid);
        if (hwid != hwid2) std::cout << to_string(hwid, tileHWID) 
                                     << " [" << to_string(swid, tileID) << "] != " 
                                     << to_string(hwid2, tileHWID) << std::endl;
        //          assert(hwid == hwid2);
      } else {
        // std::cout << "ERROR: can not convert HWID [" << to_string(hwid, tileHWID) << "] to SWID!!!" << std::endl;
      }
      
    } else {
      std::cout << "WARNING: can not convert hash [" << adcHash << "] to HWID!!!" << std::endl;
    }
  }

  int maxChannels = cabling->getMaxChannels();
  int maxGains = cabling->getMaxGains();

  std::cout << "MAX CHANNELS (in a drawer): " << maxChannels << std::endl;
  std::cout << "MAX GAINS: " << maxGains << std::endl;

  for (int ros = 1; ros < 5; ++ros) {
    for (int drawer = 0; drawer < 64; ++drawer) {
      for (int channel = 0; channel < maxChannels; ++channel) {
        for (int adc = 0; adc < maxGains; ++adc) {

          HWIdentifier hwid = tileHWID->adc_id(ros, drawer, channel, adc);  
          Identifier swid = cabling->h2s_adc_id(hwid);
          assert(hwid != swid);
          if (swid.is_valid() && !tileTBID->is_tiletb(swid)) {
            HWIdentifier hwid2 = cabling->s2h_adc_id(swid);
            if (hwid != hwid2) std::cout << to_string(hwid, tileHWID) 
                                         << " [" << to_string(swid, tileID) << "] != " 
                                         << to_string(hwid2, tileHWID) << std::endl;
            assert(hwid == hwid2);
          }
        }
      }
    }
  }



  // TEST: disconnected channels

  for (unsigned int ros = 1; ros < Tile::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < Tile::MAX_DRAWER; ++drawer) {
      for (unsigned int channel = 0; channel < Tile::MAX_CHAN; ++channel) {
        assert(cabling->isDisconnected(ros, drawer, channel) == isDisconnected(ros, drawer, channel));
      }
    }
  }

}


int main ATLAS_NOT_THREAD_SAFE (int /*argc*/, char** argv) {

  Athena_test::setupStoreGate (argv[0]);

  std::cout << "TileCablingService_test\n";

  IdDictParser parser;
  TileCablingSvcMock::init_idhelpers (parser);

  test1();

  return 0;
}


// This package builds a component library, so we don't normally
// link against the library.  So just include this here.
// #include "../src/TileDigitsThresholdFilter.cxx"
