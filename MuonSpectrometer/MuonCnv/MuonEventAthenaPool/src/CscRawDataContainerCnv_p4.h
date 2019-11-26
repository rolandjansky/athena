/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// NOTE: This is an exact copy of CscRawDataContainerCnv_p3, since from p3 to p4 only the meaning of the m_hashId changed:
// Until p3, the m_hashId stores the identifier hash which encodes the geometrical position of the CSC
// From p4 onwards, the m_hashId stores the position of the CSC identifier in the vector of identifiers

#ifndef MUON_CSCCRAWDATACONTAINERCNV_P4_H
#define MUON_CSCCRAWDATACONTAINERCNV_P4_H


#include "MuonRDO/CscRawDataContainer.h"
#include "MuonEventAthenaPool/CscRawDataContainer_p4.h"
#include "CscRawDataCollectionCnv_p4.h"

/**
Transient/Persistent converter for a container of collections of CSC Raw Hits,
*/
class CscRawDataContainerCnv_p4
    : public T_AthenaPoolTPCnvIDCont< CscRawDataContainer, CscRawDataContainer_p4, CscRawDataCollectionCnv_p4>
{
public:
  CscRawDataContainerCnv_p4() {}

};


#endif
