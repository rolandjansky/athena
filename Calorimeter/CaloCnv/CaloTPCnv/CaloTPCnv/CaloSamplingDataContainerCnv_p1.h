//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOSAMPLINGDATACONTAINERCNV_P1_H
#define CALOATHENAPOOL_CALOSAMPLINGDATACONTAINERCNV_P1_H

#include "CaloTPCnv/CaloSamplingDataContainer_p1.h"
#include "GaudiKernel/MsgStream.h"
class CaloSamplingData;


class CaloSamplingDataContainerCnv_p1 {
public:
  struct State
  {
    CaloSamplingDataContainer_p1::const_iterator m_samplingStoreIterator;
    std::vector<unsigned int>::const_iterator m_varTypePatternsIterator;
    size_t m_nPersSamplings;
  };


  void persToTrans(const CaloSamplingDataContainer_p1* pers, 
		   CaloSamplingData* trans,
                   State& state) const;
  void transToPers(const CaloSamplingData* trans, 
		   CaloSamplingDataContainer_p1* pers, MsgStream& log) const;

  // To be called at the beginning of the container before P->T
  bool setState(const CaloSamplingDataContainer_p1* pers,
                unsigned int ncluster,
                State& state) const;
};



#endif
