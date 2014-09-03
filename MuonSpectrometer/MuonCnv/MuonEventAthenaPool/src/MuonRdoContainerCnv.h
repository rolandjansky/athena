/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  custom POOL converter for Muon RDO

  @author Tadashi Maeno 
       based on SCT_RDO_ContainerCnv by RD Schaffer

       * T = RDO container class
*/

#ifndef MUONEVENTATHENAPOOL_MUONRDOCONTAINERCNV_H
#define MUONEVENTATHENAPOOL_MUONRDOCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"
#include "DataModel/DataVector.h"

class StoreGateSvc;

template <class T>
class MuonRdoContainerCnv
: public T_AthenaPoolCustCnv<T, DataVector<typename T::IDENTIFIABLE> >
{
  friend class CnvFactory<MuonRdoContainerCnv<T> >;

public:
  MuonRdoContainerCnv(ISvcLocator* svcloc);
  virtual ~MuonRdoContainerCnv();

  StatusCode initialize();

  /// Extend base-class conversion methods
  StatusCode transToPers(T* rdoC,  DataVector<typename T::IDENTIFIABLE>*& persObj);
  StatusCode persToTrans(T*& rdoC, DataVector<typename T::IDENTIFIABLE>* colV);

private:
  typedef typename T::IDENTIFIABLE COLLECTION_t;
  typedef DataVector<COLLECTION_t> COLL_vector;
  COLL_vector   m_colV;
  StoreGateSvc* m_storeGate;
};

#include "MuonRdoContainerCnv.icc"

#endif


