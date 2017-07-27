/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MUONRDOCONTAINERTPCNV_H
#define MUONEVENTATHENAPOOL_MUONRDOCONTAINERTPCNV_H

/*
  custom POOL TP converter template for Muon RDO

  @author Marcic Nowak,   CERN, Dec 2005 
  
       T = Transient RDO container class
       P = Persistent RDO container class (corresponding to T)
       CONV = TP converter between T and P
*/

#include "AthContainers/DataVector.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "MuonEventAthenaPool/CreateTransientTemplates.h"

template <class T, class P, class CONV>
class MuonRdoContainerTPCnv
   : public T_AthenaPoolCustomCnv<T, P >
{
friend class CnvFactory<MuonRdoContainerTPCnv<T, P, CONV > >;

protected:
  MuonRdoContainerTPCnv(ISvcLocator* svcloc);

  /// Extend base-class conversion methods
  virtual P*	createPersistent(T* rdoC);
  virtual T*	createTransient ();

protected: 
  typedef typename T::IDENTIFIABLE COLLECTION_t;
  typedef DataVector<COLLECTION_t> COLL_vector;

  virtual T* createTransientFrom_p0(COLL_vector* persObj, MsgStream &log);

  /** convertor*/
  CONV   m_TPconverter;
};

#include "MuonRdoContainerTPCnv.icc"

#endif


