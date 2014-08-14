/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigTrackCountsCnv
 *
 * @brief transient persistent converter for TrigTrackCounts
 *
 * @author Regina Kwee      <Regina.Kwee@cern.ch>      - CERN
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTrackCountsCnv.h,v 1.3 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTRACKCOUNTERCNV_H
#define TRIGEVENTATHENAPOOL_TRIGTRACKCOUNTERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEvent/TrigTrackCounts.h"
class TrigTrackCounts_p2;

// typedef to the latest persistent version

typedef TrigTrackCounts_p2 TrigTrackCounts_PERS;

typedef T_AthenaPoolCustomCnv<TrigTrackCounts, TrigTrackCounts_PERS > TrigTrackCountsCnvBase; 

class TrigTrackCountsCnv : public TrigTrackCountsCnvBase {

  friend class CnvFactory<TrigTrackCountsCnv>;

 protected:
  TrigTrackCountsCnv(ISvcLocator *svcloc);
  ~TrigTrackCountsCnv();

  virtual TrigTrackCounts_PERS  *createPersistent(TrigTrackCounts *transObj);
  virtual TrigTrackCounts       *createTransient();
};


#endif


