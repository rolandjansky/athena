/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_STGC_RAWDATACOLLECTION_H
#define MUONRDO_STGC_RAWDATACOLLECTION_H

#include "MuonRDO/sTGC_RawData.h"
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

class sTGC_RawDataCollection : public DataVector<sTGC_RawData>
{
public:
  sTGC_RawDataCollection(IdentifierHash hash) : m_idHash(hash) {}

  const IdentifierHash& identifierHash() const { return m_idHash; }
private:
  
  /** Offline IdentifierHash for this collection*/
  IdentifierHash m_idHash;
  
};

#endif