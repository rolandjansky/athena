/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_RPCSECTORLOGICCONTAINER_H
#define MUONRDO_RPCSECTORLOGICCONTAINER_H

#include<stdint.h>
#include<vector>

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"

#include "MuonRDO/RpcSectorLogic.h"

//
// S. Rosati Jun 2006
//

class RpcSectorLogicContainer : public DataVector<RpcSectorLogic> {

 public:

  /** Default constructor */
  RpcSectorLogicContainer();

  /** Destructor */
  ~RpcSectorLogicContainer() { };

  /** Check if the sector has already been decoded */ 
  bool findSector(const uint16_t sectorId, const uint16_t side=0) const;

  /** Flag the sector as already decoded */
  bool setSector(uint16_t sectorId, const uint16_t side=0);

 private:

  // Dataword to contain the list of already filled sectors
  uint32_t m_sectorList[2];

};

CLASS_DEF( RpcSectorLogicContainer , 1298668563 , 1 )

#endif




