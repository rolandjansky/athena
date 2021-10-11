/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTSUBDETECTORMAP_H
#define MUONMDT_CABLING_MDTSUBDETECTORMAP_H

#include "MuonCablingData/MdtMapBase.h"

class MdtRODMap;

class MdtSubdetectorMap : public MdtMapBase<MdtRODMap> {

 public:

  MdtSubdetectorMap(uint8_t subdetectorId);
  ~MdtSubdetectorMap();

  /** set functions */
  bool setRODMap(uint8_t rodId, MdtRODMap* mdtRODMap, MsgStream &log);

  /** get function */
  MdtRODMap* getRODMap(uint8_t rodId);
  
 private:

};


#endif    // MUONMDT_CABLING_MDTSUBDETECTORMAP
