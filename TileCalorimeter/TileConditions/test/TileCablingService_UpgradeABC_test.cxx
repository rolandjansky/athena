/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"

#include "StoreGate/setupStoreGate.h"

#include "TileCablingService_common_test.cxx"


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

  std::cout << "SW MAX ADC: " << maxAdcHash << std::endl;

  for (IdentifierHash adcHash = 0; adcHash < maxAdcHash; adcHash += 1) {
    Identifier swid;
    if (tileID->get_id(adcHash, swid, &adcContext) == 0) {

      int tower = tileID->tower(swid);
      if (tower == 41 || (tower > 42 && tower < 46)) continue; // Skip dummy towers

      HWIdentifier hwid = cabling->s2h_adc_id(swid);
      assert(swid != hwid);
      if (hwid.is_valid()) {
        
        if (tileID->section(swid) == TileID::GAPDET) {
          
          int ros = tileHWID->ros(hwid);
          int drawer = tileHWID->drawer(hwid);
          
          // Skip merged E1 cells
          if (tileID->sample(swid) == TileID::SAMP_E
              && tileID->tower(swid) == 42 // E1 tower
              && cabling->E1_merged_with_run2plus(ros, drawer)
              && cabling->E1_merged_with_run2plus(ros, drawer) == tileID->module(swid)) {

            Identifier swid2 = cabling->h2s_adc_id(hwid);
            assert(tileID->system(swid) == tileID->system(swid2));
            assert(tileID->section(swid) == tileID->section(swid2));
            assert(tileID->side(swid) == tileID->side(swid2));
            assert(tileID->tower(swid) == tileID->tower(swid2));
            assert(tileID->sample(swid) == tileID->sample(swid2));
            assert(tileID->pmt(swid) == tileID->pmt(swid2));
            assert(tileID->adc(swid) == tileID->adc(swid2));

            continue;
          }
          
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


  /*
  for (unsigned int tower = 0; tower < 64; ++tower) {
    for (int sample = 0; sample < 5; ++sample) {
      Identifier swid = tileID->pmt_id(section, side, module, tower, sample, pmt);
      HWIdentifier hwid = cabling->s2h_adc_id(swid);
      std::cout << to_string(swid, tileID) << " => " << to_string(hwid, tileHWID) << std::endl;
    }
  }
  */


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
      if (swid.is_valid()) {

        if ( tileTBID->is_tiletb(swid)) {
          HWIdentifier hwid2 = cabling->s2h_adc_id(swid);
          assert(tileHWID->ros(hwid) == tileHWID->ros(hwid2));
          assert(tileHWID->drawer(hwid) == tileHWID->drawer(hwid2));
          assert(tileHWID->channel(hwid) == tileHWID->channel(hwid2));
          assert(TileHWID::HIGHGAIN == tileHWID->adc(hwid2));
          continue;
        }

        HWIdentifier hwid2 = cabling->s2h_adc_id(swid);
        if (hwid != hwid2) std::cout << to_string(hwid, tileHWID) 
                                     << " [" << to_string(swid, tileID) << "] != " 
                                     << to_string(hwid2, tileHWID) << std::endl;
        assert(hwid == hwid2);
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
          assert(tileHWID->ros(hwid) == ros);
          assert(tileHWID->drawer(hwid) == drawer);
          assert(tileHWID->channel(hwid) == channel);
          assert(tileHWID->adc(hwid) == adc);

          int frag = tileHWID->frag(ros, drawer);

          Identifier swid = cabling->h2s_adc_id(hwid);
          assert(hwid != swid);
          if (swid.is_valid()) {

            if ( tileTBID->is_tiletb(swid)) {
              HWIdentifier hwid2 = cabling->s2h_adc_id(swid);
              assert(tileHWID->ros(hwid) == tileHWID->ros(hwid2));
              assert(tileHWID->drawer(hwid) == tileHWID->drawer(hwid2));
              assert(tileHWID->channel(hwid) == tileHWID->channel(hwid2));
              assert(TileHWID::HIGHGAIN == tileHWID->adc(hwid2));
              continue;
            }
            
            HWIdentifier hwid2 = cabling->s2h_adc_id(swid);
            if (hwid != hwid2) std::cout << to_string(hwid, tileHWID) 
                                         << " [" << to_string(swid, tileID) << "] != " 
                                         << to_string(hwid2, tileHWID) << std::endl;
            assert(hwid == hwid2);

            assert(cabling->frag(swid) == frag);
          }
        }
      }
    }
  }

}


int main ATLAS_NOT_THREAD_SAFE (int /*argc*/, char** argv) {

  Athena_test::setupStoreGate (argv[0]);

  std::cout << "TileCablingService_UpgradeABC_test\n";

  IdDictParser parser;
  TileCablingSvcMock::init_idhelpers (parser,
                                      "IdDictTileCalorimeter-upgradeABC.xml",
                                      TileCablingService::UpgradeABC);
  test1();

  return 0;
}


// This package builds a component library, so we don't normally
// link against the library.  So just include this here.
// #include "../src/TileDigitsThresholdFilter.cxx"
