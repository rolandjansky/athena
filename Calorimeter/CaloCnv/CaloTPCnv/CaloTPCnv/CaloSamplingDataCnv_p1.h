//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOSAMPLINGDATACNV_P1_H
#define CALOATHENAPOOL_CALOSAMPLINGDATACNV_P1_H

class CaloSamplingData;
class CaloSamplingData_p1;



//typedef T_AthenaPoolTPCnvVector<CaloSamplingdata, CaloSamplingdata_p1, CaloShowerCnv_p1> CaloSamplingdataCnvBasep1_t;

class CaloSamplingDataCnv_p1 {
public:
  CaloSamplingDataCnv_p1();

  void persToTrans(const CaloSamplingData_p1*, CaloSamplingData* /*, MsgStream &log*/) const;
  void transToPers(const CaloSamplingData*, CaloSamplingData_p1* /*, MsgStream &log*/) const;
};


#endif
