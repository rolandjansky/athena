/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACOLLECTIONCNV_P3_H
#define MUON_CSCCRAWDATACOLLECTIONCNV_P3_H

#include "MuonRDO/CscRawDataCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/CscRawDataCollection_p3.h"

#include "CscRawDataCnv_p3.h"


typedef  T_AthenaPoolTPCnvVector< CscRawDataCollection, CscRawDataCollection_p3, CscRawDataCnv_p3 >	CscRawDataCollection_Cnv_p3_base_t;

/**
Transient/Persistent converter for the Collection of CSC Raw Hits,
*/

class CscRawDataCollectionCnv_p3
   : public CscRawDataCollection_Cnv_p3_base_t
{
public:
  CscRawDataCollectionCnv_p3() {}

  virtual void	persToTrans(const CscRawDataCollection_p3* persColl, CscRawDataCollection* transColl, MsgStream &log) ;
  virtual void	transToPers(const CscRawDataCollection* transColl, CscRawDataCollection_p3* persColl, MsgStream &log) ;
};


#endif




