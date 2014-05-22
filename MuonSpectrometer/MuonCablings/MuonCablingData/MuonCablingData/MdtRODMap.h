/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MDTRODMAP_H
#define MUONMDT_CABLING_MDTRODMAP_H

#include "MuonCablingData/MdtMapBase.h"

class MdtCsmMap;

class MdtRODMap : public MdtMapBase<MdtCsmMap> {

 public:

  MdtRODMap(uint8_t rodId);
  ~MdtRODMap();

  /** set functions */
  bool setCsmMap( uint8_t csmId, MdtCsmMap* mdtCsmMap );

  /** get functions */
  MdtCsmMap* getCsmMap(uint8_t csmId);

 private:

};


#endif



