/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_RPCSECTORLOGICCONTAINER_H
#define MUONRDO_RPCSECTORLOGICCONTAINER_H

#include<stdint.h>
#include<vector>

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "MuonRDO/RpcSectorLogic.h"

//
// S. Rosati Jun 2006
//

class RpcSectorLogicContainer : public DataVector<RpcSectorLogic> {

 public:
  typedef uint32_t SectorListElement_t;
  typedef SectorListElement_t SectorList_t[2];

  /** Default constructor */
  RpcSectorLogicContainer();

  /** Destructor */
  ~RpcSectorLogicContainer() { };

  /** Check if the sector has already been decoded */ 
  bool findSector(const uint16_t sectorId, const uint16_t side=0) const;

  /** Flag the sector as already decoded */
  bool setSector(uint16_t sectorId, const uint16_t side=0);

  const SectorListElement_t* sectorList() const;
  void setSectorList (const SectorList_t& sectorList);

 private:

  // Dataword to contain the list of already filled sectors
  SectorList_t m_sectorList;

};

CLASS_DEF( RpcSectorLogicContainer , 1298668563 , 1 )

#endif




