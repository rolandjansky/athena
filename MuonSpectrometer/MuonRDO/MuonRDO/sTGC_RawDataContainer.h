/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_sTGC_RAWDATACONAINTER_H
#define MUONRDO_sTGC_RAWDATACONAINTER_H

#include "MuonRDO/sTGC_RawDataCollection.h"
#include "SGTools/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

/*
*/
class sTGC_RawDataContainer
  : public IdentifiableContainer<sTGC_RawDataCollection>
{
public:  
  sTGC_RawDataContainer();
  sTGC_RawDataContainer(unsigned int hashmax);
  ~sTGC_RawDataContainer(); 

  
  // class ID
  static const CLID& classID(); 

  /** return class ID */
  virtual const CLID& clID() const {return classID();}
};

CLASS_DEF( sTGC_RawDataContainer , 1303509372 , 1 )

#endif


