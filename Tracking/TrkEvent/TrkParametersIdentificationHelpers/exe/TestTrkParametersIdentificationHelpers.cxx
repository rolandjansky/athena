/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParametersIdentificationHelpers/TrackParametersIdentificationHelper.h"

#include <iostream>
#include <iomanip>
#include <bitset>

void testBitField(){
  Trk::BitField<unsigned int> field(31,1);
  TrackParametersIdentifier id=0;
  unsigned int max = -1;
  std::cout << " starting loop " << max << std::endl;
  for( unsigned int i=0;i<max;++i ){
    if( !field.encode(i,id) ){
      std::cout << " encoding failed for " << i << std::endl; 
      break;
    }
    unsigned int value = field.decode(id);
    std::cout << "  i " << std::setw(3) << i << " value " << std::setw(3) << value << "  " << std::bitset<32>(i) << " id " << std::bitset<32>(id) << std::endl;
  }
} 

void testCalo() {
  
  Trk::TrackParametersIdHelper helper;
  unsigned int good = 0;
  unsigned int bad  = 0;
  unsigned int goodCalo = 0;
  unsigned int badCalo  = 0;
  
  // loop over technologies
  for( unsigned int tech = AtlasDetDescr::fFirstAtlasDetTechnology; tech < AtlasDetDescr::fNumAtlasDetTechnologies; ++tech ){
    
    bool isCalo = (tech >= AtlasDetDescr::fFirstAtlasCaloTechnology) && (tech <= AtlasDetDescr::fLastAtlasCaloTechnology);
    
    // loop over samplings
    for( unsigned int sample = CaloSampling::PreSamplerB; sample < CaloSampling::Unknown; ++sample ){
      
      for( unsigned int isEntry = 0; isEntry < 2 ; ++isEntry ){
        bool entry = isEntry == 1 ? true : false;
        TrackParametersIdentifier id = helper.encode( static_cast<AtlasDetDescr::AtlasDetTechnology>(tech), 
                                                      static_cast<CaloSampling::CaloSample>(sample), entry );
        AtlasDetDescr::AtlasDetTechnology itech = helper.technology(id);
        CaloSampling::CaloSample isample = helper.caloSample(id);
        bool ientry = helper.isEntryToVolume(id);
        bool ivalid = helper.isValid(id);
        bool printDetails = false;
        if( !ivalid || sample != isample || entry != ientry ) {
          ++bad;
          if( isCalo ) {
            std::cout << "WARNING bad decoding/encoding " << std::endl;
            ++badCalo;
            printDetails = true;
          }
        }else{ 
          ++good;
          if( isCalo ) ++goodCalo;
          else {
            std::cout << " good but no calo!!! " << std::endl;
            printDetails = true;
          }
        }
        if( printDetails ){
          std::cout << " tech  " << std::setw(4) << tech  << " sample  " << std::setw(4) << sample  << " isEntry " << entry << std::endl
                    << " itech " << std::setw(4) << itech << " isample " << std::setw(4) << isample << " isEntry " << ientry
                    << " valid " << ivalid << " id " << std::bitset<32>(id) << std::endl;
        }
      }
    }
  }
  std::cout << "all:  good " << good << " bad " << bad << std::endl;
  std::cout << "calo: good " << goodCalo << " bad " << badCalo << std::endl;
    
}


int main() {
  
  testBitField();
  testCalo();
}
