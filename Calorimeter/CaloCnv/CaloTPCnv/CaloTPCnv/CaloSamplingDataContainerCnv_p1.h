//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOSAMPLINGDATACONTAINERCNV_P1_H
#define CALOATHENAPOOL_CALOSAMPLINGDATACONTAINERCNV_P1_H

#include "CaloTPCnv/CaloSamplingDataContainer_p1.h"
#include "GaudiKernel/MsgStream.h"
class CaloSamplingData;


class CaloSamplingDataContainerCnv_p1 {
public:
  void persToTrans(const CaloSamplingDataContainer_p1* pers, 
		   CaloSamplingData* trans);
  void transToPers(const CaloSamplingData* trans, 
		   CaloSamplingDataContainer_p1* pers, MsgStream& log);

  // To be called at the beginning of the container before P->T
  bool setIterator(const CaloSamplingDataContainer_p1* pers,
                   unsigned int ncluster);

  CaloSamplingDataContainer_p1::const_iterator m_samplingStoreIterator;
  std::vector<unsigned int>::const_iterator m_varTypePatternsIterator;
  size_t m_nPersSamplings;
};



#endif
