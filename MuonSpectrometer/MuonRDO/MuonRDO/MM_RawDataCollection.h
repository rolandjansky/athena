/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_MM_RAWDATACOLLECTION_H
#define MUONRDO_MM_RAWDATACOLLECTION_H

#include "MuonRDO/MM_RawData.h"
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

class MM_RawDataCollection : public DataVector<MM_RawData>
{
public:
  MM_RawDataCollection(IdentifierHash hash) : m_idHash(hash) {}

  const IdentifierHash& identifierHash() const { return m_idHash; }
  
private:
  
  /** Offline IdentifierHash for this collection*/
  IdentifierHash m_idHash;
  
};

#endif