//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOATHENAPOOL_CALOCLUSTERMOMENTCONTAINERCNV_P1_H
#define CALOATHENAPOOL_CALOCLUSTERMOMENTCONTAINERCNV_P1_H


#include "CaloTPCnv/CaloClusterMomentContainer_p1.h"
//class CaloClusterMomentContainer_p1;   
//class CaloClusterMomentContainer_p1::const_iterator;   

class CaloClusterMomentStore;

class CaloClusterMomentContainerCnv_p1 {

 public:
  CaloClusterMomentContainerCnv_p1();
  //~CaloClusterMomentContainerCnv_p1 {};

  void persToTrans(const CaloClusterMomentContainer_p1* pers,
		   CaloClusterMomentStore* trans);


  void transToPers(const CaloClusterMomentStore* trans, 
		   CaloClusterMomentContainer_p1* pers);

  //To be called at the begining of the container for a P->T conversion
  bool setIterator(const CaloClusterMomentContainer_p1* pers,
                   unsigned int ncluster);
       
 private:
  CaloClusterMomentContainer_p1::const_iterator m_momentStoreIterator;
};

#endif
