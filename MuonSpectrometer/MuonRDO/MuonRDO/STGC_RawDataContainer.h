/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_STGC_RawDataCONAINTER_H
#define MUONRDO_STGC_RawDataCONAINTER_H

#include "MuonRDO/STGC_RawDataCollection.h"
#include "SGTools/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

namespace Muon {
/*
*/
class STGC_RawDataContainer
  : public IdentifiableContainer<STGC_RawDataCollection>
{
public:  
  STGC_RawDataContainer(unsigned int hashmax);
  ~STGC_RawDataContainer(); 
  
  // class ID
  static const CLID& classID(); 

  /** return class ID */
  virtual const CLID& clID() const {return classID();}
};
}

CLASS_DEF( Muon::STGC_RawDataContainer , 1194130261 , 1 )
// CONTAINER_IS_IDENTCONT(Muon::STGC_RawDataContainer)

#endif


