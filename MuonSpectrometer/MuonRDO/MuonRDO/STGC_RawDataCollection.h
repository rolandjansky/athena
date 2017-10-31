/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_STGC_RawDataCOLLECTION_H
#define MUONRDO_STGC_RawDataCOLLECTION_H

#include "MuonRDO/STGC_RawData.h"
#include "DataModel/DataVector.h"
#include "Identifier/IdentifierHash.h"

namespace Muon {
  class STGC_RawDataCollection : public DataVector<STGC_RawData>
  {
  public:
    STGC_RawDataCollection(IdentifierHash hash) : m_idHash(hash) {}

    const IdentifierHash& identifyHash() const { return m_idHash; }
  private:
  
    /** Offline IdentifierHash for this collection*/
    IdentifierHash m_idHash;
  };
}

#endif