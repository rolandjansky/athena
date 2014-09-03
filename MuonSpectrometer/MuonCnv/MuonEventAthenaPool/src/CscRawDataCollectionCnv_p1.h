/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACOLLECTIONCNV_P1_H
#define MUON_CSCCRAWDATACOLLECTIONCNV_P1_H

/*

Transient/Persistent converter for the Collection of CSC Raw Hits,
Author: Marcin Nowak
        CERN, January, 2006

*/
#include "MuonRDO/CscRawDataCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/CscRawDataCollection_p1.h"
#include "CscRawDataCnv_p1.h"


typedef  T_AthenaPoolTPCnvVector< CscRawDataCollection, CscRawDataCollection_p1, CscRawDataCnv_p1 >	CscRawDataCollection_Cnvp1_base_t;

class CscRawDataCollectionCnv_p1
   : public CscRawDataCollection_Cnvp1_base_t
{
public:
  CscRawDataCollectionCnv_p1() {}

  virtual void	persToTrans(const CscRawDataCollection_p1* persColl, CscRawDataCollection* transColl, MsgStream &log) ;
  virtual void	transToPers(const CscRawDataCollection* transColl, CscRawDataCollection_p1* persColl, MsgStream &log) ;
};


#endif




