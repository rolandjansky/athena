/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACOLLECTIONCNV_P2_H
#define MUON_CSCCRAWDATACOLLECTIONCNV_P2_H

/*

Transient/Persistent converter for the Collection of CSC Raw Hits,
Author: Marcin Nowak
        CERN, January, 2006

*/
#include "MuonRDO/CscRawDataCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/CscRawDataCollection_p2.h"

#include "CscRawDataCnv_p2.h"


typedef  T_AthenaPoolTPCnvVector< CscRawDataCollection, CscRawDataCollection_p2, CscRawDataCnv_p2 >	CscRawDataCollection_Cnv_p2_base_t;

class CscRawDataCollectionCnv_p2
   : public CscRawDataCollection_Cnv_p2_base_t
{
public:
  CscRawDataCollectionCnv_p2() {}

  virtual void	persToTrans(const CscRawDataCollection_p2* persColl, CscRawDataCollection* transColl, MsgStream &log) ;
  virtual void	transToPers(const CscRawDataCollection* transColl, CscRawDataCollection_p2* persColl, MsgStream &log) ;
};


#endif




