/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// NOTE: This is an exact copy of CscRawDataCollectionCnv_p3, since from p3 to p4 only the meaning of the m_hashId changed:
// Until p3, the m_hashId stores the identifier hash which encodes the geometrical position of the CSC
// From p4 onwards, the m_hashId stores the position of the CSC identifier in the vector of identifiers

#ifndef MUON_CSCCRAWDATACOLLECTIONCNV_P4_H
#define MUON_CSCCRAWDATACOLLECTIONCNV_P4_H

#include "MuonRDO/CscRawDataCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/CscRawDataCollection_p4.h"

#include "CscRawDataCnv_p4.h"


typedef  T_AthenaPoolTPCnvVector< CscRawDataCollection, CscRawDataCollection_p4, CscRawDataCnv_p4 >	CscRawDataCollection_Cnv_p4_base_t;

/**
Transient/Persistent converter for the Collection of CSC Raw Hits,
*/

class CscRawDataCollectionCnv_p4
   : public CscRawDataCollection_Cnv_p4_base_t
{
public:
  CscRawDataCollectionCnv_p4() {}

  virtual void	persToTrans(const CscRawDataCollection_p4* persColl, CscRawDataCollection* transColl, MsgStream &log) ;
  virtual void	transToPers(const CscRawDataCollection* transColl, CscRawDataCollection_p4* persColl, MsgStream &log) ;
};


#endif

