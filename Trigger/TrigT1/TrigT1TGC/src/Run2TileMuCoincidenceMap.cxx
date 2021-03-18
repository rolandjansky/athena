/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/Run2TileMuCoincidenceMap.h"

#include <fstream>

namespace LVL1TGC {

Run2TileMuCoincidenceMap::Run2TileMuCoincidenceMap(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey)
 : m_readCondKey(readCondKey)
{
  // for debug  
  //dumpMap();
}

void Run2TileMuCoincidenceMap::dumpMap() const
{
  // select right database according to a set of thresholds
  std::string fullName="TileMuCoincidenceMap.v01._12.out";

  std::ofstream file(fullName.c_str());    

  for (size_t side=0; side< TGCTriggerData::N_SIDE; side++){
    for (size_t sec=0; sec< TGCTriggerData::N_ENDCAP_SECTOR; sec++){
      for (size_t ssc=0; ssc< TGCTriggerData::N_ENDCAP_SSC; ssc++){
        file << "# " << side << " " << sec << " " << ssc << " ";

        for(int i=0; i<TGCTriggerData::N_PT_THRESH; i++) {
          file << int(this->getFlagPT(i+1, ssc, sec, side)) << " ";
        }

        for(int i=0; i<TGCTriggerData::N_ROI_IN_SSC; i++) {
          file << int(this->getFlagROI(i, ssc, sec, side)) << " ";
        }
        file << std::endl;

        for(int i=0; i<TGCTriggerData::N_TILE_INPUT; i++) {
          file << int(this->getTrigMask(i, ssc, sec, side)) << " ";
        }
        file << std::endl;
      }
    }
  }
  file.close();	  
}

uint8_t Run2TileMuCoincidenceMap::getTrigMask(const int module,
                                              const int16_t ssc,
                                              const int16_t sec,
                                              const int16_t side) const
{
  if ((module<0)||(module>=TGCTriggerData::N_TILE_INPUT)) return TM_NA;

  SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
  const TGCTriggerData* readCdo{*readHandle};
  uint16_t mask = readCdo->getTrigMaskTile(ssc, sec, side);
  return mask>>(module*4) & 0x7;
}

uint8_t Run2TileMuCoincidenceMap::getFlagPT(const int pt,
                                            const int16_t ssc,
                                            const int16_t sec,
                                            const int16_t side) const
{
  if ((pt<=0)||(pt>TGCTriggerData::N_PT_THRESH)) return -1;

  SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
  const TGCTriggerData* readCdo{*readHandle};
  uint8_t ptmask = readCdo->getFlagPtTile(ssc, sec, side);
  return (ptmask>>(pt-1)) & 0x1;  /* only 1st bit needed (0x1) */
}

uint8_t Run2TileMuCoincidenceMap::getFlagROI(const int roi,
                                             const int16_t ssc,
                                             const int16_t sec,
                                             const int16_t side) const
{
  if ((roi<0)||(roi>=TGCTriggerData::N_ROI_IN_SSC)) return -1;

  SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
  const TGCTriggerData* readCdo{*readHandle};
  uint8_t roimask = readCdo->getFlagRoiTile(ssc, sec, side);
  return (roimask >> roi) & 0x1;  /* only 1st bit needed (0x1) */
}

uint16_t Run2TileMuCoincidenceMap::getAddr(int16_t side, int16_t sec, int16_t ssc) const
{
  return ((side & TGCTriggerData::SIDE_MASK)<<TGCTriggerData::ADDR_SIDE_SHIFT) +
         ((sec & TGCTriggerData::SECTOR_MASK)<<TGCTriggerData::ADDR_SECTOR_SHIFT) +
         (ssc & TGCTriggerData::SSC_MASK);
}


} //end of namespace bracket
