/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @class  : TrigSpacePointCountsCnv
 *
 * @brief transient persistent converter for TrigSpacePointCounts
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGSPACEPOINTCOUNTSCNV_H
#define TRIGEVENTATHENAPOOL_TRIGSPACEPOINTCOUNTSCNV_H

#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p3.h"

class TrigSpacePointCounts_p3;

// typedef to the latest persistent version

typedef TrigSpacePointCounts_p3  TrigSpacePointCounts_PERS;

typedef T_AthenaPoolCustomCnv<TrigSpacePointCounts, TrigSpacePointCounts_PERS > TrigSpacePointCountsCnvBase; 

class TrigSpacePointCountsCnv : public TrigSpacePointCountsCnvBase {

  friend class CnvFactory<TrigSpacePointCountsCnv>;

  virtual StatusCode initialize();

public:
  TrigSpacePointCountsCnv(ISvcLocator *svcloc);
  ~TrigSpacePointCountsCnv();

protected:
  virtual TrigSpacePointCounts_PERS  *createPersistent(TrigSpacePointCounts *transObj);
  virtual TrigSpacePointCounts       *createTransient();

private:
  TrigSpacePointCountsCnv_p1 m_converter_p1;
  TrigSpacePointCountsCnv_p2 m_converter_p2;
  TrigSpacePointCountsCnv_p3 m_converter_p3;
};


#endif
