/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_MM_RAWDATACONAINTER_H
#define MUONRDO_MM_RAWDATACONAINTER_H

#include "MuonRDO/MM_RawDataCollection.h"
#include "MuonRDO/MM_RawDataCollection_Cache.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

namespace Muon {
/**
*/
class MM_RawDataContainer
  : public IdentifiableContainer<MM_RawDataCollection>
{
public:  
  MM_RawDataContainer(unsigned int hashmax);
  MM_RawDataContainer(); 
  MM_RawDataContainer(MM_RawDataCollection_Cache* cache); 
  virtual ~MM_RawDataContainer(); 

  /// class ID
  static const CLID& classID(); 

  /** return class ID */
  virtual const CLID& clID() const {return classID();}

};
}

CLASS_DEF( Muon::MM_RawDataContainer , 1155093320 , 1 )

#endif



