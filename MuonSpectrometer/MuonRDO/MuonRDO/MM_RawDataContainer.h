/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_MM_RAWDATACONAINTER_H
#define MUONRDO_MM_RAWDATACONAINTER_H

#include "MuonRDO/MM_RawDataCollection.h"
#include "SGTools/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

/**
*/
class MM_RawDataContainer
  : public IdentifiableContainer<MM_RawDataCollection>
{
public:  
  MM_RawDataContainer(unsigned int hashmax);
  virtual ~MM_RawDataContainer(); 

  /// class ID
  static const CLID& classID(); 

  /** return class ID */
  virtual const CLID& clID() const {return classID();}

};

CLASS_DEF( MM_RawDataContainer , 1314812033 , 1 )

#endif



