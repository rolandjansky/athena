/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigSpacePointCountsCnv
 *
 * @brief transient persistent converter for TrigSpacePointCounts
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCountsCnv.h,v 1.4 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGSPACEPOINTCOUNTSCNV_H
#define TRIGEVENTATHENAPOOL_TRIGSPACEPOINTCOUNTSCNV_H

#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigSpacePointCounts_p3;

// typedef to the latest persistent version

typedef TrigSpacePointCounts_p3  TrigSpacePointCounts_PERS;

typedef T_AthenaPoolCustomCnv<TrigSpacePointCounts, TrigSpacePointCounts_PERS > TrigSpacePointCountsCnvBase; 

class TrigSpacePointCountsCnv : public TrigSpacePointCountsCnvBase {

  friend class CnvFactory<TrigSpacePointCountsCnv>;

  virtual StatusCode initialize();

 protected:
  TrigSpacePointCountsCnv(ISvcLocator *svcloc);
  ~TrigSpacePointCountsCnv();

  virtual TrigSpacePointCounts_PERS  *createPersistent(TrigSpacePointCounts *transObj);
  virtual TrigSpacePointCounts       *createTransient();
};


#endif
